#ifndef _FMDemodulatorX_2_
#define _FMDemodulatorX_2_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class FMDemodulatorX_2{
public:
	FMDemodulatorX_2(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
		steadyScheduleCount = 1;
		initScheduleCount = 63;
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
	float bandwidth;
	float max;
	float sampRate;
	float mGain;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		Out.updatetail(1);
	}
	void initVarAndState() {
	bandwidth=10000.000000;
	max=27000.000000;
	sampRate=250000000.000000;
	}
	void init(){ 
	{
		float pi=3.141593;
		mGain=max*(sampRate/(bandwidth*pi));
	}	}
	void work(){ 
	{
		float temp=0;
		temp=(float)(In[0].x*In[1].x);
		temp=(float)(mGain*atan(temp));
		Out[0].x=temp;
	}
		pushToken();
		popToken();
	}
};
#endif