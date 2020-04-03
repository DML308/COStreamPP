#include "x86CodeGenaration.h"
SymbolTable *running_top;
extern SymbolTable* top;
extern SymbolTable S;
extern sequentialNode* globalSequential;
extern bool ifConstantFlow;

list<Node *> stateArrayVariable;
extern list<Variable *> paramArrayVariable;
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
    cout << "Program  size=" << Program->size() << endl;
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
            buf << "extern " + iter->toString() + ";" << "\n";
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
    if(globalSequential){
        if(globalSequential->ifNeedMathExtension) {
            buf<<"#include \"MathExtension.h\"\n";
        }
    }
    
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
        }
    }
    // 为sequential添加数据流数据类型
    if (globalSequential != NULL)  {
        typeSet.insert(make_pair("double", "x"));
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
                    buf << "Buffer<streamData> " << edgename << "(" << size << "," << copySize << "," << copyStartPos << ");\n";
                    break;
                }
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
        paramArrayVariable.clear();
        stateArrayVariable.clear();
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
            for (auto it : *inputs)
                inEdgeName.push_back(((idNode *)it)->name);
        }
        if (outputs != NULL)
        {
            for (auto it : *outputs)
                outEdgeName.push_back(((idNode *)it)->name);
        }
        buf << "public:\n";
        /*写入类成员函数*/
        CGactorsConstructor(flatNodes_[i], buf, className, inEdgeName, outEdgeName);
        CGactorsRunInitScheduleWork(buf, inEdgeName, outEdgeName);
        CGactorsRunSteadyScheduleWork(buf, inEdgeName, outEdgeName);
        

        /*写入类成员变量*/
        buf << "private:\n";
        for (auto out : outEdgeName)
            buf << "\tProducer<streamData> " << out << ";\n";
        for (auto in : inEdgeName)
            buf << "\tConsumer<streamData> " << in << ";\n";
        buf << "\tint steadyScheduleCount;\t//稳态时一次迭代的执行次数\n";
        buf << "\tint initScheduleCount;\n";

        //写入composite传入的参数 此处为声明，在init中赋值
        running_top = flatNodes_[i]->compositecall_runnningtop;
        SymbolTable *opt_top = new SymbolTable(NULL,NULL);// FindRightSymbolTable(flatNodes_[i]->contents->loc->first_line);
        //top = opt_top;
        //ifCodeGeneration = false;
        //generatorOperatorNode(flatNodes_[i]->contents);
        opt_top = FindRightSymbolTable(flatNodes_[i]->contents->loc->first_line);
        //for(auto it : running_top->)
        string param = running_top->toParamString(opt_top);
        buf << param;

        //写入init部分前的statement定义，调用tostring()函数，解析成规范的类变量定义格式
        CGactorsStmts(buf, &stmts);
        CGactorsPopToken(buf, flatNodes_[i], inEdgeName);
        CGactorsPushToken(buf, flatNodes_[i], outEdgeName);
        //init部分前的statement赋值
        CGactorsinitVarAndState(buf, &stmts);
        /* composite 中init函数 */
        CGactorsInit(buf, init);
        /* composite中work函数 */
        CGactorsWork(buf, work);
        /* 类体结束*/
        buf << "};\n";
        /*定义初始化operator中声明的数组*/
        for(auto node : stateArrayVariable){
            string str = node->toString();
            int index = 0;
            for (auto c : str)
            {
                if (c == ' ')
                    break;
                index++;
            }
            str = str.substr(0,index+1) + className + "::" + str.substr(index+1,str.size()) + "\n";
            buf<<str;
        }
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

