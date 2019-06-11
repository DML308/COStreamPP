#ifndef _ACTORSTAGEASS_H_
#define _ACTORSTAGEASS_H_
#include "6.schedulerSSG.h"
class StageAssignment
{
public:
	void actorStageMap(map<FlatNode *, int> processor2actor); //参数和FlatNode与core之间的映射
	void actorStageMapForGPU(map<FlatNode *,int>processor2actor);

	void actorTopologicalorder(vector<FlatNode *> original);
	int FindStage(FlatNode *);							 //根据结点寻找其所在的阶段号
	int MaxStageNum();											 // 最大阶段号
	vector<FlatNode *> FindActor(int stage); //根据阶段号寻找结点
protected:
	vector<FlatNode *> actortopo;			//用于存储actor的拓扑排序
	map<FlatNode *, int> Actor2Stage; //用于存储阶段赋值的结果
	vector<FlatNode *> ActorSet;
	multimap<int, FlatNode *> Stage2Actor; //用于存储阶段赋值的结果
	/***以下两个变量为GPU代码生成专用，用于记录数据传输所在的阶段***/
	map<FlatNode *,int>DataOfActor2Stage;
	multimap<int,FlatNode*>Stage2DataOfActor;
public:
	int MaxStageNumForGPU();//返回最大的阶段编号
	multimap<int,map<FlatNode*,int>>datastage;//用于标记数据传输是发送还是取数据，方便thread.cpp的生成,该数据结构中第一个int用于记录阶段号，第二个int用于记录标志位,1:取数据  2：发数据
	int GpuNum;
	int setGpuNum(int n){GpuNum =n;}
	vector<FlatNode*> StageAssignment::FindDataOfActor(int stage);
};
#endif