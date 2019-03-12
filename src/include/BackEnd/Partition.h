#ifndef _PARTITION_H
#define _PARTITION_H
#include "6.schedulerSSG.h"
class Partition
{
public:
  //所有调度方法均会使用到的成员变量
  map<FlatNode *, int> FlatNode2PartitionNum;     //节点到划分编号的映射
  multimap<int, FlatNode *> PartitonNum2FlatNode; //划分编号到节点的映射
  vector<FlatNode *> PartitonNumSet;              //用于记录返回的节点集合
  int mnparts;                                    //划分的份数，依赖于指定的places个数

public:
  Partition();
  virtual void SssgPartition(SchedulerSSG *sssg, int level) = 0; //划分成员方法，具体实现由子类实现
  int findID(SchedulerSSG *sssg, FlatNode *flatnode);            //根据flatnode找到其下标号 如source_0中的0
  void setCpuCoreNum(int, SchedulerSSG *);                       //设置place数目（即进程数目）
  vector<FlatNode *> findNodeSetInPartition(int partitionNum);   //根据编号num查找其中的节点，将节点集合返回给PartitonNumSet(编号->节点)
  int findPartitionNumForFlatNode(FlatNode *flatnode);           //根据节点返回其所在划分区的编号(节点->编号) for dot
  int getParts();
  inline std::map<FlatNode *, int> GetFlatNode2PartitionNum(void)
  {
    return FlatNode2PartitionNum;
  } //返回划分个数mnparts
};

void DumpStreamGraph(SchedulerSSG *ssg, Partition *mp, string fileName);

#endif