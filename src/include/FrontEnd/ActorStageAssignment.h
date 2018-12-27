#ifndef _ACTORSTAGEASS_H_
#define _ACTORSTAGEASS_H_
#include "schedulerSSG.h"
class StageAssignment
{
public:
	void actorStageMap(map<FlatNode *,int>processor2actor);//参数和FlatNode与core之间的映射
	void actorTopologicalorder(vector<FlatNode *>original);
protected:
	vector<FlatNode *>actortopo;//用于存储actor的拓扑排序
	map<FlatNode *,int>Actor2Stage;//用于存储阶段赋值的结果
	multimap<int,FlatNode*>Stage2Actor;//用于存储阶段赋值的结果
private:
};
#endif