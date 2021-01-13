#ifndef _doE_329_
#define _doE_329_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class doE_329{
public:
	doE_329(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(32);
	}
	void pushToken() {
		Out.updatetail(48);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i;
		for(i=0;i<48;i++)			{
		Out[i].x=In[E[i]-1].x;
	}
	}
		pushToken();
		popToken();
	}
};
#endif