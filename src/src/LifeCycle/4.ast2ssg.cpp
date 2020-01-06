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

void resizeSplitjoinWindow(compositeNode *splitjoinComposite){
    list<Node *> *stmts = splitjoinComposite->body->stmt_List;
    compositeCallNode *father_split_node = (compositeCallNode *)stmts->front();
    compositeCallNode *father_join_node = (compositeCallNode *)stmts->back();

    operatorNode *father_split_operator = (operatorNode *)father_split_node->actual_composite->body->stmt_List->front();
    operatorNode *father_join_operator = (operatorNode *)father_join_node->actual_composite->body->stmt_List->front();

    list<Node *> *father_split_window = father_split_operator->operBody->win->win_list;
    list<Node *> *father_join_window = father_join_operator->operBody->win->win_list;

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
            compositeNode *actuaal_composite = comp_call_node->actual_composite;
            list<Node *> *actual_stmts = actuaal_composite->body->stmt_List;
            for(auto actual_it : *actual_stmts){
                if(actual_it->type == SplitJoin){
                    compositeNode *sp_composite_node = ((splitjoinNode *)actual_it)->replace_composite;
                    list<Node *> *sp_stmts = sp_composite_node->body->stmt_List;
                    compositeCallNode *split_node = (compositeCallNode *)sp_stmts->front();
                    compositeCallNode *join_node = (compositeCallNode *)sp_stmts->back();

                    operatorNode *split_operator = (operatorNode *)split_node->actual_composite->body->stmt_List->front();
                    operatorNode *join_operator = (operatorNode *)join_node->actual_composite->body->stmt_List->front();

                    list<Node *> *split_window = split_operator->operBody->win->win_list;
                    list<Node *> *join_window = join_operator->operBody->win->win_list;

                    // 只考虑单输入输出
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
                }
            }
        }
        
        split_p++;
        join_p++;
        
        
    }
}
/*
* 功能：递归的调用，完成splitjoin和pipeline节点的展开，以及完成opearatorNode到flatnode节点的映射
* 输入参数：composite
* 输出：设置静态数据流图的对应flatNode节点，完成数据流边到flatNode节点的映射
*/
void GraphToOperators(compositeNode *composite, Node *oldComposite)
{
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
                ((splitjoinNode *)exp)->replace_composite = unfold->UnfoldSplitJoin(((splitjoinNode *)exp));
                GraphToOperators(((splitjoinNode *)(exp))->replace_composite, ((splitjoinNode *)(exp))->replace_composite);
                resizeSplitjoinWindow(((splitjoinNode *)(exp))->replace_composite);
            }
            else if (exp->type == Pipeline)
            {
                debug("pipeline %s\n",exp->toString().c_str());
                ((pipelineNode *)exp)->replace_composite = unfold->UnfoldPipeline(((pipelineNode *)exp));
                GraphToOperators(((pipelineNode *)(exp))->replace_composite, ((pipelineNode *)(exp))->replace_composite);
                //todo 是否需要反向传递窗口大小
            }
            break;
        }
        case Operator_:
        {
            debug("operator_ %s\n",it->toString().c_str());
            ssg->GenerateFlatNodes((operatorNode *)it, oldComposite, composite);
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
            ((splitjoinNode *)it)->replace_composite = unfold->UnfoldSplitJoin(((splitjoinNode *)it));
            GraphToOperators(((splitjoinNode *)(it))->replace_composite, ((splitjoinNode *)(it))->replace_composite);
            resizeSplitjoinWindow(((splitjoinNode *)(it))->replace_composite);
            break;
        }
        case Pipeline:
        {
            debug("pipeline %s\n", it->toString().c_str());
            ((pipelineNode *)it)->replace_composite = unfold->UnfoldPipeline(((pipelineNode *)it));
            GraphToOperators(((pipelineNode *)(it))->replace_composite, ((pipelineNode *)(it))->replace_composite);
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
