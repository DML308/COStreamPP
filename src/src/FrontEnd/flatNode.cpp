#include "flatNode.h"
/* 后两个参数暂时都未用到 ，暂时不清楚oldcontent和content的区别 */
FlatNode::FlatNode(operatorNode *node, Node *oldCom, compositeNode *newCom)
{
    operName = node->operName;
    contents = node;
    oldContents = contents;
    nOut = 0;
    nIn = 0;
}