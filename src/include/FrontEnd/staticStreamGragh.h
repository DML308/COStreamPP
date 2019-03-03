#ifndef _STATIC_STREAM_GRAPH_H_
#define _STATIC_STREAM_GRAPH_H_

#include "flatNode.h"
/* 保存所有的展开 operator */
class StaticStreamGraph
{
public:
  FlatNode *topNode; // SDF图的起始节点，假设只有一个输入为0的节点
  StaticStreamGraph() {}
  vector<FlatNode *> flatNodes; // 静态数据流图所有节点集合
  //map<operatorNode *, FlatNode *> mapOper2FlatNode;
  map<string, FlatNode *> mapEdge2UpFlatNode; // 将有向边与其上端绑定
  map<string, FlatNode *> mapEdge2DownFlatNode; //将有向边与其下端绑定
  map<FlatNode *, int> mapSteadyWork2FlatNode;  // 存放各个operator的workestimate（稳态工作量估计）
  map<FlatNode *, int> mapInitWork2FlatNode;    // 存放各个operator的workestimate（初态）
  // 构建SDF图，初始化flatNodes, mapEdge2FlatNode
  void GenerateFlatNodes(operatorNode *u, Node *oldComposite, compositeNode *newComposite);
  void SetFlatNodesWeights();
  void ResetFlatNodeNames();
  void ResetFlatNodeVisitTimes();      // 重置每个flastnode的visit times值
  void AddSteadyWork(FlatNode *, int); // 存放稳态调度工作量
  void AddInitWork(FlatNode *, int);   // 存放初态调度工作量
  inline void SetTopNode()
  {
    topNode = flatNodes[0];
  }
  inline vector<FlatNode *> GetFlatNodes()
  {
    return flatNodes;
  }
  inline map<string, FlatNode *> GetMapEdge2UpFlatNode()
  {
    return mapEdge2UpFlatNode;
  }
  inline map<string, FlatNode *> GetMapEdge2DownFlatNode()
  {
    return mapEdge2DownFlatNode;
  }
  inline map<FlatNode *, int> GetSteadyWorkMap(void)
  {
    return mapSteadyWork2FlatNode;
  }
  inline map<FlatNode *, int> GetInitWorkMap(void)
  {
    return mapInitWork2FlatNode;
  }
  
protected:
};

void GraphToOperators(compositeNode *composite, Node *oldComposite);
StaticStreamGraph *AST2FlatStaticStreamGraph(compositeNode *mainComposite);
void WorkEstimate(StaticStreamGraph *ssg);

#endif