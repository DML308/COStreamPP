#include "staticStreamGragh.h"
static StaticStreamGraph *ssg = NULL;

//对composite节点展开成数据流图
void GraphToOperators(compositeNode *composite, compositeNode *oldComposite)
{
    /* 获取compositebody内的statementNode */
    list<Node *> list = *(composite->body->stmt_List);
    cout<<"list.size()= "<<list.size()<<endl;
    for (auto it : (list))
    {
        cout<<it->type<<endl;
        switch (it->type)
        {
        // 当type为binop时候检查binop的右子节点是否为以下节点类型
        case Binop:
            cout<<"Binop"<<endl;
            break;
        case Operator_:
            cout <<((operatorNode*)it)->operName<<"     "<< "Operator_" << endl;
            //cout<<"Line:: "<<it->loc->first_line<<endl;
            break;
        case CompositeCall:
            cout << "compositeCall" << endl;
            break;
        case SplitJoin:
            cout << "SplitJoin" << endl;
            break;
        case Pipeline:
            cout << "Pipeline" << endl;
            break;
        }
    }
    

    return;
}

StaticStreamGraph *AST2FlatStaticStreamGraph(compositeNode *mainComposite)
{
    ssg = new StaticStreamGraph();
    GraphToOperators(mainComposite, mainComposite);
    return ssg;
}
