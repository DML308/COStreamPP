#include "x86CodeGenaration.h"

static bool IOHandler = false;//是否有IO操作节点，如果没有则进入正常工作流程

X86CodeGeneration::X86CodeGeneration(int cpuCoreNum, SchedulerSSG *sssg, const char *, StageAssignment *psa, Partition *mp)
{
    psa_ = psa;
    sssg_ = sssg;
    mp_ = mp;
    flatNodes_ = sssg->GetFlatNodes();
    nCpucore_ = cpuCoreNum;
    nActors_ = flatNodes_.size();

    //IO所需要的变量
    workLen = 0;
    fileReaderActor = NULL;

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
        if((iter1->name).find("FILEREADER")!=-1)
        {
            //如果找到了名字为FileReader的节点则证明有IO读取操作，保存节点的结构
            fileReaderActor = iter1;
            //进入带IO的操作流程，否则与原来一致
            IOHandler = true;
            //输出IO头节点名字，测试用
            cout<<iter1->name<<endl;
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
            buf << iter->toString()+ ";" << "\n";
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
    map<string, string> typeSet;
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
                        string type = temp.substr(0, temp.find(' '));
                        string data = temp.substr(temp.find(' '), temp.size() - 1);
                        typeSet.insert(make_pair(type, data));
                    }
                }
                if (outputs != NULL)
                {
                    for (auto it : *outputs)
                    {
                        auto nd = static_cast<inOutdeclNode *>(it)->strType;
                        string temp = static_cast<strdclNode *>(nd)->toString();
                        string type = temp.substr(0, temp.find(' '));
                        string data = temp.substr(temp.find(' '), temp.size() - 1);
                        typeSet.insert(make_pair(type, data));
                    }
                }
            }
            for(auto it : *(((compositeNode*)iter1)->body->stmt_List)){
                if(it->type == StrDcl){
                    for(auto stdcl : ((strdclNode*)it)->id_list){
                        cout<< stdcl->name<<stdcl->valType<<endl;
                        typeSet.insert(make_pair(stdcl->valType,stdcl->name));
                    }
                }
            }
            cout<<"end"<<endl;
        }
    }
    //写入数据流数据类型结构体
    buf << "struct streamData{\n";
    int cnt = 1;
    for (auto it : typeSet)
    {
        buf << "\t" << it.first << " " << it.second << ";\n";
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
    //为特殊的缓冲区分配内存
    int init1, init2;                                                            //发送actor和接受actor初态调度产生和接受的数据量
    for (auto iter_1 = flatNodes_.begin(); iter_1 != flatNodes_.end(); ++iter_1) //遍历所有结点
    {
        for (auto iter_2 = (*iter_1)->outFlatNodes.begin(); iter_2 != (*iter_1)->outFlatNodes.end(); iter_2++)
        {
            bool special = true;                                                                             //缓冲区是否能重复利用,若为真则不能重复利用
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

                        //只有在peek=pop且上下游节点不存在阶段差的时候才能实现内存共享
                        if(copySize==0&&copyStartPos==0&&stageminus==0)
                            special = false;
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
                    if(special)//如果是特殊缓冲区则单独分配内存
                    {
                        buf << "Buffer<streamData> " << edgename << "(" << size << "," << copySize << "," << copyStartPos << ");\n";
                        bufferSpace bspace;
                        bspace.original = edgename;
                        bspace.instance = edgename;
                        bspace.buffersize = size;
                        bspace.buffertype = 1;

                        bufferMatch[bspace.original] = bspace;
                    }
                    break;
                }
        }
    }
    //为同一核上可以共享内存的缓冲区分配内存
    //根据拓扑排序模拟运行各个CPU上的计算节点找出可以被复用的缓冲区
    map<int,vector<FlatNode*>> processor2topoactors = psa_->processor2topoactors;
    map<int,vector<FlatNode*>>::iterator miter = processor2topoactors.begin();
    while(miter!=processor2topoactors.end())
    {
        vector<FlatNode*> nodes = miter->second;//获取同一个CPU上拓扑排序后的节点
        vector<bufferSpace> vb;//用来存储在一次稳态调度中已经使用完的缓冲区
        vector<string> alloc;//用来存储实际分配的缓冲区，string代表缓冲区名称，利用bufferMatch找到实际的缓冲区
        vector<pair<int,int>> allocRecord;//与alloc一一对应，存储缓冲区的copySize和copyStartPos

        //按拓扑排序访问各个节点
        for(int i=0;i<nodes.size();i++)
        {
            vector<FlatNode*> downNodes = nodes[i]->outFlatNodes;//找出所有与该节点相连的下游节点准备分配内存
            for(int j=0;j<downNodes.size();j++)
            {
                string edgename = nodes[i]->name+"_"+downNodes[j]->name;
                if(bufferMatch.find(edgename)!=bufferMatch.end())//如果该边为特殊缓冲区在之前已经分配完成则进入下一条边的分配
                    continue;

                //计算所需要占用的缓冲区大小-开始
                FlatNode* father = nodes[i];
                FlatNode* child = downNodes[j];
                int stageminus;//表示两个actor所分配的阶段差
                int size;//缓冲区的大小
                stageminus = psa_->FindStage(child)-psa_->FindStage(father);//发送方和接受方的软件流水阶段差
                vector<FlatNode*>::iterator iter = father->outFlatNodes.begin();
                    
                int edgePos = 0;
                //获取父节点outFlatNodes的迭代器，如果与子节点相等代表该子节点在父节点的第几条边上
                while (iter != father->outFlatNodes.end())
                {
                        if (*iter == child)
                            break;
                        edgePos++;
                        iter++;
                }
                int perSteadyPushCount = sssg_->GetSteadyCount(father)*father->outPushWeights.at(edgePos);//稳态时产生的数据量
                int copySize = 0, copyStartPos = 0;
                size = perSteadyPushCount*(stageminus + 2);
                //计算所需占用的缓冲区大小-结束

                //分配时首先搜索队列中是否有已经使用完的缓冲区,没有再自己分配内存，使用队列中的缓冲区要将其从队列中删除
                bool flag = false;//缓冲区是否分配完成
                if (!vb.empty())//代表有可以复用的缓冲区
                {
                        vector<bufferSpace>::iterator it = vb.begin();
                        //寻找当前可用缓冲区中最小的进行内存共享
                        while (it != vb.end())
                        {
                            if (it->buffersize >= size)
                            {
                                bufferSpace bspace;
                                bspace.original = edgename;
                                bspace.instance = it->instance;
                                bspace.buffersize = it->buffersize;
                                bspace.buffertype = 2;

                                bufferMatch[bspace.original] = bspace;
                                flag = true;
                                vb.erase(it);
                                break;
                            }
                            it++;
                        }

                        //若当前可用缓冲区大小都不符合要求则对最大缓冲区进行扩容
                        if(!flag)
                        {
                            auto iter = vb.end()-1;
                            iter->buffersize = size;
                            bufferMatch[iter->instance].buffersize = size;
                            bufferMatch[iter->original].buffersize = size;

                            bufferSpace bspace;
                            bspace.original = edgename;
                            bspace.instance = iter->instance;
                            bspace.buffersize = iter->buffersize;
                            bspace.buffertype = 2;

                            bufferMatch[bspace.original] = bspace;
                            flag = true;
                            vb.erase(iter);
                        }
                }
                    
                //找不到可以复用的缓冲区，自己进行分配
                if (!flag)
                {
                    //buf << "Buffer<streamData> " << edgename << "(" << size << "," << copySize << "," << copyStartPos << ");\n";
                    bufferSpace bspace;
                    bspace.original = edgename;
                    bspace.instance = edgename;
                    bspace.buffersize = size;
                    bspace.buffertype = 2;

                    alloc.push_back(bspace.original);//实际分配内存缓冲区的
                    allocRecord.push_back(make_pair(copySize,copyStartPos));

                    bufferMatch[bspace.original] = bspace;
                }
            }

            //由于空闲缓冲区队列中的缓冲区与原始缓冲区存在映射关系，所以要更新其缓冲区大小
            for(int j=0;j<vb.size();j++)
            {
                vb[j].buffersize = bufferMatch[vb[j].instance].buffersize;
            }

            //当该节点内存分配完之后说明该节点执行完毕，可以将节点上游能够复用的缓冲区加入到队列中
            vector<FlatNode*> upNodes = nodes[i]->inFlatNodes;
            for (int j = 0; j < upNodes.size(); j++)
            {
                    string edgename = upNodes[j]->name+"_"+nodes[i]->name;
                    if (bufferMatch[edgename].buffertype == 2)
                        vb.push_back(bufferMatch[edgename]);
            }

            sort(vb.begin(), vb.end(), [](bufferSpace& a,bufferSpace& b){return a.buffersize<b.buffersize;});
        }

        //实际进行内存分配
        for(int i=0;i<alloc.size();i++)
        {
            buf << "Buffer<streamData> " << bufferMatch[alloc[i]].original << "(" << bufferMatch[alloc[i]].buffersize << "," << allocRecord[i].first << "," << allocRecord[i].second << ");\n";
        }
        miter++;
    }

    //测试是否有缓冲区没有分配
    for (vector<FlatNode *>::iterator iter_1 = flatNodes_.begin(); iter_1 != flatNodes_.end(); ++iter_1)
    {
        for (vector<FlatNode *>::iterator iter_2 = (*iter_1)->outFlatNodes.begin(); iter_2 != (*iter_1)->outFlatNodes.end(); iter_2++)
        {
            string edgename = (*iter_1)->name + "_" + (*iter_2)->name;
            if (bufferMatch.find(edgename) == bufferMatch.end())
                cout <<edgename <<" not exist"<< endl;
        }
    }

    ofstream out("Global.cpp");
    out << buf.str();
}
/* 生成各个类的计算函数 */
void X86CodeGeneration::CGactors()
{
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

        //如果当前节点为IO节点
        if(flatNodes_[i] == fileReaderActor)
        {
            buf << "#include \"RingBuffer.h\"\n";
            workLen = flatNodes_[i]->outPushWeights[0];
        	buf << "struct source{\n";
            //由于目前不支持多类型流变量，这里先强制设置为int
		    buf << "int" << " " << "buffer[" << workLen << "];\n";
		    buf << "};\n";
		    buf << "extern RingBuffer<source> ringBuffer;\n";
        }

        buf << "class " << className << "{\n"; // 类块开始
        vector<string> inEdgeName;
        vector<string> outEdgeName;
        auto oper = flatNodes_[i]->contents;
        operBodyNode *body = oper->operBody;
        list<Node *> stmts = body->stmt_list;
        Node *init = body->init;
        Node *work = body->work;
        list<Node *> *inputs = oper->inputs;
        list<Node *> *outputs = oper->outputs;
        if (inputs != NULL)
        {
            for (auto it : *inputs){
                inEdgeName.push_back(((idNode *)it)->name);
                string upNodeName = (sssg_->mapEdge2UpFlatNode)[((idNode *)it)->name]->name;
                string edgename = upNodeName+"_"+className;
                bufferType[((idNode *)it)->name] = bufferMatch[edgename].buffertype;//确定输入边类型
            }
        }
        if (outputs != NULL)
        {
            for (auto it : *outputs){
                outEdgeName.push_back(((idNode *)it)->name);
                string downNodeName = (sssg_->mapEdge2DownFlatNode)[((idNode *)it)->name]->name;
                string edgename = className+"_"+downNodeName;
                bufferType[((idNode *)it)->name] = bufferMatch[edgename].buffertype;//确定输出边类型
            }
        }
        buf << "public:\n";
        /*写入类成员函数*/
        CGactorsConstructor(flatNodes_[i], buf, className, inEdgeName, outEdgeName);
        CGactorsRunInitScheduleWork(buf, inEdgeName, outEdgeName, flatNodes_[i]);
        CGactorsRunSteadyScheduleWork(buf, inEdgeName, outEdgeName, flatNodes_[i]);
        /*写入类成员变量*/
        buf << "private:\n";
        for (auto out : outEdgeName)
            buf << "\tProducer<streamData> " << out << ";\n";
        for (auto in : inEdgeName)
            buf << "\tConsumer<streamData> " << in << ";\n";
        buf << "\tint steadyScheduleCount;\t//稳态时一次迭代的执行次数\n";
        buf << "\tint initScheduleCount;\n";
        //如果是IO头节点
        if(flatNodes_[i] == fileReaderActor)
        {
		    buf << "\tsource sourceBuff;\n";
        }
        //写入init部分前的statement定义，调用tostring()函数，解析成规范的类变量定义格式
        CGactorsStmts(buf, &stmts);
        CGactorsPopToken(buf, flatNodes_[i], inEdgeName);
        CGactorsPushToken(buf, flatNodes_[i], outEdgeName);
        //init部分前的statement赋值
        CGactorsinitVarAndState(buf, &stmts);
        /* composite 中init函数 */
        CGactorsInit(buf, init);
        /* composite中work函数 */
        CGactorsWork(buf, work, flatNodes_[i]);
        /* 类体结束*/
        buf << "};\n";
        buf << "#endif";
        className += ".h";
        ofstream out(className);
        out << buf.str();
    }
}

