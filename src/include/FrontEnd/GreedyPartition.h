#ifndef _GREEDY_PARTITION_H_
#define _GREEDY_PARTITION_H_
#include "Partition.h"
#include <algorithm>
#include <map>
enum FlatNodeState
{
    INSIDE = 0,
    BORDER,
    ALONE
};

class GreedyPartition : public Partition
{
  public:
    GreedyPartition(SchedulerSSG *sssg)
    {
        nvtxs = 0;
        ee = 1.1;
    }
    ~GreedyPartition()
    {
    }
    void SssgPartition(SchedulerSSG *sssg, int level); //一级划分,调用doPartition
    void doPartition(SchedulerSSG *sssg, int k);
    int chooseMaxGain(SchedulerSSG *sssg, vector<FlatNode *> &S, vector<FlatNode *> &Xi, vector<FlatNode *> &X0);
    void updateCandidate(SchedulerSSG *sssg, vector<FlatNode *> &X0, vector<FlatNode *> &S, int index);
    int getPartEdge(int index)
    {
        return edge[index];
    }
    int getTotalEdge(SchedulerSSG *sssg, int k);
    int getPart(FlatNode *p)
    {
        for (int i = 0; i < X.size(); i++)
        {
            if (find(X[i].begin(), X[i].end(), p) != X[i].end())
                return i;
        }
    }
    int getPartWeight(int index)
    {
        return w[index];
    }

    void upDateFlatNodeState(FlatNode *p);
    FlatNodeState getFlatNodeState(FlatNode *p);
    void doTabuSearch(SchedulerSSG *sssg, int k);
    void errorDecrease(SchedulerSSG *sssg, int k);
    void errorDecrease2(SchedulerSSG *sssg, int k);
    int orderPartitionResult();
    void setActorWorkload(SchedulerSSG *sssg);

  private:
    vector<vector<FlatNode *>> X;                   //划分的结果
    vector<int> w;                                  //划分的K个子图每个子图的总工作量
    vector<int> edge;                               //划分的K个子图每个子图的通信边的权重
    vector<int> vwgt;                               //每个顶点的权重(总工作量)
    vector<FlatNode *> S;                           //候选节点的集合
    vector<int> part;                               //每个节点对应的核号
    int nvtxs;                                      //节点的个数
    double ee;                                      //平衡因子
    map<FlatNode *, FlatNodeState> FlatNodeToState; //每个节点对应的状态
};
#endif