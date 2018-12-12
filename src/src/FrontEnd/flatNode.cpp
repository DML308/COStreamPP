#include "flatNode.h"
FlatNode::FlatNode(operatorNode *node, compositeNode *composite, compositeNode *newComposite)
{
    name = node->operName;
    composite = newComposite;
}