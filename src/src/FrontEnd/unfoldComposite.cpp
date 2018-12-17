#include "unfoldComposite.h"

compositeNode *UnfoldComposite::UnfoldSplitJoin(splitjoinNode *node)
{
    compositeNode *tmp = NULL;
    string comName = MakeCompositeName("splitjoin");
    if (node->split->dup_round->type == RoundRobin)
        tmp = UnfoldDuplicate(comName, node);
    else
        tmp = UnfoldRoundrobin(comName, node);
    return tmp;
}

compositeNode *UnfoldComposite::UnfoldRoundrobin(string comName, splitjoinNode *node){
    compositeNode *tmp = NULL;
    
    return tmp;
}

compositeNode *UnfoldComposite::UnfoldDuplicate(string comName, splitjoinNode *node){
    compositeNode *tmp = NULL;
    return tmp;
}

compositeNode *UnfoldComposite::UnfoldPipeline(Node *node){
    compositeNode *tmp = NULL;
    return tmp;
}