//actors constructor
void X86CodeGeneration::CGactorsConstructor(FlatNode *actor, stringstream &buf, string className, vector<string> inEdgeName, vector<string> outEdgeName)
{
    vector<string> svec(outEdgeName);
    svec.insert(svec.end(), inEdgeName.begin(), inEdgeName.end());
    buf << "\t" << className << "(";
    if (svec.size() != 0)
    {
        buf << "Buffer<streamData>& " << svec.front();
        for (auto edge = ++svec.begin(); edge != svec.end(); ++edge)
            buf << ",Buffer<streamData>& " << *edge;
    }
    buf << "):";
    if (svec.size() != 0)
    {
        buf << svec.front() << "(" << svec.front() << ")";
        for (auto edge = ++svec.begin(); edge != svec.end(); ++edge)
            buf << "," << *edge << "(" << *edge << ")";
    }
    buf << "{\n";
    buf << "\t\tsteadyScheduleCount = " << sssg_->GetSteadyCount(actor) << ";\n";
    buf << "\t\tinitScheduleCount = " << sssg_->GetInitCount(actor) << ";\n";
    buf << "\t}\n";
}

void X86CodeGeneration::CGactorsRunInitScheduleWork(stringstream &buf, vector<string> inEdgeName, vector<string> outEdgeName, FlatNode *actor)
{
    buf << "\tvoid runInitScheduleWork() {\n";
    buf << "\t\tinitVarAndState();\n";
    buf << "\t\tinit();\n";
    buf << "\t\tfor(int i=0;i<initScheduleCount;i++)\n";
    buf << "\t\t\twork();\n";
    if (outEdgeName.size() != 0)
        for (auto out : outEdgeName)
            buf << "\t\t" << out << ".resetTail();\n";
    if (inEdgeName.size() != 0)
        for (auto in : inEdgeName)
            buf << "\t\t" << in << ".resetHead();\n";
    buf << "\t}\n";
}
void X86CodeGeneration::CGactorsRunSteadyScheduleWork(stringstream &buf, vector<string> inEdgeName, vector<string> outEdgeName, FlatNode *actor)
{
    buf << "\tvoid runSteadyScheduleWork() {\n";
    buf << "\t\tfor(int i=0;i<steadyScheduleCount;i++)\n";
    buf << "\t\t\twork();\n";
    if (outEdgeName.size() != 0)
        for (auto out : outEdgeName){
            if(bufferType[out]==1)
                buf << "\t\t" << out << ".resetTail();\n";
            else if(bufferType[out]==2)//若为2代表可以共享内存，每次使用需要清空缓冲区
                buf << "\t\t" << out << ".resetTail2();\n";
        }
    if (inEdgeName.size() != 0)
        for (auto in : inEdgeName){
            if(bufferType[in]==1)
                buf << "\t\t" << in << ".resetHead();\n";
            else if(bufferType[in]==2)//若为2代表可以共享内存，每次使用需要清空缓冲区
                buf << "\t\t" << in << ".resetHead2();\n";
        }
    buf << "\t}\n";
}
void X86CodeGeneration::CGactorsStmts(stringstream &buf, list<Node *> *stmts)
{
    if (stmts != NULL)
    {
        for (auto it : *stmts)
        {
            string str = it->toString()+";";
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
}
void X86CodeGeneration::CGactorsPopToken(stringstream &buf, FlatNode *actor, vector<string> inEdgeName)
{
    buf << "\tvoid popToken() {\n";
    auto pop = actor->inPopWeights.begin();
    if (inEdgeName.size() != 0)
    {
        for (auto in : inEdgeName)
        {
            buf << "\t\t" << in << ".updatehead(" << *pop << ");\n";
            ++pop;
        }
    }
    buf << "\t}\n";
}
void X86CodeGeneration::CGactorsPushToken(stringstream &buf, FlatNode *actor, vector<string> outEdgeName)
{
    buf << "\tvoid pushToken() {\n";
    auto push = actor->outPushWeights.begin();
    if (outEdgeName.size() != 0)
    {
        for (auto out : outEdgeName)
        {
            buf << "\t\t" << out << ".updatetail(" << *push << ");\n";
            ++push;
        }
    }

    buf << "\t}\n";
}
void X86CodeGeneration::CGactorsinitVarAndState(stringstream &buf, list<Node *> *stmts)
{
    buf << "\tvoid initVarAndState() {\n";
    if (stmts != NULL)
    {
        for (auto it : *stmts)
        {
            string str = it->toString()+";";
            vector<string> svec;
            /*解析逗号和等号,类似int i=0,j;形式变成i=0;的形式,初始化stmts*/
            string temp = "";
            for (auto c : str)
            {
                if (c != ',' && c != ';')
                    temp += c;
                else
                {
                    //过滤不含等号的字符串
                    temp += ';';
                    if (temp.find('=') != string::npos)
                        svec.push_back(temp);
                    temp = "";
                }
            }
            if (temp != "" && temp.find('=') != string::npos)
                svec.push_back(temp);
            //解析空格 去除类型 eg int i=1;==> i=1
            for (auto it : svec)
            {
                if (it.find(' ') != string::npos)
                {
                    it = it.substr(it.find(' '), it.size() - 1);
                    buf << "\t\t" << it << "\n";
                }
                else
                    buf << "\t\t" << it << "\n";
            }
        }
    }
    buf << "\t}\n";
}
void X86CodeGeneration::CGactorsInit(stringstream &buf, Node *init)
{
    buf << "\tvoid init(){ \n";
    if (init != NULL)
        buf << init->toString();
    buf << "\t}\n";
}
void X86CodeGeneration::CGactorsWork(stringstream &buf, Node *work, FlatNode* actor)
{
    buf << "\tvoid work(){ \n";
    /* work中的还含有流形式参数需要被替换 等符号表实现*/
    if(actor == fileReaderActor)
    {
        buf << "\t\twhile(!ringBuffer.read((char*)&sourceBuff));\n";
        buf << "\t\tfor(int i=0;i<" << workLen << ";i++)\n";
		buf << "\t\t{\n";
		buf << "\t\t\tSource[i].x=sourceBuff.buffer[i];\n";//默认流中变量一定为int x
		buf << "\t\t}\n";
    }
    if (work != NULL)
        buf << work->toString();
    buf << "\n\t\tpushToken();\n";
    buf << "\t\tpopToken();\n";
    buf << "\t}\n";
}
/* 生成所有线程文件 */
void X86CodeGeneration::CGThreads()
{
    for (int i = 0; i < nCpucore_; ++i)
    {
        stringstream buf;
        buf << "/*该文件定义各thread的入口函数，在函数内部完成软件流水迭代*/\n";
        int MaxStageNum = psa_->MaxStageNum(); //最大阶段号
        buf << "#include \"Buffer.h\"\n";
        buf << "#include \"Producer.h\"\n";
        buf << "#include \"Consumer.h\"\n";
        buf << "#include \"Global.h\"\n";
        buf << "#include \"AllActorHeader.h\"\t//包含所有actor的头文件\n";
        buf << "#include \"lock_free_barrier.h\"\t//包含barrier函数\n";
        buf << "#include \"rdtsc.h\"\n";
        buf << "#include <fstream>\n";
        buf << "extern int MAX_ITER;\n"; //执行次数，用作迭代上限
        if (i)                           //除线程0外，其他线程调用workSync等待线程0，线程0进行累计其他线程是否结束，并设置标志位
        {
            buf << "void thread_" << i << "_fun()\n{\n";
            buf << "\tworkerSync(" << i << ");\n";
        }
        else
        {
            buf << "void thread_" << i << "_fun()\n{\n";
            buf << "\tmasterSync(" << nCpucore_ << ");\n";
        }
        vector<FlatNode *> actorSet = mp_->findNodeSetInPartition(i); //actorSet存放线程index上所有actor的集合
        //遍历actorSet所有actor，生成其类型实例
        for (auto iter = actorSet.begin(); iter != actorSet.end(); ++iter)
        {
            buf << "\t" << (*iter)->name << " " << (*iter)->name << "_obj("; //定义actor对象，actor->name_obj,调用构造函数，参数为输入输出边的全局变量
            //mapActor2InEdge和mapActor2OutEdge存放该actor所对应的输入输出边buffer的名称
            auto pos1 = mapActor2InEdge.equal_range(*iter);
            auto pos2 = mapActor2OutEdge.equal_range(*iter);
            while (pos2.first != pos2.second)
            {
                //buf << pos2.first->second << ",";
                buf<<bufferMatch[pos2.first->second].instance<<",";//使用实际共享的缓冲区作为实际输入
                ++pos2.first;
            }
            while (pos1.first != pos1.second)
            {
                //buf << pos1.first->second << ",";
                buf<<bufferMatch[pos1.first->second].instance<<",";//使用实际共享的缓冲区作为实际输入
                ++pos1.first;
            }
            //稳态iad
            buf.seekp((int)buf.tellp() - 1);
            buf << ");\n";
        }
        //stage表示阶段号数组，初始除0外都为0
        buf << "\tchar stage[" << MaxStageNum << "]={0};\n";
        buf << "\tstage[0]=1;\n";
        //遍历该线程上的所有的阶段号，在对应的阶段号内调用每个actor的initwork
        buf << "\tfor(int _stageNum=0;_stageNum<" << MaxStageNum << ";_stageNum++)\n";
        buf << "\t{\n";
        set<int> stageSet = mapNum2Stage.find(i)->second;      //查找该thread对应的阶段号集合
        for (int stage = MaxStageNum - 1; stage >= 0; stage--) //迭代stage Num
        {
            auto iter = stageSet.find(stage); //查找该线程对应在阶段i是否有actor
            if (iter != stageSet.end())
            { //该stage在该thread上
                buf << "\t\tif(" << stage << "==_stageNum)\n\t\t{\n";
                vector<FlatNode *> flatVec = psa_->FindActor(stage); //取得在该阶段的所有actor集合
                //cout<<flatVec.size()<<endl;
                for (auto fNode : flatVec) //遍历actor，调用初态initScheduleWork
                {
                    if (i == mp_->findPartitionNumForFlatNode(fNode))
                        buf << "\t\t\t" << fNode->name << "_obj.runInitScheduleWork();\n";
                }
                buf << "\t\t}\n";
            }
        }
        if (i)
            buf << "\t\n\t\tworkerSync(" << i << ");\n";
        else
            buf << "\t\n\t\tmasterSync(" << nCpucore_ << ");\n";
        //初态调度完成
        buf << "\t}\n";
        /* 稳态调度开始 */
        buf << "\tfor(int _stageNum=" << MaxStageNum << ";_stageNum<2*" << MaxStageNum << "+MAX_ITER-1;_stageNum++)\n\t{\n";
        for (int stage = MaxStageNum - 1; stage >= 0; stage--) //迭代stage
        {
            auto stageiter = stageSet.find(stage);
            if (stageiter != stageSet.end())
            { //该stage在该thread上
                buf << "\t\tif(stage[" << stage << "])\n\t\t{\n";
                vector<FlatNode *> flatVec = psa_->FindActor(stage);
                for (auto iter : flatVec)
                {
                    if (i == mp_->findPartitionNumForFlatNode(iter))
                        buf << "\t\t\t" << iter->name << "_obj.runSteadyScheduleWork();\n";
                }
                buf << "\t\t}\n";
            }
        }
        buf << "\t\tfor(int index=" << MaxStageNum - 1 << "; index>= 1; --index)\n";
        buf << "\t\t\tstage[index] = stage[index-1];\n\t\tif(_stageNum == (MAX_ITER - 1 + " << MaxStageNum << "))\n\t\t{\n\t\t\tstage[0]=0;\n\t\t}\n";
        if (i)
            buf << "\t\n\t\tworkerSync(" << i << ");\n";
        else
            buf << "\t\n\t\tmasterSync(" << nCpucore_ << ");\n";
        buf << "\t}\n";
        /*稳态调度截止*/
        buf << "}\n";
        string filename = "thread_" + to_string(i) + ".cpp";
        ofstream out(filename);
        out << buf.str();
    }
}
/*所有actor节点头文件*/
void X86CodeGeneration::CGAllActorHeader()
{
    stringstream buf;
    for (auto it : flatNodes_)
        buf << "#include \"" << it->name << ".h\"\n";
    ofstream out("AllActorHeader.h");
    out << buf.str();
}
/****************生成main文件*************************/
void X86CodeGeneration::CGMain()
{
    stringstream buf;
    buf << "#include <iostream>\n";
    buf << "#include <fstream>\n";
    buf << "#include <stdlib.h>\n";
    buf << "#include <pthread.h>\n";
    buf << "#include \"setCpu.h\"\n";
    buf << "#include \"lock_free_barrier.h\"	//包含barrier函数\n";
    buf << "#include \"Global.h\"\n";
    buf << "#include \"RingBuffer.h\"\n";
    buf << "using namespace std;\n";
    buf << "int MAX_ITER=1;//默认的执行次数是1\n";
    if(IOHandler)
    {
        //定义环形缓冲区和数据批次数
        buf << "struct source{\n";
		buf << "\tint " << "buffer[" << workLen << "];\n";
		buf << "};\n";
		buf << "RingBuffer<source> ringBuffer(1024,sizeof(source));\n";
        buf << "int workExecuteTimes=0;\n";
        buf <<  "\n";
        //生成文件读取线程
        buf << "void* thread_io_fun_start(void *arg)\n";
		buf << "{\n";
		buf << "\tsource iobuff;\n";
        buf << "\tifstream inSource(\"input.bin\",ios::binary);\n";
		buf << "\tfor(int i=0;i<workExecuteTimes;i++)\n";
		buf << "\t{\n";
		buf << "\t\tinSource.read((char*)iobuff.buffer,sizeof(int)*"<<workLen << ");\n";
		buf << "\t\twhile(!ringBuffer.write((char*)&iobuff));\n";
		buf << "\t}\n";
        buf << "\treturn 0;\n";
		buf << "}\n";
    }

    for (int i = 0; i < nCpucore_; i++)
    {
        buf << "extern void thread_" << i << "_fun();\n";
    }

    for (int i = 1; i < nCpucore_; i++)
    {
        buf << "void* thread_" << i << "_fun_start(void *)\n"
            << "{\n\t"
            << "set_cpu(" << i << ");\n\t"
            << "thread_" << i << "_fun();\n\t"
            << "return 0;\n"
            << "}\n";
    }
    buf << "int main(int argc,char **argv)\n{\n";
    buf << "\tvoid setRunIterCount(int,char**);\n";
    buf << "\tsetRunIterCount(argc,argv);\n";
    //主线程计算输入文件的数据批次，默认输入源为当前目录下的input.bin
    if(IOHandler)
    {
        buf << "\tifstream in(\"input.bin\",ios::binary);\n";
		buf << "\tin.seekg(0,ios::end);\n";
		buf << "\tint length = in.tellg();\n";
		buf << "\tin.seekg(0,ios::beg);\n";
		buf << "\tworkExecuteTimes=length/(sizeof(int)*"<<workLen << ");\n";
        //workExecuteTimes代表文件数据量能够支持work函数执行的次数
        //由于程序执行分为初态调度和稳态调度，所以要对MAX_ITER进行计算
        //初态调度消耗的数据量为 初态调度次数*一次work函数消耗的数据量(初态调度在一次程序执行时只会执行一次)
        //稳态调度消耗的数据量为 稳态调度次数*一次work函数消耗的数据量*MAX_ITER
        //如果.cos文件中不存在peek!=pop的节点的话，那么初态调度次数一定为0
        //初态调度似乎是为了流媒体处理中填充边界数据加入的特性，因为peek!=pop节点的引入会导致后续处理变的复杂
        //MAX_ITER的计算方式这里还是采取肖硕学长的计算方式MAX_ITER=(总数据量-初态调度次数*一次work函数消耗的数据量)/(稳态调度次数*一次work函数消耗的数据量)
        //由于可以获取到头节点work函数执行的数据量，因此MAX_ITER=(workExecuteTimes-头节点初态调度次数)/头节点稳态调度次数
        int initworkcount = sssg_->GetInitCount(flatNodes_[0]);
        int steadyworkcount = sssg_->GetSteadyCount(flatNodes_[0]);
        buf << "\tMAX_ITER = (workExecuteTimes-"<<initworkcount<<")/"<<steadyworkcount<<";\n";
    }
    buf << "\tset_cpu(0);\n";
    buf << "\tallocBarrier(" << nCpucore_ << ");\n";
    buf << "\tpthread_t tid[" << nCpucore_ - 1 << "];\n";
    for (int i = 1; i < nCpucore_; i++)
    {
        buf << "\tpthread_create (&tid[" << i - 1 << "], NULL, thread_" << i << "_fun_start, (void*)NULL);\n";
    }
    if(IOHandler)
    {
		buf << "\tif(!ringBuffer.Initialize())\n";
		buf << "\t{\n";
		buf << "\t\treturn -1;\n";
		buf << "\t}\n";
		buf << "\tpthread_t th;\n";
		buf << "\tint ret = pthread_create (&th, NULL, thread_io_fun_start, (void*)NULL);\n";
    }
    buf << "\tthread_0_fun();\n";
    if(IOHandler)
    {
		buf << "\tret = pthread_join(th,NULL);\n";
		buf << "\tif(ret!=0)\n";
		buf << "\t{\n";
		buf << "\t\tcout<<\"join error\"<<endl;\n";
		buf << "\t\treturn -1;\n";
		buf << "\t}\n";
    }
    buf << "\treturn 0;\n";
    buf << "}\n";
    buf << "\/\/设置运行次数\n";
    buf << "void setRunIterCount(int argc,char **argv)\n{\n";
    buf << "\tint oc;\n";
    buf << "\tchar *b_opt_arg;\n";
    buf << "\twhile((oc=getopt(argc,argv,\"i:\"))!=-1)\n";
    buf << "\t{\n";
    buf << "\t\tswitch(oc)\n";
    buf << "\t\t{\n";
    buf << "\t\t\tcase 'i':\n";
    buf << "\t\t\t\tMAX_ITER=atoi(optarg);\n";
    buf << "\t\t\t\tbreak;\n";
    buf << "\t\t}\n";
    buf << "\t}\n";
    buf << "}\n";
    ofstream out("main.cpp");
    out << buf.str();
}

/****************生成函数文件*************************/
void X86CodeGeneration::CGFunctionHeader()
{
    stringstream buf;
    buf << "#ifndef _FUNCTION_H_\n";
    buf << "#define _FUNCTION_H_\n";
    if (Program != NULL)
    {
        for (auto it : *Program)
        {
            if (it->type == FuncDcl)
            {
                buf << (((funcDclNode *)it)->toString()) + ";";
            }
        }
    }
    buf << "\n";
    buf << "#endif\n";
    ofstream out("function_.h");
    out << buf.str();
}

void X86CodeGeneration::CGFunction()
{
    stringstream buf;
    buf << "#include \"function_.h\"\n";
    if (Program != NULL)
    {
        for (auto it : *Program)
        {
            if (it->type == FuncDcl)
            {
                buf << (((funcDclNode *)it)->toString());
                if (((funcDclNode *)it)->funcBody != NULL)
                {
                    buf << "{\n";
                    buf << "\t" + ((funcDclNode *)it)->funcBody->toString();
                    buf << "\n}\n";
                }
                else
                {
                    buf << "{\n}";
                }
            }
        }
    }
    ofstream out("function_.cpp");
    out << buf.str();
}