#include "x86CodeGenaration.h"
X86CodeGeneration::X86CodeGeneration(int cpuCoreNum, SchedulerSSG *sssg, const char *, StageAssignment *psa, Partition *mp)
{
    psa_ = psa;
    sssg_ = sssg;
    mp_ = mp;
    flatNodes_ = sssg->GetFlatNodes();
    nCpucore_ = cpuCoreNum;
    nActors_ = flatNodes_.size();
    //建立mapActor2InEdge，mapActor2OutEdge
    for (auto iter1 : flatNodes_)
    {
        for (auto iter2 : iter1->inFlatNodes)
        {
            string str = iter2->name + "_" + iter1->name; //得到形如A_B的边名称
            mapActor2InEdge.insert(make_pair(iter1, str));
        }
        for (auto iter3 : iter1->outFlatNodes)
        {
            string str = iter1->name + "_" + iter3->name;
            mapActor2OutEdge.insert(make_pair(iter1, str));
        }
    }
    //构造每个线程上的stage集合mapNum2Stage
    for (int i = 0; i < nCpucore_; i++)
    {
        set<int> tempstageset;
        tempstageset.clear();
        vector<FlatNode *> tempactors = mp->findNodeSetInPartition(i); //根据线程号找到actor集合
        //遍历actor
        for (auto iter : tempactors)
        {
            tempstageset.insert(psa_->FindStage(iter));
        }
        mapNum2Stage.insert(make_pair(i, tempstageset));
    }
}

void X86CodeGeneration::CGMakefile()
{
    stringstream buf;
    buf << "PROGRAM := a.out\n";
    buf << "SOURCES := $(wildcard ./*.cpp)\n";
    buf << "SOURCES += $(wildcard ./src/*.cpp)\n";
    buf << "OBJS    := $(patsubst %.cpp,%.o,$(SOURCES))\n";
    buf << "CC      := g++\n";
    buf << "CFLAGS  := -ggdb -Wall \n";
    buf << "INCLUDE := -I .\n";
    buf << "LIB     := -lpthread -ldl\n\n";
    buf << ".PHONY: clean install\n\n";
    buf << "$(PROGRAM): $(OBJS)\n";
    buf << "\t$(CC) -o $@ $^ $(LIB)\n";
    buf << "%.o: %.c\n";
    buf << "\t$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDE)\n\n";
    buf << "clean:\n";
    buf << "\trm $(OBJS) $(PROGRAM) -f\n";
    buf << "install: $(PROGRAM)\n";
    buf << "\tcp $(PROGRAM) ./bin/";
    ofstream out("Makefile");
    out << buf.str();
}
/* 遍历语法树，找到所有为declareNode,调用toString（）方法，写入生成文件 */
void X86CodeGeneration::CGGlobalvar()
{
    stringstream buf;
    buf << "#include \"GlobalVar.h\"\n";
    for (auto iter : *Program)
    {
        if (iter->type == Decl)
            buf << iter->toString() << "\n";
    }
    ofstream out("GlobalVar.cpp");
    out << buf.str();
}
/* 生成全局变量的声明 */
void X86CodeGeneration::CGGlobalvarHeader()
{
    stringstream buf, res;
    buf << "#ifndef GLOBALVAL_H\n";
    buf << "#define GLOBALVAL_H\n";
    for (auto iter : *Program)
    {
        if (iter->type == Decl)
            buf << "extern " + iter->toString() << "\n";
    }
    buf << "#endif";
    string line;
    bool flag = false;
    while (getline(buf, line))
    {
        string str;
        auto pos = line.find("=");
        if (pos != string::npos)
        {
            str = line.substr(0, pos) + ";";
        }
        else
            str = line;
        res << str << "\n";
    }
    ofstream out("GlobalVar.h");
    out << res.str();
}

