#ifndef _ACTORSTAGEASS_H_
#define _ACTORSTAGEASS_H_
#include "6.schedulerSSG.h"
class StageAssignment
{
public:
	void actorStageMap(map<FlatNode *, int> processor2actor); //参数和FlatNode与core之间的映射
	void actorTopologicalorder(vector<FlatNode *> original);
	int FindStage(FlatNode *);							 //根据结点寻找其所在的阶段号
	int MaxStageNum();											 // 最大阶段号
	vector<FlatNode *> FindActor(int stage); //根据阶段号寻找结点
protected:
	vector<FlatNode *> actortopo;			//用于存储actor的拓扑排序
	map<FlatNode *, int> Actor2Stage; //用于存储阶段赋值的结果
	vector<FlatNode *> ActorSet;
	multimap<int, FlatNode *> Stage2Actor; //用于存储阶段赋值的结果
};
#endif