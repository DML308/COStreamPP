#include "unfoldComposite.h"

compositeNode *UnfoldComposite::UnfoldSplitJoin(splitjoinNode *node)
{
    compositeNode *tmp = NULL;
    string comName = MakeCompositeName("splitjoin");
    
    if (node->split->dup_round->type == RoundRobin)
        tmp = UnfoldRoundrobin(comName, node);
    else
        tmp = UnfoldDuplicate(comName, node);
    return tmp;
}

compositeNode *UnfoldComposite::UnfoldRoundrobin(string comName, splitjoinNode *node)
{
    compositeNode *tmp = NULL;
    operatorNode *spiltOperator=NULL,*joinOperator=NULL;
    list<Node *> *inputs = node->inputs;
    list<Node *> *outputs = node->outputs;
    ComInOutNode *inout = new ComInOutNode(inputs, outputs, NULL);
    compHeadNode *head = new compHeadNode(comName, inout);
    assert(inputs!=NULL&&outputs!=NULL);
    cout<< "inputs.size()= " <<inputs->size()<<" outputs.size()= "<<outputs->size() <<endl;


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