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
                list<Constant*> paramList;

                if(stream_List){
                    if(runningTop){
                        paramList = generateStreamList(stream_List,runningTop); //获得参数的值
                    }else{
                        paramList = generateStreamList(stream_List,exp->loc->first_line-1); //获得参数的值
                    }
                }

                runningTop = generateCompositeRunningContext(((compositeCallNode *)(exp))->actual_composite,paramList); //传入参数,并生成 composite 调用的执行上下文环境
                runningStack.push_back(runningTop); // 调用栈

                GraphToOperators(((compositeCallNode *)(exp))->actual_composite, exp);

                //出栈
                runningStack.pop_back();
                runningTop = runningStack.back();
                
            }
            else if (exp->type == SplitJoin)
            {
                debug("splitjoin %s\n",exp->toString().c_str());
                ((splitjoinNode *)exp)->replace_composite = unfold->UnfoldSplitJoin(((splitjoinNode *)exp));
                GraphToOperators(((splitjoinNode *)(exp))->replace_composite, ((splitjoinNode *)(exp))->replace_composite);
            }
            else if (exp->type == Pipeline)
            {
                debug("pipeline %s\n",exp->toString().c_str());
                ((pipelineNode *)exp)->replace_composite = unfold->UnfoldPipeline(((pipelineNode *)exp));
                GraphToOperators(((pipelineNode *)(exp))->replace_composite, ((pipelineNode *)(exp))->replace_composite);
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
            if(stream_List){
                if(runningTop){
                    paramList = generateStreamList(stream_List,runningTop); //获得参数的值
                }else{
                    paramList = generateStreamList(stream_List,(*it).loc->first_line-1); //嵌套调用 composite 获得参数的值
                }
            }
            runningTop = generateCompositeRunningContext(((compositeCallNode *)(it))->actual_composite,paramList); //传入参数,并生成 composite调用的执行上下文环境
            // 确定window大小
            
            runningStack.push_back(runningTop); // 调用栈

            GraphToOperators(((compositeCallNode *)it)->actual_composite, it);

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
            break;
        }
        case Pipeline:
        {
            debug("pipeline %s\n", it->toString().c_str());
            ((pipelineNode *)it)->replace_composite = unfold->UnfoldPipeline(((pipelineNode *)it));
            GraphToOperators(((pipelineNode *)(it))->replace_composite, ((pipelineNode *)(it))->replace_composite);
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
    streamFlow(mainComposite);
    debug("--------- 执行GraphToOperators, 逐步构建FlatNode ---------------\n");

    //list<Constant*> paramList;
    runningTop = &S; //传入参数,并生成 composite 调用的执行上下文环境
    runningStack.push_back(runningTop); // 调用栈

    //compositeCallNode *mainCompositeCall = new compositeCallNode(NULL,"main",NULL,NULL,mainComposite);
    list<Constant*> paramList = list<Constant*>();
    runningTop = generateCompositeRunningContext(mainComposite,paramList); //传入参数,并生成 composite 调用的执行上下文环境
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
