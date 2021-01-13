#ifndef _IntSource_0_
#define _IntSource_0_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class IntSource_0{
public:
	IntSource_0(Buffer<streamData>& Out):Out(Out){
		steadyScheduleCount = 64;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
	}
private:
	Producer<streamData> Out;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	int i;
	void popToken() {
	}
	void pushToken() {
		Out.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	{
		i=0;
	}	}
	void work(){ 
	{
		Out[0].x=(double)From[i++];
	}
		pushToken();
		popToken();
	}
};
#endif