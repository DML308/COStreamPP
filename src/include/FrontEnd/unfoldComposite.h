#ifndef _UNFOLD_COMPOSITE_H
#define _UNFOLD_COMPOSITE_H
#include "node.h"
#include "symbol.h"
class UnfoldComposite
{
  public:
    int compNum;
    vector<Node *> call_List;
    UnfoldComposite()
    {
        compNum = 0;
    }
    /* 给与每一个不同的splitjoin或者pipeline节点不同的名字 */
    inline string MakeCompositeName(string name)
    {
        return name + to_string(compNum);
    }
    void setCallList(list<Node *> *stmt);
    compositeNode *UnfoldRoundrobin(string comName, splitjoinNode *node);
    compositeNode *UnfoldDuplicate(string comName, splitjoinNode *node);
    compositeNode *UnfoldPipeline(Node *node);
    compositeNode *UnfoldSplitJoin(splitjoinNode *node);
    operatorNode *MakeSplitOperator(Node *input, list<Node *> *arguments, int style);
    operatorNode *MakeJoinOperator(Node *output, list<Node *> *inputs, list<Node *> *arguments);
    compositeNode *streamReplace(compositeNode *comp, list<Node *> *input, list<Node *> *output);
};
#endif