
#ifndef _SCHEDULER_SSG_H_
#define _SCHEDULER_SSG_H_
#include "staticStreamGragh.h"
class SchedulerSSG : public StaticStreamGraph
{
  public:
	SchedulerSSG(StaticStreamGraph *ssg);
	int lcm(int a, int b); // 求a,b的最小公倍数
	int gcd(int a, int b); // 求a,b的最大公约数
	bool InitScheduling();
	bool SteadyScheduling();
	int GetInitCount(FlatNode *node);
	int GetSteadyCount(FlatNode *node);
	map<FlatNode *, int> mapFlatNode2InitCount;   // SDF图所有节点初始化调度序列
	map<FlatNode *, int> mapFlatNode2SteadyCount; // SDF图所有节点稳定状态调度序列<节点，执行次数>
	double total_work;							  //SDF总的工作量
	double edge_work;							  // SDF总的边大小
};

SchedulerSSG *SchedulingSSG(StaticStreamGraph *ssg);

#endif
