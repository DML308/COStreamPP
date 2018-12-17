#include "unfoldComposite.h"

void UnfoldComposite::setCallList(list<Node *> *stmts)
{
    /*遍历splitjoin/pipeline结构中的statement，将compositecallNode加入到call_List中*/
    for (auto it : *(stmts))
    {
        //cout << it->type << endl;
        if (it->type == Add)
        {
            if (((addNode *)it)->content->type == CompositeCall)
                call_List->push_back((compositeCallNode *)((addNode *)it)->content);
        }
        else if (it->type == For)
        {
            long long initial = MAX_INF;
            long long condition = MAX_INF;
            forNode *node = (forNode *)it;
            Node *init = node->init;
            expNode *cond = node->cond;
            expNode *next = node->next;
            assert(node->stmt->type == Block);
            list<Node *> *stmts = ((blockNode *)(node->stmt))->stmt_List;
            assert(stmts->size() == 1);
            auto ptr = *(stmts->begin());
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

            for (long long i = initial; i < condition; ++i)
            {
                //  if (((addNode *)ptr)->content->type == CompositeCall){
                //      if(call_List==NULL)
                //         call_List=new list<compositeCallNode*>({(compositeCallNode *)((addNode *)ptr)->content});
                //     else
                //     call_List->push_back((compositeCallNode *)((addNode *)ptr)->content);
                //  }
                    
                //call_List->push_back(NULL);
            }
            if(call_List!=NULL) cout << "size= " << call_List->size() << endl;
        }
    }
}

operatorNode *UnfoldComposite::MakeSplitOperator(Node *input, list<Node *> *arguments, int style)
{
    operatorNode *res = NULL;
    vector<string> operName({"Duplicate", "Roundrobin"});

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
        tmp = UnfoldRoundrobin(comName, node);
        setCallList(node->bodyStmt_List);
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