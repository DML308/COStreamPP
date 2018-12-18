#ifndef _STATIC_STREAM_GRAPH_H_
#define _STATIC_STREAM_GRAPH_H_

#include "flatNode.h"
#include <set>

class StaticStreamGraph
{
public:
  StaticStreamGraph() {}
  vector<FlatNode *> flatNodes;// SDF图所有节点集合
  map<Node *, FlatNode *> mapEdge2UpFlatNode;        // 将有向边与其上端绑定
  multimap<Node *, FlatNode *> mapEdge2DownFlatNode; //将有向边与其下端绑定
  map<FlatNode *, int> mapSteadyWork2FlatNode; // 存放各个operator的workestimate（稳态工作量估计）
  map<FlatNode *, int> mapInitWork2FlatNode;   // 存放各个operator的workestimate（初态）
  // 构建SDF图，初始化flatNodes, mapEdge2FlatNode
  void GenerateFlatNodes(operatorNode *u, Node *oldComposite,compositeNode *newComposite);
  void AddOutEdges(FlatNode* dest);
  void AddInEdges(FlatNode *src);
};
void GraphToOperators(compositeNode *composite, Node *oldComposite);
StaticStreamGraph *AST2FlatStaticStreamGraph(compositeNode *mainComposite);

#endif