//生成stream流类型和全局数据流缓存区的声明
/*边的命名规则：A_B,其中A->B*/
void X86CodeGeneration::CGGlobalHeader()
{
    stringstream buf;
    buf << "#ifndef _GLOBAL_H\n";
    buf << "#define _GLOBAL_H\n";
    buf << "#include \"Buffer.h\"\n";
    buf << "#include <math.h>\n";
    buf << "#include <string>\n";
    buf << "using namespace std;\n";
    //遍历所有compositeNode的streamType，找到流中所有包含的数据类型，作为结构体streamData中的数据
    set<string> typeSet;
    for (auto iter1 : *Program)
    {
        if (iter1->type == Composite)
        {
            compositeNode *comNode = static_cast<compositeNode *>(iter1);
            compHeadNode *head = comNode->head;
            ComInOutNode *inout = head->inout;
            if (inout != NULL)
            {
                list<Node *> *inputs = inout->input_List;
                list<Node *> *outputs = inout->output_List;
                if (inputs != NULL)
                {
                    for (auto it : *inputs)
                    {
                        auto nd = static_cast<inOutdeclNode *>(it)->strType;
                        string temp = static_cast<strdclNode *>(nd)->toString();
                        string str = temp.substr(0, temp.find(' '));
                        typeSet.insert(str);
                    }
                }
                if (outputs != NULL)
                {
                    for (auto it : *outputs)
                    {
                        auto nd = static_cast<inOutdeclNode *>(it)->strType;
                        string temp = static_cast<strdclNode *>(nd)->toString();
                        string str = temp.substr(0, temp.find(' '));
                        typeSet.insert(str);
                    }
                }
            }
        }
    }
    //写入数据流数据类型结构体
    buf << "struct streamData{\n";
    int cnt = 1;
    for (auto it : typeSet)
    {
        buf << "\t" << it << " " << it[0] << to_string(cnt++) << ";\n";
    }
    buf << "};\n";
    for (auto iter1 : flatNodes_)
    {
        for (auto iter2 : iter1->outFlatNodes)
        {
            string edgename = iter1->name + "_" + iter2->name;
            buf << "extern Buffer<"
                << "streamData"
                << "> " << edgename << ";" << endl;
        }
    }
    buf << "#endif\n";
    ofstream out("Global.h");
    out << buf.str();
}

/*全局变量，用于存储边的信息*/
void X86CodeGeneration::CGGlobal()
{
    stringstream buf;
    buf << "#include \"Buffer.h\"\n";
    buf << "#include \"Global.h\"\n";
    buf << "#include <vector>\n";
    buf << "using namespace std;\n";
    int init1, init2;                                                            //发送actor和接受actor初态调度产生和接受的数据量
    for (auto iter_1 = flatNodes_.begin(); iter_1 != flatNodes_.end(); ++iter_1) //遍历所有结点
    {
        for (auto iter_2 = (*iter_1)->outFlatNodes.begin(); iter_2 != (*iter_1)->outFlatNodes.end(); iter_2++)
        {
            int stageminus;                                                                                  //stageminus表示两个actor所分配的阶段差
            int size;                                                                                        //缓冲区的大小
            string edgename = (*iter_1)->name + "_" + (*iter_2)->name;                                       //边的名称
            stageminus = psa_->FindStage(*iter_2) - psa_->FindStage(*iter_1);                                //发送方和接受方的软件流水阶段差
            int edgePos = iter_2 - (*iter_1)->outFlatNodes.begin();                                          //iter_2在iter_1的输出边的序号
            int perSteadyPushCount = sssg_->GetSteadyCount(*iter_1) * (*iter_1)->outPushWeights.at(edgePos); //发送actor每次调用steadywork需要push的个数,nc
            int copySize = 0, copyStartPos = 0;                                                              //拷贝的数据大小，copy存放的开始位置
            for (int inEdgeIndex = 0; inEdgeIndex < (*iter_2)->inFlatNodes.size(); inEdgeIndex++)
                if ((*iter_2)->inFlatNodes.at(inEdgeIndex) == (*iter_1))
                {
                    int perWorkPeekCount = (*iter_2)->inPeekWeights[inEdgeIndex];                 //接收边actor每次peek的个数,b
                    int perWorkPopCount = (*iter_2)->inPopWeights[inEdgeIndex];                   //接收边actor每次调用work需要pop的个数,c
                    init1 = sssg_->GetInitCount(*iter_1) * (*iter_1)->outPushWeights.at(edgePos); //发送actor调用initwork产生的数据量
                    init2 = sssg_->GetInitCount(*iter_2) * perWorkPopCount;
                    size = init1 + perSteadyPushCount * (stageminus + 2);
                    if (perWorkPeekCount == perWorkPopCount) //peek == pop
                    {
                        if (perSteadyPushCount)
                        {
                            copySize = (init1 - init2) % perSteadyPushCount;
                            copyStartPos = init2 % perSteadyPushCount;
                        }
                    }
                    //peek != pop
                    else
                    {
                        int leftnum = ((init1 - init2) % perSteadyPushCount + perSteadyPushCount - (perWorkPeekCount - perWorkPopCount) % perSteadyPushCount) % perSteadyPushCount;
                        copySize = leftnum + perWorkPeekCount - perWorkPopCount;
                        int addtime = copySize % perSteadyPushCount ? copySize / perSteadyPushCount + 1 : copySize / perSteadyPushCount;
                        copyStartPos = init2 % perSteadyPushCount;
                        size += addtime * perSteadyPushCount;
                    }
                    buf << "Buffer<"
                        << "streamData"
                        << "> " << edgename << "(" << size << "," << copySize << "," << copyStartPos << ");\n";
                    break;
                }
        }
    }
    ofstream out("Global.cpp");
    out << buf.str();
}

