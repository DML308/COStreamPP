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

    compositeNode *UnfoldSequential(sequentialNode *node);
    Node *MakeRoundrobinWork(list<Node *> *input, list<constantNode *> *arguments, list<Node*> *outputs,int style);
    Node *MakeJoinWork(list<Node *> *input, list<constantNode *> *arguments, list<Node*> *outputs);
    compositeCallNode *MakeSplitOperator(Node *input, list<Node *> *arguments, int style);
    compositeCallNode *MakeJoinOperator(Node *output, list<Node *> *inputs, list<Node *> *arguments);
    
    compositeNode *streamReplace(compositeNode *composite, list<Node *> *inputs, list<Node *> *outputs,int flag);
    compositeNode *compositeCallStreamReplace(compositeNode *composite, list<Node *> *inputs, list<Node *> *outputs);
    void modifyStreamName(operatorNode *oper,list<Node*> *stream ,bool style );
    void modifyWorkName(Node *u,string replaceName,string name);
    Node* workNodeCopy(Node *u);

    compositeNode* makeForwardComposite(layerNode *layer);
    compositeNode* makeBackComposite(layerNode *layer);
    compositeNode* makeLossComposite(layerNode *layer);
    operatorNode* makeDenseOperator(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs);
    Node* makeDenseInit(layerNode *layer);
    Node* makeDenseWork(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs);
    operatorNode* makeActivationOperator(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs);
    operatorNode* makeLossOperator(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs);
    operatorNode* makeDDenseOperator(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs);
    Node* makeDDenseWork(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs);
    Node* makeStream(string name, string type);
    compositeNode* makeConv2DLayer(layerNode *layer);
    Node* makeConv2DLayerBody(layerNode *layer, list<Node *> *inputs_id, list<Node *> *outputs_id);
    compositeNode* makeConv2DKernel(layerNode *layer/*, Node *kernelIndex */);
    Node* makeConv2DKernelBody(layerNode *layer, list<Node *> *inputs_id, list<Node *> *outputs_id/*, Node *kernelIndex */);
    Node* makeConv2DKernelOperInit(layerNode *layer/*, Node *kernelIndex */);
    Node* makeConv2DKernelOperWork(layerNode *layer, list<Node *> *inputs_id, list<Node *> *outputs_id/*, Node *kernelIndex */);
    operatorNode* makeConv2DKernelOper(layerNode *layer, list<Node *> *inputs_id, list<Node *> *outputs_id/*, Node *kernelIndex */);
    compositeNode* makeCopyComp();
    compositeNode* makeDConv2DLayer(layerNode *layer);
    Node* makeDConv2DLayerBody(layerNode *layer, list<Node *> *inputs_id, list<Node *> *outputs_id);
    compositeNode* makeDConv2DKernel(layerNode *layer/*,  Node *kernelIndex*/);
    Node* makeDConv2DKernelBody(layerNode *layer, list<Node *> *inputs_id, list<Node *> *outputs_id/*,  Node *kernelIndex*/);
    Node* makeDConv2DKernelOperWork(layerNode *layer, list<Node *> *inputs_id, list<Node *> *outputs_id/*,  Node *kernelIndex*/);
    operatorNode* makeDConv2DKernelOper(layerNode *layer, list<Node *> *inputs_id, list<Node *> *outputs_id/*,  Node *kernelIndex*/);
    operatorNode* makeConv2DDilateAndExtendOperator(layerNode *layer,list<Node *> *inputs_id, list<Node *> *outputs_id);
    Node* makeConv2DDilateAndExtendOperWork(conv2DLayerNode *layer,list<Node *> *inputs_id, list<Node *> *outputs_id);
    // style 默认为0, 表示duplicate, style = 1 时,表示roundrobin
    operatorNode* makeSpecialSplitOperator(Node* input, long long splitCount, layerNode* layer,int style = 0);
    operatorNode* makeSpecialJoinOperator(Node *output, list<Node *> *inputs);
    compositeNode* makeMaxPooling2DLayer(layerNode *layer);
    Node* makeMaxPooling2DLayerBody(maxPooling2DLayerNode* layer, list<Node *> *inputs_id, list<Node *> *outputs_id);
    compositeNode* makeMaxPooling2DKernel(maxPooling2DLayerNode *layer);
    Node* makeMaxPooling2DKernelBody(maxPooling2DLayerNode *layer, list<Node *> *inputs_id, list<Node *> *outputs_id);
    Node* makeMaxPooling2DKernelOperWork(maxPooling2DLayerNode *layer, list<Node *> *inputs_id, list<Node *> *outputs_id);
    operatorNode* makeMaxPooling2DKernelOper(maxPooling2DLayerNode *layer, list<Node *> *inputs_id, list<Node *> *outputs_id);
    compositeNode* makeDMaxPooling2DLayer(maxPooling2DLayerNode *layer);
    Node* makeDMaxPooling2DLayerBody(maxPooling2DLayerNode* layer, list<Node *> *inputs_id, list<Node *> *outputs_id);
    compositeNode* makeDMaxPooling2DKernel(maxPooling2DLayerNode *layer);
    Node* makeDMaxPooling2DKernelBody(maxPooling2DLayerNode *layer, list<Node *> *inputs_id, list<Node *> *outputs_id);
    Node* makeDMaxPooling2DKernelOperWork(maxPooling2DLayerNode *layer, list<Node *> *inputs_id, list<Node *> *outputs_id);
    operatorNode* makeDMaxPooling2DKernelOper(maxPooling2DLayerNode *layer, list<Node *> *inputs_id, list<Node *> *outputs_id);
    compositeNode* makeAveragePooling2DLayer(layerNode *layer);
    Node* makeAveragePooling2DLayerBody(averagePooling2DLayerNode* layer, list<Node *> *inputs_id, list<Node *> *outputs_id);
    compositeNode* makeAveragePooling2DKernel(averagePooling2DLayerNode *layer);
    Node* makeAveragePooling2DKernelBody(averagePooling2DLayerNode *layer, list<Node *> *inputs_id, list<Node *> *outputs_id);
    Node* makeAveragePooling2DKernelOperWork(averagePooling2DLayerNode *layer, list<Node *> *inputs_id, list<Node *> *outputs_id);
    operatorNode* makeAveragePooling2DKernelOper(averagePooling2DLayerNode *layer, list<Node *> *inputs_id, list<Node *> *outputs_id);
    compositeNode* makeDAveragePooling2DLayer(averagePooling2DLayerNode *layer);
    Node* makeDAveragePooling2DLayerBody(averagePooling2DLayerNode* layer, list<Node *> *inputs_id, list<Node *> *outputs_id);
    compositeNode* makeDAveragePooling2DKernel(averagePooling2DLayerNode *layer);
    Node* makeDAveragePooling2DKernelBody(averagePooling2DLayerNode *layer, list<Node *> *inputs_id, list<Node *> *outputs_id);
    Node* makeDAveragePooling2DKernelOperWork(averagePooling2DLayerNode *layer, list<Node *> *inputs_id, list<Node *> *outputs_id);
    operatorNode* makeDAveragePooling2DKernelOper(averagePooling2DLayerNode *layer, list<Node *> *inputs_id, list<Node *> *outputs_id);
};
#endif