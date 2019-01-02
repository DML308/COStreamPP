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
                //cout << "Operator_" << endl;
                ssg->GenerateFlatNodes((operatorNode *)exp, oldComposite, composite);
            }
            else if (exp->type == CompositeCall)
            {
                //cout<<"compositeCall"<<endl;
                GraphToOperators(((compositeCallNode *)(exp))->actual_composite, exp);
            }
            else if (exp->type == SplitJoin)
            {
                //cout << "SplitJoin" << endl;
                ((splitjoinNode *)exp)->replace_composite = unfold->UnfoldSplitJoin(((splitjoinNode *)exp));
                GraphToOperators(((splitjoinNode *)(exp))->replace_composite, ((splitjoinNode *)(exp))->replace_composite);
            }
            else if (exp->type == Pipeline)
            {
                //cout<<"pipeline"<<endl;
                ((pipelineNode *)exp)->replace_composite = unfold->UnfoldPipeline(((pipelineNode *)exp));
                GraphToOperators(((pipelineNode *)(exp))->replace_composite, ((pipelineNode *)(exp))->replace_composite);
            }
            break;
        }
        case Operator_:
        {
            //cout << "Operator_" << endl;
            ssg->GenerateFlatNodes((operatorNode *)it, oldComposite, composite);
            break;
        }
        case CompositeCall:
        {
            //cout << "compositeCall" << endl;
            GraphToOperators(((compositeCallNode *)it)->actual_composite, it);
            break;
        }
        case SplitJoin:
        {
            //cout << "SplitJoin" << endl;
            ((splitjoinNode *)it)->replace_composite = unfold->UnfoldSplitJoin(((splitjoinNode *)it));
            GraphToOperators(((splitjoinNode *)(it))->replace_composite, ((splitjoinNode *)(it))->replace_composite);
            break;
        }
        case Pipeline:
        {
            //cout << "Pipeline" << endl;
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
    GraphToOperators(mainComposite, mainComposite);
    ssg->SetTopNode();
    /* 将每个composite重命名 */
    ssg->ResetFlatNodeNames();
    ssg->SetFlatNodesWeights();
    /* 测试peek，pop，push值 */

    // for (auto it : ssg->flatNodes){
    // 	cout << "push: ";
    // 	for (auto it2 : it->outPushWeights){
    // 		cout << it2 << " ";
    // 	}
    // 	cout << endl;
    // 	cout << "pop: ";
    // 	for (auto it3 : it->inPopWeights){
    // 		cout << it3 << " ";
    // 	}
    // 	cout << endl;
    // 	cout << "peek: ";
    // 	for (auto it4 : it->inPeekWeights){
    // 		cout << it4 << " ";
    // 	}
    // 	cout << endl << endl;
    // }
    return ssg;
}
