#ifndef _BitstoInts_773_
#define _BitstoInts_773_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class BitstoInts_773{
public:
	BitstoInts_773(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
		steadyScheduleCount = 16;
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
	int n;
	void popToken() {
		In.updatehead(4);
	}
	void pushToken() {
		Out.updatetail(1);
	}
	void initVarAndState() {
	n=4;
	}
	void init(){ 
	}
	void work(){ 
	{
		int i,v=0;
		for(i=0;i<n;i++)			{
		v=v|(In[i].x<<i);
	}
		Out[0].x=v;
	}
		pushToken();
		popToken();
	}
};
#endif