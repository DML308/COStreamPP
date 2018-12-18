#include "unfoldComposite.h"

/* 这个函数需要常量传播，目前为理想的情况 splitjoin,pipeline的循环结构都为常量*/
void UnfoldComposite::setCallList(list<Node *> *stmts)
{
    /*遍历splitjoin/pipeline结构中的statement，将compositecallNode加入到call_List中*/
    for (auto it : *(stmts))
    {
        //cout << it->type << endl;
        if (it->type == Add)
        {
            if (((addNode *)it)->content->type == CompositeCall)
                call_List.push_back(((addNode *)it)->content);
        }
        else if (it->type == For)
        {
            long long initial = MAX_INF;
            long long condition = MAX_INF;
            forNode *node = (forNode *)it;
            Node *init = node->init;
            expNode *cond = node->cond;
            expNode *next = node->next;
            /* 不一定是block */
            assert(node->stmt->type == Block);
            list<Node *> *stmts = ((blockNode *)(node->stmt))->stmt_List;
            assert(stmts->size() == 1);
            auto ptr = stmts->front();
            assert(ptr->type == Add);
            binopNode *init_b = NULL, *cond_b = NULL, *next_b = NULL;
            if (init->type == Decl)
            {
            }
            else if (init->type == Binop)
            {
                init_b = (binopNode *)init;
                assert(init_b->right->type == constant);
                constantNode *con_init = (constantNode *)(init_b->right);
                assert(con_init->style == "interger");
                initial = con_init->llval;
            }
            assert(cond->type == Binop);
            cond_b = (binopNode *)cond;
            assert(cond_b->right->type == constant);
            constantNode *con_cond = (constantNode *)(cond_b->right);
            assert(con_cond->style == "interger");
            condition = con_cond->llval;
            cout << "init= " << initial << " cond= " << condition << endl;
            assert(initial!=MAX_INF && condition!=MAX_INF);
            for (long long i = initial; i < condition; ++i)
            {
                if (((addNode *)ptr)->content->type == CompositeCall)
                {
                    call_List.push_back(((addNode *)ptr)->content);
                }
            }
        }
    }
}

operatorNode *UnfoldComposite::MakeSplitOperator(Node *input, list<Node *> *arguments, int style)
{
    operatorNode *res = NULL;
    vector<string> operName({"Duplicate", "Roundrobin"});
    vector<Node*> outputs;
    int len=call_List.size();

    //operBodyNode *body=new operBodyNode(NULL,NULL,NULL,windows);

    //operatorNode *res=new operatorNode();

    return res;
}

compositeNode *UnfoldComposite::UnfoldSplitJoin(splitjoinNode *node)
{
    compositeNode *tmp = NULL;
    string comName = MakeCompositeName("splitjoin");

    if (node->split->dup_round->type == RoundRobin)
    {
        setCallList(node->bodyStmt_List);
        tmp = UnfoldRoundrobin(comName, node);
    }
    else
        tmp = UnfoldDuplicate(comName, node);
    return tmp;
}

compositeNode *UnfoldComposite::UnfoldRoundrobin(string comName, splitjoinNode *node)
{
    compositeNode *tmp = NULL;
    operatorNode *spiltOperator = NULL, *joinOperator = NULL;
    /* arg_list表示split roundrobin(size);的size参数列表 */
    list<Node *> *arg_list = ((roundrobinNode *)node->split->dup_round)->arg_list;
    list<Node *> *inputs = node->inputs;
    list<Node *> *outputs = node->outputs;
    ComInOutNode *inout = new ComInOutNode(inputs, outputs, NULL);
    compHeadNode *head = new compHeadNode(comName, inout);
    assert(inputs != NULL && outputs != NULL);
    cout << "inputs.size()= " << inputs->size() << " outputs.size()= " << outputs->size() << endl;

    call_List.clear();
    return tmp;
}

compositeNode *UnfoldComposite::UnfoldDuplicate(string comName, splitjoinNode *node)
{
    compositeNode *tmp = NULL;
    //cout<<"----------------------------------------------"<<endl;
    return tmp;
}

compositeNode *UnfoldComposite::UnfoldPipeline(Node *node)
{
    compositeNode *tmp = NULL;
    return tmp;
}