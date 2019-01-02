#include "staticStreamGragh.h"
#include "unfoldComposite.h"
#include "compositeFlow.h"
static StaticStreamGraph *ssg = NULL;
extern UnfoldComposite *unfold;
//对composite节点展开成数据流图 oldComposite表示原来的compositecall,对于splitjoin，pipeline结构无区别
/* 暂时不清楚oldComposite的作用 用node*表示*/
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

/**
 * 
 * 
 * 
 */
StaticStreamGraph *AST2FlatStaticStreamGraph(compositeNode *mainComposite)
{
    ssg = new StaticStreamGraph();
    streamFlow(mainComposite);
    GraphToOperators(mainComposite, mainComposite);
    // for(auto it:ssg->flatNodes){
    //     cout<<it->nIn<<" "<<it->nOut<<endl;
    // }
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
