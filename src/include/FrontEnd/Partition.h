#include "schedulerSSG.h"
class Partition
{
  public:
    //所有调度方法均会使用到的成员变量
    std::map<FlatNode *, int> FlatNode2PartitionNum;     //节点到划分编号的映射
    std::multimap<int, FlatNode *> PartitonNum2FlatNode; //划分编号到节点的映射
    std::vector<FlatNode *> PartitonNumSet;              //用于记录返回的节点集合
    int mnparts;                                         //划分的份数，依赖于指定的places个数

  public:
    Partition();
    virtual void SssgPartition(SchedulerSSG *sssg, int level) = 0; //划分成员方法，具体实现由子类实现
    int findID(SchedulerSSG *sssg, FlatNode *flatnode);            //根据flatnode找到其下标号 如source_0中的0
    void setCpuCoreNum(int, SchedulerSSG *);                       //设置place数目（即进程数目）
    int findPartitionNumForFlatNode(FlatNode *flatnode);//根据节点返回其所在划分区的编号(节点->编号) for dot
    int getParts();                                                //返回划分个数mnparts
};

void DumpStreamGraph(SchedulerSSG *ssg,Partition *mp,string fileName);