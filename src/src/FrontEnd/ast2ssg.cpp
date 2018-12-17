#include "staticStreamGragh.h"
static StaticStreamGraph *ssg = NULL;

//对composite节点展开成数据流图
void GraphToOperators(compositeNode *composite)
{
    /* 获取compositebody内的statementNode */
    list<Node *> list = *(composite->body->stmt_List);
    //cout << "list.size()= " << list.size() << endl;
    for (auto it : (list))
    {
        //cout << it->type << endl;
        // 当type为binop时候检查binop的右子节点是否为以下节点类型
        if (it->type == Binop)
        {
            expNode *exp = static_cast<binopNode *>(it)->right;
            if (exp->type == Operator_)
            {
                cout << "Operator_" << endl;
            }
            else if (exp->type == CompositeCall)
            {
                cout << "compositeCall" << endl;
                GraphToOperators(((compositeCallNode *)(exp))->actual_composite);
            }
            else if (exp->type == SplitJoin)
            {
                cout << "SplitJoin" << endl;
            }
            else if (exp->type == Pipeline)
            {
                cout << "Pipeline" << endl;
            }
        }
        else if (it->type == Operator_)
        {
            cout << "Operator_" << endl;
        }
        else if (it->type == CompositeCall)
        {
            cout << "compositeCall" << endl;
            GraphToOperators(((compositeCallNode *)it)->actual_composite);
        }
        else if (it->type == SplitJoin)
        {
            cout << "SplitJoin" << endl;
        }
        else if (it->type == Pipeline)
        {
            cout << "Pipeline" << endl;
        }
    }
    return;
}

StaticStreamGraph *AST2FlatStaticStreamGraph(compositeNode *mainComposite)
{
    ssg = new StaticStreamGraph();
    GraphToOperators(mainComposite);
    return ssg;
}
