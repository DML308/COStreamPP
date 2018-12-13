#ifndef _UNFOLD_COMPOSITE_H
#define _UNFOLD_COMPOSITE_H
#include "node.h"
class UnfoldComposite
{
  public:
    int gCurrentCompositeNum;
    UnfoldComposite() {
        gCurrentCompositeNum=0;
    }
    /* 给与每一个不同的splitjoin或者pipeline节点不同的名字 */
    inline string MakeCompositeName(string name){
        return name+to_string(gCurrentCompositeNum);
    }
    compositeNode *UnfoldRoundrobin(string comName, splitjoinNode *node);
    compositeNode *UnfoldDuplicate(string comName, splitjoinNode *node);
    compositeNode *UnfoldPipeline(Node *node);
    compositeNode *UnfoldSplitJoin(splitjoinNode *node);
};
#endif