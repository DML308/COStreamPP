#ifndef _AmplifyX_45_
#define _AmplifyX_45_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class AmplifyX_45{
public:
	AmplifyX_45(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
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
	float k;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		Out.updatetail(1);
	}
	void initVarAndState() {
	k=1.700000;
	}
	void init(){ 
	}
	void work(){ 
	{
		Out[0].x=k*In[0].x;
	}
		pushToken();
		popToken();
	}
};
#endif