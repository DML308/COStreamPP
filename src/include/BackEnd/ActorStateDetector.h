#ifndef _FILTER_STATE_DETECTOR_H
#define _FILTER_STATE_DETECTOR_H

#include "../FrontEnd/flatNode.h"

class ActorStateDetector
{
public:
	ActorStateDetector(operatorNode *op){ opNode = op;stateful = false;};
	/*判断actor是否是stateful*/
	void hasMutableState();	
	/*判断一个变量是否是mutable变量*/


	bool GetOperatorState(){return stateful;}
private:
	operatorNode *opNode;
	bool stateful;
};

bool DetectiveActorState(FlatNode *flatNode);
int UporDownStatelessNode(FlatNode *node);
#endif