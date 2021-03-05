#ifndef _Subtracter_38_
#define _Subtracter_38_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Subtracter_38{
public:
	Subtracter_38(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
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
		In.updatehead(2);
	}
	void pushToken() {
		Out.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		Out[0].x=In[1].x-In[0].x;
	}
		pushToken();
		popToken();
	}
};
#endif