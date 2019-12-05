#ifndef _UNFOLD_COMPOSITE_H
#define _UNFOLD_COMPOSITE_H
#include "node.h"
#include "symbol.h"
class UnfoldComposite
{
  public:
    int num;
    UnfoldComposite()
    {
        num = 0;
    }
    /* 给与每一个不同的splitjoin或者pipeline节点不同的名字 */
    inline string MakeCompositeName(string name)
    {
        return name +"_"+ to_string(num);
    }
    compositeNode *UnfoldRoundrobin(string comName, splitjoinNode *node);
    compositeNode *UnfoldDuplicate(string comName, splitjoinNode *node);
    compositeNode *UnfoldPipeline(pipelineNode *node);
    compositeNode *UnfoldSplitJoin(splitjoinNode *node);
    compositeNode *UnfoldSquential(squentialNode *node);
    Node *MakeRoundrobinWork(list<Node *> *input, list<Node *> *arguments, list<Node*> *outputs);
    Node *MakeJoinWork(list<Node *> *input, list<Node *> *arguments, list<Node*> *outputs);
    operatorNode *MakeSplitOperator(Node *input, list<Node *> *arguments, int style);
    operatorNode *MakeJoinOperator(Node *output, list<Node *> *inputs, list<Node *> *arguments);
    compositeNode *streamReplace(compositeNode *composite, list<Node *> *inputs, list<Node *> *outputs,int flag);
    compositeNode *compositeCallStreamReplace(compositeNode *composite, list<Node *> *inputs, list<Node *> *outputs);
    void modifyStreamName(operatorNode *oper,list<Node*> *stream ,bool style );
    void modifyWorkName(Node *u,string replaceName,string name);
    Node* workNodeCopy(Node *u);
    compositeNode* makeInputComposite(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs);
    operatorNode* makeInputOperator(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs);
    compositeNode* makeForwardComposite(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs);
    compositeNode* makeBackComposite(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs);
    compositeNode* makeLossComposite(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs);
    operatorNode* makeDenseOperator(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs);
    Node* makeDenseInit(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs);
    Node* makeDenseWork(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs);
    operatorNode* makeActivationOperator(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs);
    operatorNode* makeLossOperator(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs);
    operatorNode* makeDDenseOperator(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs);
    Node* makeDDenseWork(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs);
    Node* makeStream(string name, string type);
    compositeNode* makeConv2DLayer(layerNode *layer);
    Node* makeConv2DLayerBody(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs);
    Node* makeConv2DKernel(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs);
    Node* makeConv2DKernelInit(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs);
    Node* makeConv2DKernelWork(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs);
};
#endif