void X86CodeGeneration::CGactors()
{
    //生成各个类的计算函数
    for (int i = 0; i < nActors_; ++i)
    {
        stringstream buf;
        string className = flatNodes_[i]->name;
        buf << "#ifndef _" << className << "_\n";
        buf << "#define _" << className << "_\n";
        buf << "#include <string>\n";
        buf << "#include <iostream>\n";
        buf << "#include \"Buffer.h\"\n";
        buf << "#include \"Consumer.h\"\n";
        buf << "#include \"Producer.h\"\n";
        buf << "#include \"Global.h\"\n";
        buf << "#include \"GlobalVar.h\"\n";
        buf << "using namespace std;\n";
        buf << "class " << className << "{\n"; // 类块开始
        //获取节点类的输入输出边的名字
        string inputEdgeName = "";
        string outputEdgeName = "";
        auto oper = flatNodes_[i]->contents;
        list<Node *> *inputs = oper->inputs;
        list<Node *> *outputs = oper->outputs;
        if (inputs != NULL)
        {
            Node *top = inputs->front();
            inputEdgeName = ((idNode *)top)->name;
        }
        if (outputs != NULL)
        {
            Node *top = outputs->front();
            outputEdgeName = ((idNode *)top)->name;
        }
        buf << "public:\n";
        //写入构造函数
        CGactorsConstructor(flatNodes_[i], buf, className, inputEdgeName, outputEdgeName);
        //写入成员变量
        buf << "private:\n";
        buf << "\tProducer<streamData> " << outputEdgeName << ";\n";
        buf << "\tConsumer<streamData> " << inputEdgeName << ";\n";
        buf << "\tint steadyScheduleCount;\t//稳态时一次迭代的执行次数\n";
        buf << "\tint initScheduleCount;\n";
        //写入init部分前的statement定义，调用tostring()函数，解析成规范的类变量定义格式
        operBodyNode *body = oper->operBody;
        list<Node *> *stmts = body->stmt_list;
        if (stmts != NULL)
        {
            for (auto it : *stmts)
            {
                string str = it->toString();
                /*解析等号类似int i=0,j=1形式变成int i,j的形式,变量定义不能初始化*/
                string temp = "";
                bool flag = 1;
                for (auto c : str)
                {
                    if (c == ',' || c == ';')
                        flag = true;
                    if (c != '=' && flag)
                        temp += c;
                    else
                    {
                        flag = false;
                    }
                }
                buf << "\t" << temp << "\n";
            }
        }
        //写入init部分前的statement赋值
        buf<<"\tvoid initVarAndState() {\n";
        buf<<"\t\t\n";
        buf<<"\t}\n";

        buf << "};\n";
        buf << "#endif";

        className += ".h";
        ofstream out(className);
        out << buf.str();
    }
}

//actors constructor
void X86CodeGeneration::CGactorsConstructor(FlatNode *actor, stringstream &buf, string className, string inputEdgeName, string outputEdgeName)
{
    buf << "\t" << className << "(";
    if (outputEdgeName != "" && inputEdgeName != "")
    {
        buf << "Buffer<streamData>& " << outputEdgeName << ",";
        buf << "Buffer<streamData>& " << inputEdgeName;
    }
    else if (outputEdgeName != "")
        buf << "Buffer<streamData>& " << outputEdgeName;
    else if (inputEdgeName != "")
        buf << "Buffer<streamData>& " << inputEdgeName;
    buf << "):";
    if (outputEdgeName != "" && inputEdgeName != "")
    {
        buf << outputEdgeName << "(" << outputEdgeName << "),";
        buf << inputEdgeName << "(" << inputEdgeName << ")";
    }
    else if (outputEdgeName != "")
        buf << outputEdgeName << "(" << outputEdgeName << ")";
    else if (inputEdgeName != "")
        buf << inputEdgeName << "(" << inputEdgeName << ")";
    buf << "{\n";
    buf << "\t\tsteadyScheduleCount = " << sssg_->GetSteadyCount(actor) << ";\n";
    buf << "\t\tinitScheduleCount = " << sssg_->GetInitCount(actor) << ";\n";
    buf << "\t}\n";
}

void X86CodeGeneration::CGThreads()
{
}