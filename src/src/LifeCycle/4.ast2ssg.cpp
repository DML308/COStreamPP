//#define DEBUG
#include "staticStreamGragh.h"
#include "unfoldComposite.h"
#include "compositeFlow.h"
#include "symboltableGenerate.h"

static StaticStreamGraph *ssg = NULL;
extern UnfoldComposite *unfold;
extern list<SymbolTable *> runningStack;
extern SymbolTable *runningTop;
extern SymbolTable S;

extern StaticStreamGraph *ssgs[MAX_SCOPE_DEPTH][MAX_SCOPE_DEPTH][MAX_SCOPE_DEPTH];


int count = 0;
int a_level = -1;
int a_version[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

void resizeSplitjoinWindow(compositeNode *splitjoinComposite){
    bool isResize = false;
    list<Node *> *stmts = splitjoinComposite->body->stmt_List;
    compositeCallNode *father_split_node = (compositeCallNode *)stmts->front();
    compositeCallNode *father_join_node = (compositeCallNode *)stmts->back();

    compositeNode *father_split_composite = father_split_node->actual_composite;
    compositeNode *father_join_composite = father_join_node->actual_composite;

    operatorNode *father_split_operator = (operatorNode *)father_split_composite->body->stmt_List->front();
    operatorNode *father_join_operator = (operatorNode *)father_join_composite->body->stmt_List->front();

    list<Node *> *father_split_window = father_split_operator->operBody->win->win_list;
    list<Node *> *father_join_window = father_join_operator->operBody->win->win_list;

    list<constantNode *> *split_arguments = new list<constantNode *>();
    list<constantNode *> *join_arguments = new list<constantNode *>();

    auto split_p = father_split_window->begin();//标记是splitjoin下的第几个节点
    auto join_p = father_join_window->begin();
    int count = 0;

    for(auto it : *stmts){
        if(it == stmts->back()){
            continue;
        }
        if(it == stmts->front()){
            continue;
        }
        if(it->type == CompositeCall){
            compositeCallNode *comp_call_node = (compositeCallNode *)it;
            compositeNode *actual_composite = comp_call_node->actual_composite;
            list<Node *> *actual_stmts = actual_composite->body->stmt_List;
            for(auto actual_it : *actual_stmts){
                if(actual_it->type == SplitJoin){
                    isResize = true;
                    compositeNode *sp_composite_node = ((splitjoinNode *)actual_it)->replace_composite;
                    list<Node *> *sp_stmts = sp_composite_node->body->stmt_List;
                    compositeCallNode *split_node = (compositeCallNode *)sp_stmts->front();
                    compositeCallNode *join_node = (compositeCallNode *)sp_stmts->back();
                   
                    operatorNode *split_operator = (operatorNode *)split_node->actual_composite->body->stmt_List->front();
                    operatorNode *join_operator = (operatorNode *)join_node->actual_composite->body->stmt_List->front();

                    list<Node *> *split_window = split_operator->operBody->win->win_list;
                    list<Node *> *join_window = join_operator->operBody->win->win_list;

                    // 获取内部 split join 节点的窗口大小,只考虑单输入输出
                    winStmtNode *split_input = (winStmtNode *)split_window->back(); //在生成split节点时,输入窗口最后生成
                    winStmtNode *join_output = (winStmtNode *)join_window->back();  //在生成join节点时,输出窗口最后生成

                    constantNode *split_window_size,*join_window_size;
                    if((*split_p)->type == WindowStmt){

                        Node *win_type = split_input->winType;
                        if(win_type->type == Tumbling){
                            tumblingNode *tumbling_window = (tumblingNode *)win_type;
                            split_window_size = (constantNode *)tumbling_window->arg_list->front();
                        }
                        if(win_type->type == Sliding){
                            slidingNode *sliding_window = (slidingNode *)win_type;
                            split_window_size = (constantNode *)sliding_window->arg_list->front();
                        }
                        //修改上一层splitjoin的 split,join 节点窗口大小
                        Node *father_type = ((winStmtNode *)(*split_p))->winType;
                        if(father_type->type == Tumbling){
                            tumblingNode *tumbling_window = (tumblingNode *)father_type;
                            tumbling_window->arg_list->clear();
                            //todo 是否需要复制consantNode
                            tumbling_window->arg_list->push_back(split_window_size);
                        }
                        if(father_type->type == Sliding){
                            slidingNode *sliding_window = (slidingNode *)father_type;
                            sliding_window->arg_list->clear();
                            sliding_window->arg_list->push_back(split_window_size);
                            sliding_window->arg_list->push_back(split_window_size);
                        }
                    }

                    if((*join_p)->type == WindowStmt){
                        Node *win_type = join_output->winType;
                        if(win_type->type == Tumbling){
                            tumblingNode *tumbling_window = (tumblingNode *)win_type;
                            join_window_size = (constantNode *)tumbling_window->arg_list->front();
                        }
                        if(win_type->type == Sliding){
                            slidingNode *sliding_window = (slidingNode *)win_type;
                            join_window_size = (constantNode *)sliding_window->arg_list->front();
                        }
                        //修改上一层splitjoin的 split,join 节点窗口大小
                        Node *father_type = ((winStmtNode *)(*join_p))->winType;
                        if(father_type->type == Tumbling){
                            tumblingNode *tumbling_window = (tumblingNode *)father_type;
                            tumbling_window->arg_list->clear();
                            //todo 是否需要复制
                            tumbling_window->arg_list->push_back(join_window_size);
                        }
                        if(father_type->type == Sliding){
                            slidingNode *sliding_window = (slidingNode *)father_type;
                            sliding_window->arg_list->clear();
                            sliding_window->arg_list->push_back(join_window_size);
                            sliding_window->arg_list->push_back(join_window_size);
                        }
                    }
        
                    split_arguments->push_back(split_window_size);

        
                    join_arguments->push_back(join_window_size);    
                }
            }
        }
        
        split_p++;
        join_p++;

       
    }
    //由于窗口大小的改变 需要重写split join 节点的 work
    if(isResize){
        father_split_operator->operBody->work = unfold->MakeRoundrobinWork(father_split_operator->inputs,split_arguments,father_split_operator->outputs,father_split_composite->splitType);
        father_join_operator->operBody->work = unfold->MakeJoinWork(father_join_operator->inputs,join_arguments,father_join_operator->outputs);
    }
}

//求a,b的最大公约数
int gcd(int a, int b)
{
    int r = 0;
    if (a < b)
    {
        r = a;
        a = b;
        b = r;
    }
    assert(b);
    while (a % b)
    {
        assert(b);
        r = a % b;
        a = b;
        b = r;
    }

    return b;
}

//求a,b的最小公倍数
int lcm(int a, int b)
{
    int product = a * b;

    return product / gcd(a, b);
}

bool SteadyScheduling(StaticStreamGraph *ssg)
{
    list<FlatNode *> flatNodeList;
    map<FlatNode *, int>::iterator pos;
    // 默认第一个节点是源，也就是说peek和pop均为0,在图的表示上暂不允许有多个源，但可以有多个peek = pop = 0节点
    FlatNode *up = ssg->topNode, *down = NULL;
    int nPush = 0, nPop = 0, nLcm = 0;
    int x, y, i, j;

    map<FlatNode *, int> mapFlatNode2SteadyCount; // SDF图所有节点稳定状态调度序列<节点，执行次数>

    // 现在考虑的是只有一个输入口的情况
    while (1)
    {
        // 稳态调度系列初始系数为1
        mapFlatNode2SteadyCount.insert(make_pair(up, 1));
        // 遍历该节点的所有输出节点（广度优先遍历）
        for (i = 0; i < up->nOut; ++i)
        {
            nPush = up->outPushWeights[i]; // 上端节点的push值
            down = up->outFlatNodes[i];    // 找到下端节点
            for (j = 0; down->inFlatNodes[j] != up; j++)
                ;                         // 下端节点找到与上端节点对应的标号
            nPop = down->inPopWeights[j]; // 下端节点取出对应的pop值
            // 检查该节点是否已进行稳态调度，每条只进行一次稳态调度
            //Map的find函数寻找元素elem出现的位置，返回对应位置的迭代器，若没有出现则返回尾元素的下一个位置
            pos = mapFlatNode2SteadyCount.find(down);
            // down 节点未进行稳态调度
            if (pos == mapFlatNode2SteadyCount.end())
            {
                debug("对 down 节点 %s 进行稳态调度\n",down->name.c_str());
                // 得到上端节点的稳态调度系数（这个稳态调度系数就是一个迭代器，它指向Map中的指定节点《first，second》 《结点，执行次数》）
                pos = mapFlatNode2SteadyCount.find(up);
                x = pos->second;
                nPush *= x; // 为什么是x*nPush呢？理解稳态调度的概念--节点在流水线稳定运行中执行的最少次数 = 每次push * 稳态执行次数
                line("nPush %d nPop %d\n",nPush, nPop);
                if (nPush != 0)
                {
                    // nPush, nPop的最小公倍数;
                    nLcm = lcm(nPush, nPop);
                    int temp = nLcm / nPush; //上下节点达到平衡时，需要的执行次数
                    if (temp != 1)           // 加一个判断，提高效率，乘1是不必要的
                    {
                        // 根据计算规则得来的
                        for (pos = mapFlatNode2SteadyCount.begin(); pos != mapFlatNode2SteadyCount.end(); ++pos)
                            pos->second *= temp;
                    }
                    mapFlatNode2SteadyCount.insert(make_pair(down, nLcm / nPop));
                }
                else // 对push(0)作处理
                {
                    // 取 1 值
                    mapFlatNode2SteadyCount.insert(make_pair(down, 1));
                }
                // 将down加入listNode是为了对down的输出节点进行调度,相当于遍历
                flatNodeList.push_back(down);
            }
            else //该节点已进行稳态调度，检查SDF图是否存在稳态调度系列，一般不存在的话表明程序有误
            {
                y = pos->second; //下方节点的执行次数
                pos = mapFlatNode2SteadyCount.find(up);
                x = pos->second; //上端节点的执行次数
                //nPop == 0 说明在进行join 0 操作
                if ((nPop != 0) && (nPush * x) != (nPop * y))
                {
                    cout << "不存在稳态调度..." << endl;
                    system("pause");
                    exit(1); // 表示不存在稳态调度
                }            //if
            }                //else
        }                    //for
        if (flatNodeList.size() == 0)
            break; // 链表为空，说明所有节点已调度完毕
        up = flatNodeList.front();
        flatNodeList.pop_front(); //对已经调度完的节点移除list
    }                             //while
    return true;
}


/*
* 功能：递归的调用，完成splitjoin和pipeline节点的展开，以及完成opearatorNode到flatnode节点的映射
* 输入参数：composite
* 输出：设置静态数据流图的对应flatNode节点，完成数据流边到flatNode节点的映射
*/
void GraphToOperators(compositeNode *composite, Node *oldComposite)
{
    int version = 0;
    /* 获取compositebody内的statementNode */
    assert(composite != NULL && oldComposite != NULL);
    assert(composite->body->stmt_List != NULL);
    list<Node *> body_stmt = *(composite->body->stmt_List);
    for (auto it : body_stmt)
    {
        // 当type为binop时候检查binop的右子节点是否为以下节点类型
        switch (it->type)
        {
        case Binop:
        {
            expNode *exp = static_cast<binopNode *>(it)->right;
            if (exp->type == Operator_)
            {
                debug("operator_ %s\n",exp->toString().c_str());
                ssg->GenerateFlatNodes((operatorNode *)exp, oldComposite, composite);
                if(ssgs[count][a_level][a_version[a_level]]){
                    FlatNode *right_flatnode = new FlatNode((operatorNode *)exp, oldComposite, composite);
                    ssgs[count][a_level][a_version[a_level]]->flatNodes.push_back(right_flatnode);
                    ssgs[count][a_level][a_version[a_level]]->partFlatNodes.insert(make_pair(ssg->flatNodes.back(),1));
                    ssgs[count][a_level][a_version[a_level]]->flatNodes.back()->real_faltnode = ssg->flatNodes.back();
                    (ssg->flatNodes.back())->copy_flatnode = right_flatnode;
                }
            }
            else if (exp->type == CompositeCall)
            {
                debug("compositeCall %s\n",exp->toString().c_str());
                //runningTop = new SymbolTable(runningTop,exp->loc);
                
                list<Node *> *stream_List = ((compositeCallNode *)(exp))->stream_List;
                list<Node *> *inputs = ((compositeCallNode *)(exp))->inputs;
                list<Node *> *outputs = ((compositeCallNode *)(exp))->outputs;
                list<Constant*> paramList;

                if(stream_List){
                    if(runningTop){
                        paramList = generateStreamList(stream_List,runningTop); //获得参数的值
                    }else{
                        paramList = generateStreamList(stream_List,exp->loc->first_line-1); //获得参数的值
                    }
                }

                SymbolTable *scope;
                compositeCallNode *call = ((compositeCallNode *)(exp));
                if(call->scope){
                    scope = call->scope;
                }else{
                    scope = runningTop;
                }
                
                //传入参数,并生成 composite 调用的执行上下文环境
                runningTop = generateCompositeRunningContext(call,call->actual_composite,paramList,inputs,outputs);
                runningStack.push_back(runningTop); // 调用栈

                GraphToOperators(call->actual_composite, exp);

                //出栈
                runningStack.pop_back();
                runningTop = runningStack.back();
                
            }
            else if (exp->type == SplitJoin)
            {
                debug("splitjoin %s\n",exp->toString().c_str());

                StaticStreamGraph *local_ssg = new StaticStreamGraph();
                a_level++;
                a_version[a_level]++;
                ssgs[count][a_level][a_version[a_level]] = local_ssg;

                ((splitjoinNode *)exp)->replace_composite = unfold->UnfoldSplitJoin(((splitjoinNode *)exp));
                GraphToOperators(((splitjoinNode *)(exp))->replace_composite, ((splitjoinNode *)(exp))->replace_composite);
                

                a_level--;
                
                local_ssg->SetTopNode();
                local_ssg->setEndNode();
               
                if(a_level == -1){
                    count ++;
                    a_level = -1;
                    for(int i=0;i<10;i++){
                        a_version[i] = -1;
                    }
                    
                }

                //resizeSplitjoinWindow(((splitjoinNode *)(exp))->replace_composite);
            }
            else if (exp->type == Pipeline)
            {
                debug("pipeline %s\n",exp->toString().c_str());
                
                StaticStreamGraph *local_ssg = new StaticStreamGraph();
                a_level++;
                a_version[a_level]++;
                ssgs[count][a_level][a_version[a_level]] = local_ssg;

                ((pipelineNode *)exp)->replace_composite = unfold->UnfoldPipeline(((pipelineNode *)exp));
                GraphToOperators(((pipelineNode *)(exp))->replace_composite, ((pipelineNode *)(exp))->replace_composite);
                

                a_level--;
                
                local_ssg->SetTopNode();
                local_ssg->setEndNode();

                if(a_level == -1){
                    count ++;
                    a_level = -1;
                    for(int i=0;i<10;i++){
                        a_version[i] = -1;
                    }
                    
                }
                //todo 是否需要反向传递窗口大小
            }
            else if (exp->type == Sequential)
            {   
                ((sequentialNode *)exp)->replace_composite = unfold->UnfoldSequential(((sequentialNode *)exp));
                GraphToOperators(((sequentialNode *)(exp))->replace_composite, ((sequentialNode *)(exp))->replace_composite);
            }
            break;
        }
        case Operator_:
        {
            debug("operator_ %s\n",it->toString().c_str());
            ssg->GenerateFlatNodes((operatorNode *)it, oldComposite, composite);
            if(ssgs[count][a_level][a_version[a_level]]){
                    FlatNode *right_flatnode = new FlatNode((operatorNode *)it, oldComposite, composite);
                    ssgs[count][a_level][a_version[a_level]]->flatNodes.push_back(right_flatnode);
                    ssgs[count][a_level][a_version[a_level]]->partFlatNodes.insert(make_pair(ssg->flatNodes.back(),1));
                    ssgs[count][a_level][a_version[a_level]]->flatNodes.back()->real_faltnode = ssg->flatNodes.back();
                    (ssg->flatNodes.back())->copy_flatnode = right_flatnode;
                }
            break;
        }
        case CompositeCall:
        {
            debug("compositeCall %s\n", it->toString().c_str());

            list<Node *> *stream_List = ((compositeCallNode *)(it))->stream_List;
            list<Constant*> paramList;
            list<Node *> *inputs = ((compositeCallNode *)(it))->inputs;
            list<Node *> *outputs = ((compositeCallNode *)(it))->outputs;

            if(stream_List){
                if(runningTop){
                    paramList = generateStreamList(stream_List,runningTop); //获得参数的值
                }else{
                    paramList = generateStreamList(stream_List,(*it).loc->first_line-1); //嵌套调用 composite 获得参数的值
                }
            }
            SymbolTable *scope = NULL;
            compositeCallNode *call = ((compositeCallNode *)(it));
            if(call->scope){
                scope = call->scope;
            }
            runningTop = generateCompositeRunningContext(call,call->actual_composite,paramList,inputs,outputs); //传入参数,并生成 composite调用的执行上下文环境
            // 确定window大小
            
            runningStack.push_back(runningTop); // 调用栈

            GraphToOperators(call->actual_composite, it);

            //出栈
            runningStack.pop_back();
            runningTop = runningStack.back();

            break;
        }
        case SplitJoin:
        {
            debug("splitjoin %s\n", it->toString().c_str());
            
            
            StaticStreamGraph *local_ssg = new StaticStreamGraph();
            a_level++;
            a_version[a_level]++;
            ssgs[count][a_level][a_version[a_level]] = local_ssg;

           ((splitjoinNode *)it)->replace_composite = unfold->UnfoldSplitJoin(((splitjoinNode *)it));
            GraphToOperators(((splitjoinNode *)(it))->replace_composite, ((splitjoinNode *)(it))->replace_composite);
                

            a_level--;
            
            if(a_level == -1){
                count ++;
                a_level = -1;
                for(int i=0;i<10;i++){
                    a_version[i] = -1;
                }
                
            }
            local_ssg->SetTopNode();
            local_ssg->setEndNode();
                
            //local_ssg->SetFlatNodesWeights();
            //SteadyScheduling(local_ssg);
            //resizeSplitjoinWindow(((splitjoinNode *)(it))->replace_composite);
            break;
        }
        case Pipeline:
        {
            debug("pipeline %s\n", it->toString().c_str());
            
            
            StaticStreamGraph *local_ssg = new StaticStreamGraph();
            a_level++;
            a_version[a_level]++;
            ssgs[count][a_level][a_version[a_level]] = local_ssg;

           ((pipelineNode *)it)->replace_composite = unfold->UnfoldPipeline(((pipelineNode *)it));
            GraphToOperators(((pipelineNode *)(it))->replace_composite, ((pipelineNode *)(it))->replace_composite);
                

            a_level--;
            
            if(a_level == -1){
                count ++;
                a_level = -1;
                for(int i=0;i<10;i++){
                    a_version[i] = -1;
                }
                
            }
            local_ssg->SetTopNode();
            local_ssg->setEndNode();
            //todo 是否需要反向传递窗口大小
            break;
        }
        default:
            break;
        }
    }
    return;
}

/*
 *  功能：将抽象语法树转为平面图 
 *  输入参数：gMaincomposite
 *  streamFlow：对所有Main composite的composite调用进行实际流边量名的替换
 *  GraphToOperators：递归的调用，完成splitjoin和pipeline节点的展开，以及完成opearatorNode到flatnode节点的映射
 *  SetTopNode：设置顶层节点
 *  ResetFlatNodeNames：给所有的图节点重命名
 *  SetFlatNodesWeights：设置静态数据流图的peek，pop，push值
 *  输出：静态数据流图ssg
 */
StaticStreamGraph *AST2FlatStaticStreamGraph(compositeNode *mainComposite)
{
    ssg = new StaticStreamGraph();
    //streamFlow(mainComposite);
    debug("--------- 执行GraphToOperators, 逐步构建FlatNode ---------------\n");

    //list<Constant*> paramList;
    runningTop = &S; //传入参数,并生成 composite 调用的执行上下文环境
    runningStack.push_back(runningTop); // 调用栈

    //compositeCallNode *mainCompositeCall = new compositeCallNode(NULL,"main",NULL,NULL,mainComposite);
    list<Constant*> paramList = list<Constant*>();
    list<Node *> *inputs =new list<Node*>();
    list<Node *> *outputs =new list<Node*>();

    runningTop = generateCompositeRunningContext(NULL,mainComposite,paramList,inputs,outputs); //传入参数,并生成 composite 调用的执行上下文环境
    runningStack.push_back(runningTop); // 调用栈


    GraphToOperators(mainComposite, mainComposite);

    runningStack.pop_back();
    runningTop = runningStack.back();
    //出栈
    runningStack.pop_back();
    runningTop = runningStack.back();

    ssg->SetTopNode();
    /* 将每个composite重命名 */
    ssg->ResetFlatNodeNames();
    ssg->SetFlatNodesWeights();
    /* 测试peek，pop，push值 */
    cout << "--------- 执行AST2FlatStaticStreamGraph后, 查看静态数据流图中的全部 FlatNode ---------------\n";
#if 0
    for (auto it : ssg->flatNodes)
    {
        cout << it->name << ":\t" << it->toString() << endl;
        if (it != ssg->flatNodes.back())
            cout << "    ↓" << endl;
    }
#endif
    return ssg;
}
