#ifndef _HEURISTIC_GREEDY_PARTITION_H_
#define _HEURISTIC_GREEDY_PARTITION_H_
#include "Partition.h"
#include "GreedyPartition.h"
#include <algorithm>
#include <map>

class HeuristicGreedyPartition : public GreedyPartition
{
  public:
    HeuristicGreedyPartition(SchedulerSSG *sssg) : GreedyPartition(sssg)
    {
        nvtxs = 0;
        ee = 1.1;
    }
    ~HeuristicGreedyPartition()
    {
    }
    /* 下面这个函数是 HeuristicGreedyPartion 重载的函数 */
    void SssgPartition(SchedulerSSG *sssg);
    /* 下面函数是 HeuristicGreedyPartion 新增的函数 */
    float getBalance(int k); //获取当前划分的平衡因子
    void disturb(SchedulerSSG *sssg, int k, int max_part, int min_part);
    //禁忌搜索通信优化
    void dynamicOptimization(SchedulerSSG *sssg, int k);                                    //最大化负载均衡，最小化通信边
    bool moveActor(SchedulerSSG *sssg, pair<FlatNode *, pair<int, int>> &candidate, int k); //尝试移动节点
    void updateGainCandidate(SchedulerSSG *sssg, int k);                                    //更新通信量收益增加列表
    void updateGainStructure(SchedulerSSG *sssg, int k);                                    //更新各顶点的邻接节点的信息
    void savePartitionResult(SchedulerSSG *sssg);

  private:
    //禁忌搜索通信量优化 新增的数据结构
    map<FlatNode *, int> partRecord;                     //记录节点所在的划分区间
    vector<pair<FlatNode *, pair<int, int>>> candidates; //候选的边界节点,第二个pair记录移动的收益和移动的方向
    map<FlatNode *, map<FlatNode *, int>> insideGraph;   //与节点相连接在同一个子图的节点,pair记录两个节点间的通信量
    map<FlatNode *, map<FlatNode *, int>> outsideGraph;  //与节点相连接在不同子图的节点,pair记录两个节点间的通信量
    map<FlatNode *, int> tabuList;                       //禁忌列表，保存禁止移动的点和禁止的步伐长度
};
#endif