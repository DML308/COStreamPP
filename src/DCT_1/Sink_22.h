#ifndef _Sink_22_
#define _Sink_22_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Sink_22{
public:
	Sink_22(Buffer<streamData>& In):In(In){
		steadyScheduleCount = 8;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		In.resetHead();
	}
private:
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(8);
	}
	void pushToken() {
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i;
		for(i=0;i<8;i++)			{
		cout<<In[i].x;
		cout<<" ";
	}
		cout<<"\n";
	}
		pushToken();
		popToken();
	}
};
#endif