#ifndef _UNFOLD_COMPOSITE_H
#define _UNFOLD_COMPOSITE_H
#include "node.h"
class UnfoldComposite
{
  public:
    int compNum;
    list<compositeCallNode*>* call_List;
    UnfoldComposite() {
        compNum=0;
        call_List=NULL;
    }
    /* 给与每一个不同的splitjoin或者pipeline节点不同的名字 */
    inline string MakeCompositeName(string name){
        return name+to_string(compNum);
    }
    compositeNode *UnfoldRoundrobin(string comName, splitjoinNode *node);
    compositeNode *UnfoldDuplicate(string comName, splitjoinNode *node);
    compositeNode *UnfoldPipeline(Node *node);
    compositeNode *UnfoldSplitJoin(splitjoinNode *node);
    operatorNode  *MakeSplitOperator(Node *input, list<Node*> *arguments, int style);

};
#endif