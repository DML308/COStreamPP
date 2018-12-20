#ifndef _STATIC_STREAM_GRAPH_H_
#define _STATIC_STREAM_GRAPH_H_

#include "flatNode.h"
/* 保存所有的展开 operator */
static list<operatorNode *> *oper_List = new list<operatorNode *>();
class StaticStreamGraph
{
public:
  FlatNode *topNode; // SDF图的起始节点，假设只有一个输入为0的节点
  StaticStreamGraph() {}
  vector<FlatNode *> flatNodes; // 静态数据流图所有节点集合
  //map<operatorNode *, FlatNode *> mapOper2FlatNode;
  multimap<Node *, FlatNode *> mapEdge2UpFlatNode;   // 将有向边与其上端绑定
  multimap<Node *, FlatNode *> mapEdge2DownFlatNode; //将有向边与其下端绑定
  map<FlatNode *, int> mapSteadyWork2FlatNode;       // 存放各个operator的workestimate（稳态工作量估计）
  map<FlatNode *, int> mapInitWork2FlatNode;         // 存放各个operator的workestimate（初态）
  // 构建SDF图，初始化flatNodes, mapEdge2FlatNode
  void GenerateFlatNodes(operatorNode *u, Node *oldComposite, compositeNode *newComposite);
  void SetTopLevel()
  {
    topNode = flatNodes[0];
  }
  void SetFlatNodesWeights();
  void ResetFlatNodeNames();
};
void GraphToOperators(compositeNode *composite, Node *oldComposite);
StaticStreamGraph *AST2FlatStaticStreamGraph(compositeNode *mainComposite);

#endif