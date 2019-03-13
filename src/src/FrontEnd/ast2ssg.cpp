#include "staticStreamGragh.h"
#include "unfoldComposite.h"
#include "compositeFlow.h"

static StaticStreamGraph *ssg = NULL;
extern UnfoldComposite *unfold;
/*
* 功能：递归的调用，完成splitjoin和pipeline节点的展开，以及完成opearatorNode到flatnode节点的映射
* 输入参数：composite
* 输出：设置静态数据流图的对应flatNode节点，完成数据流边到flatNode节点的映射
*/
void GraphToOperators(compositeNode *composite, Node *oldComposite)
{
    /* 获取compositebody内的statementNode */
    assert(composite != NULL && oldComposite != NULL);
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
                cout << "operator_" << endl;
                ssg->GenerateFlatNodes((operatorNode *)exp, oldComposite, composite);
            }
            else if (exp->type == CompositeCall)
            {
                cout << "compositeCall" << endl;
                GraphToOperators(((compositeCallNode *)(exp))->actual_composite, exp);
            }
            else if (exp->type == SplitJoin)
            {
                cout << "splitjoin" << endl;
                ((splitjoinNode *)exp)->replace_composite = unfold->UnfoldSplitJoin(((splitjoinNode *)exp));
                GraphToOperators(((splitjoinNode *)(exp))->replace_composite, ((splitjoinNode *)(exp))->replace_composite);
            }
            else if (exp->type == Pipeline)
            {
                cout << "pipeline" << endl;
                ((pipelineNode *)exp)->replace_composite = unfold->UnfoldPipeline(((pipelineNode *)exp));
                GraphToOperators(((pipelineNode *)(exp))->replace_composite, ((pipelineNode *)(exp))->replace_composite);
            }
            break;
        }
        case Operator_:
        {
            cout << "operator_" << endl;
            ssg->GenerateFlatNodes((operatorNode *)it, oldComposite, composite);
            break;
        }
        case CompositeCall:
        {
            cout << "compositeCall" << endl;
            GraphToOperators(((compositeCallNode *)it)->actual_composite, it);
            break;
        }
        case SplitJoin:
        {
            cout << "splitjoin" << endl;
            ((splitjoinNode *)it)->replace_composite = unfold->UnfoldSplitJoin(((splitjoinNode *)it));
            GraphToOperators(((splitjoinNode *)(it))->replace_composite, ((splitjoinNode *)(it))->replace_composite);
            break;
        }
        case Pipeline:
        {
            cout << "pipeline" << endl;
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
    //cout << "--------- 执行GraphToOperators, 逐步构建FlatNode ---------------\n";
    GraphToOperators(mainComposite, mainComposite);
    ssg->SetTopNode();
    /* 将每个composite重命名 */
    ssg->ResetFlatNodeNames();
    ssg->SetFlatNodesWeights();
    /* 测试peek，pop，push值 */

    cout << "--------- 执行AST2FlatStaticStreamGraph后, 查看静态数据流图中的全部 FlatNode ---------------\n";
    for (auto it : ssg->flatNodes){
        cout<<it->name<<":\t"<<it->toString()<<endl;
        if(it!=ssg->flatNodes.back())cout << "    ↓" << endl;
    }
    return ssg;
}
