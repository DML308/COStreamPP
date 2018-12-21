#include "staticStreamGragh.h"
static StaticStreamGraph *ssg = NULL;

//对composite节点展开成数据流图 oldComposite表示原来的compositecall,对于splitjoin，pipeline结构无区别
/* 暂时不清楚oldComposite的作用 用node*表示*/
void GraphToOperators(compositeNode *composite, Node *oldComposite)
{
    /* 获取compositebody内的statementNode */
    assert(composite!=NULL && oldComposite!=NULL);
    list<Node *> body_stmt = *(composite->body->stmt_List);
    //cout << "list.size()= " << list.size() << endl;
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
                //oper_List->push_back((operatorNode*)exp);
                ssg->GenerateFlatNodes((operatorNode*)exp, oldComposite, composite);
            }
            else if (exp->type == CompositeCall)
            {
                //cout << "compositeCall" << endl;
                GraphToOperators(((compositeCallNode *)(exp))->actual_composite, exp);
            }
            else if (exp->type == SplitJoin)
            {
                //cout << "SplitJoin" << endl;
                GraphToOperators(((splitjoinNode *)(exp))->replace_composite, ((splitjoinNode *)(exp))->replace_composite);
            }
            else if (exp->type == Pipeline)
            {
                cout << "Pipeline" << endl;
            }
            break;
        }
        case Operator_:
        {
            //cout << "Operator_" << endl;
            //oper_List->push_back((operatorNode*)it);
            ssg->GenerateFlatNodes((operatorNode*)it, oldComposite, composite);
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
            GraphToOperators(((splitjoinNode *)(it))->replace_composite, ((splitjoinNode *)(it))->replace_composite);
            break;
        }
        case Pipeline:
        {
            cout << "Pipeline" << endl;
            break;
        }
        default:
            break;
        }
    }
    return;
}

StaticStreamGraph *AST2FlatStaticStreamGraph(compositeNode *mainComposite)
{
    ssg = new StaticStreamGraph();
    GraphToOperators(mainComposite, mainComposite);
    /*  打印为dot图的composite名字 
    for(auto it:ssg->flatNodes){
        cout<<it->nIn<<" "<<it->nOut<<endl;
    }
    */
    ssg->SetTopLevel();
    /* 将每个composite重命名 */
    ssg->ResetFlatNodeNames();
    ssg->SetFlatNodesWeights();

    return ssg;
}
