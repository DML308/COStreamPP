
#ifndef _SCHEDULER_SSG_H_
#define _SCHEDULER_SSG_H_
#include "staticStreamGragh.h"
//#include "Partition.h"
class SchedulerSSG : public StaticStreamGraph
{
public:
	SchedulerSSG(StaticStreamGraph *ssg);
	int lcm(int a, int b); // 求a,b的最小公倍数
	int gcd(int a, int b); // 求a,b的最大公约数
	bool InitScheduling();
	bool SteadyScheduling();
	map<FlatNode *, int> SteadySchedulingGroup(vector<FlatNode *>);//对vector中的点集进行局部稳态调度
	int GetInitCount(FlatNode *node);
	int GetSteadyCount(FlatNode *node);
	map<FlatNode *, int> mapInitCount2FlatNode; // SDF图所有节点初始化调度序列
	map<FlatNode *, int> mapSteadyCount2FlatNode; // SDF图所有节点稳定状态调度序列<节点，执行次数>
	double total_work;//SDF总的工作量
	double edge_work;// add by wangliang SDF总的边大小 
};

SchedulerSSG *SchedulingSSG(StaticStreamGraph *ssg);


#endif
