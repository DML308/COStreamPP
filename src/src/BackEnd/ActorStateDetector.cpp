#include "ActorStateDetector.h"
#include <iostream>
using namespace std;

void ActorStateDetector::hasMutableState()
{  
	//是state则返回True
	//新前端已经增加了对operter中有无状态的判断，合并后取消注释该函数即可使用
	/*
    if(opNode->hasState)
		stateful =  true;
	else
		stateful =  false;
	*/
}

bool DetectiveActorState(FlatNode *flatNode)
{
	ActorStateDetector* detector = new ActorStateDetector(flatNode->contents);
	detector->hasMutableState();
	return detector->GetOperatorState();
}

/*检查无状态节点父子节点状态情况*/
 int UporDownStatelessNode(FlatNode *node)
{
	int marks = 0, marks1 = 0, marks2 = 0;
	vector<FlatNode*>::iterator iter1;
	for (iter1 = node->inFlatNodes.begin(); iter1 != node->inFlatNodes.end(); ++iter1)
	{
		if (!DetectiveActorState(*iter1))
		{
			marks1 = 0;
			break;
		}
		else
			marks1 = 1;
	}
	for (iter1 = node->outFlatNodes.begin(); iter1 != node->outFlatNodes.end(); ++iter1)
	{
		if (!DetectiveActorState(*iter1))
		{
			marks2 = 0;
			break;
		}
		else
			marks2 = 2;
	}
	if (marks1 != 0 && marks2 == 0)marks = marks1;
	else if (marks1 == 0 && marks2 != 0)
	{
		marks = marks2;
	}
	else if (marks1 != 0 && marks2 != 0)
	{
		marks = 3;
	}
	return marks;
}