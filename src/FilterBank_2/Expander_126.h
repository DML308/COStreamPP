#ifndef _Expander_126_
#define _Expander_126_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Expander_126{
public:
	Expander_126(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
		steadyScheduleCount = 1;
		initScheduleCount = 4;
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
	int M;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		Out.updatetail(32);
	}
	void initVarAndState() {
	M=32;
	}
	void init(){ 
	}
	void work(){ 
	{
		int i;
		Out[0].x=In[0].x;
		for(i=1;i<M;i++)			{
		Out[i].x=0;
	}
	}
		pushToken();
		popToken();
	}
};
#endif