void X86CodeGeneration::CGactorsRunInitScheduleWork(stringstream &buf, vector<string> inEdgeName, vector<string> outEdgeName)
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
void X86CodeGeneration::CGactorsRunSteadyScheduleWork(stringstream &buf, vector<string> inEdgeName, vector<string> outEdgeName)
{
    buf << "\tvoid runSteadyScheduleWork() {\n";
    buf << "\t\tfor(int i=0;i<steadyScheduleCount;i++)\n";
    buf << "\t\t\twork();\n";
    if (outEdgeName.size() != 0)
        for (auto out : outEdgeName)
            buf << "\t\t" << out << ".resetTail();\n";
    if (inEdgeName.size() != 0)
        for (auto in : inEdgeName)
            buf << "\t\t" << in << ".resetHead();\n";
    buf << "\t}\n";
}
void X86CodeGeneration::CGactorsStmts(stringstream &buf, list<Node *> *stmts)
{
    if (stmts != NULL)
    {
        for (auto it : *stmts)
        {
            string str = "";
            bool isArray = false;
            //确定数组各维大小 例:int a[size][size] => int a[8][8]
            if(it->type == Decl){  
                declareNode* decl_node = ((declareNode *)it);
                list<idNode *> id_list = ((declareNode *)it)->id_list;
                list<idNode *> copy_id_list;// = list<idNode *>();
                if(id_list.size()){
                    for(auto id_node :id_list){
                        if(id_node->isArray){
                            isArray = true;
                            list<Node *>copy_args = list<Node *>();
                            list<Node *>args = id_node->arg_list;
                            for(auto arg :args){
                                if(arg->type == Id){
                                    Variable* value = running_top->LookupIdentifySymbol(((idNode *)arg)->name);
                                    constantNode* constant_value = running_top->fromVariableToConstant(value);
                                    copy_args.push_back(constant_value);
                                }else{
                                    copy_args.push_back(arg);
                                }
                            }
                            idNode* array_id = new idNode(id_node->name);
                            array_id->arg_list = copy_args;
                            array_id->isArray = true;
                            array_id->init = NULL;
                            copy_id_list.push_back(array_id);
                       }else{
                           copy_id_list.push_back(id_node);
                       }

                   }
               }
               if(isArray){
                    for(auto it :copy_id_list){//如果是 int x[5][6],y[7][8] 进行拆分
                        list<idNode *>each_id_list;
                        each_id_list.push_back(it);
                        declareNode* each_decl_node = new declareNode(decl_node->prim,each_id_list);
                        if(it->init){
                            stateArrayVariable.push_back(each_decl_node);
                        }else{
                            str += each_decl_node->toString();
                        }
                        
                    }
                    for(auto id : stateArrayVariable){
                        //declareNode* copy_decl_node = new declareNode(decl_node->prim,copy_id_list);
                        str += "static " + id->toString();
                    }
                    
               } 
            }
            
            if(!isArray){
                str += it->toString();
            }
            
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
    //进行param的初始化
          
    SymbolTable *opt_top = top;
    string param = running_top->toParamValueString(opt_top);
    buf<<param;
    if (stmts != NULL)
    {
        for (auto it : *stmts)
        {
            bool isArray = false;
            if(it->type == Decl){
                declareNode* decl_node = ((declareNode *)it);
                list<idNode *> id_list = ((declareNode *)it)->id_list;
                list<idNode *> copy_id_list;// = list<idNode *>();
                if(id_list.size()){
                   for(auto id_node :id_list){
                       if(id_node->isArray){ //如果是 int x[5][6],y[7][8];如何处理
                            isArray = true;
                            break;
                        }
                     }
                }
            }
            if(isArray) continue;
            string str = it->toString()+";";
            vector<string> svec;
            vector<int> array;
            /*解析逗号和等号,类似int i=0,j;形式变成i=0;的形式,初始化stmts*/
            string temp = "";
            for (auto c : str)
            {
                if(c == '{'){
                    array.push_back(1);
                }
                if(c == '}'){
                    array.pop_back();
                }
                if (c != ',' && c != ';')
                    temp += c;
                else 
                {
                    if(c == ',' && array.size()){
                        temp+=c;
                        continue;
                    }
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
void X86CodeGeneration::CGactorsWork(stringstream &buf, Node *work)
{
    buf << "\tvoid work(){ \n";
    /* work中的还含有流形式参数需要被替换 等符号表实现*/
    string param_str = "";
    for(auto variable : paramArrayVariable){
        param_str += variable->type + ' ' + variable->name;
        for(auto size : ((ArrayConstant *)(variable->value))->arg_size){
            string size_str;
            size_str = '['+ to_string(size) + ']';
            param_str += size_str;
        }
        string value_str ;
        value_str = " =" + ((ArrayConstant *)(variable->value))->printStr() + ";" +"\n";
        param_str += value_str;
    }
    buf << param_str;
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
                buf << pos2.first->second << ",";
                ++pos2.first;
            }
            while (pos1.first != pos1.second)
            {
                buf << pos1.first->second << ",";
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
    buf << "#include <stdlib.h>\n";
    buf << "#include <pthread.h>\n";
    buf << "#include \"setCpu.h\"\n";
    buf << "#include \"lock_free_barrier.h\"	//包含barrier函数\n";
    buf << "#include \"Global.h\"\n";
    if(globalSequential){
        if (globalSequential->ifNeedMathExtension) {
            buf << "#include <time.h>\n";
        }
    }
    
    buf << "using namespace std;\n";
    buf << "int MAX_ITER=1;//默认的执行次数是1\n";
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
    if(globalSequential){
        if (globalSequential->ifNeedMathExtension) {
            buf << "srand(time(NULL));\n";
        }
    }
    
    buf << "\tvoid setRunIterCount(int,char**);\n";
    buf << "\tsetRunIterCount(argc,argv);\n";
    buf << "\tset_cpu(0);\n";
    buf << "\tallocBarrier(" << nCpucore_ << ");\n";
    buf << "\tpthread_t tid[" << nCpucore_ - 1 << "];\n";
    for (int i = 1; i < nCpucore_; i++)
    {
        buf << "\tpthread_create (&tid[" << i - 1 << "], NULL, thread_" << i << "_fun_start, (void*)NULL);\n";
    }
    buf << "\tthread_0_fun();\n";
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