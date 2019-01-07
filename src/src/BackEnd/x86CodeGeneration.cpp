#include "x86CodeGenaration.h"
X86CodeGeneration::X86CodeGeneration(int cpuCoreNum, SchedulerSSG *sssg, const char *, StageAssignment *psa, Partition *mp)
{
    psa_ = psa;
    sssg_ = sssg;
    mp_ = mp;
    flatNodes_ = sssg->GetFlatNodes();
    nCpucore_ = cpuCoreNum;
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
    int cnt=1;
    for(auto it:typeSet){
        buf<<"\t"<<it<<" "<<it[0]<<to_string(cnt++)<<";\n";
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
    buf << "#include \"global.h\"\n";
    buf << "#include <vector>\n";
    buf << "using namespace std;\n";
    ofstream out("Global.cpp");
    out << buf.str();
}