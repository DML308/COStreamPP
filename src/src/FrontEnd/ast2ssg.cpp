#include "staticStreamGragh.h"
#include "unfoldComposite.h"
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
                //cout << "Pipeline" << endl;
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
            break;
        }
        default:
            break;
        }
    }
    return;
}


void streamFlow(compositeNode *main)
{
    list<Node *> body_stmt = *(main->body->stmt_List);
    for (auto it : body_stmt)
    {
        switch (it->type)
        {
        case Binop:
        {
            expNode *right = static_cast<binopNode *>(it)->right;
            if (right->type == Operator_)
            {
            }
            else if (right->type == CompositeCall)
            {
                compositeNode *comp = ((compositeCallNode *)right)->actual_composite;
                ((compositeCallNode *)right)->actual_composite = unfold->streamReplace(comp, ((compositeCallNode *)right)->inputs, ((compositeCallNode *)right)->outputs,1);
            }
            else if (right->type == SplitJoin)
            {
            }
            else if (right->type == Pipeline)
            {
            }
            break;
        }
        case Operator_:
        {
            break;
        }
        case CompositeCall:
        {
            compositeNode *comp = ((compositeCallNode *)it)->actual_composite;
            ((compositeCallNode *)it)->actual_composite = unfold->streamReplace(comp, ((compositeCallNode *)it)->inputs, ((compositeCallNode *)it)->outputs,1);
            break;
        }
        case SplitJoin:
        {
            break;
        }
        case Pipeline:
        {
            break;
        }
        default:
            break;
        }
    }
}

StaticStreamGraph *AST2FlatStaticStreamGraph(compositeNode *mainComposite)
{
    ssg = new StaticStreamGraph();
    streamFlow(mainComposite);
    GraphToOperators(mainComposite, mainComposite);
    
    // for(auto it:ssg->flatNodes){
    //     cout<<it->nIn<<" "<<it->nOut<<endl;
    // }
    ssg->SetTopLevel();
    /* 将每个composite重命名 */
    ssg->ResetFlatNodeNames();
    ssg->SetFlatNodesWeights();

    return ssg;
}
