#ifndef _Saturation_11_
#define _Saturation_11_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Saturation_11{
public:
	Saturation_11(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
		steadyScheduleCount = 384;
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
	int max;
	int min;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		Out.updatetail(1);
	}
	void initVarAndState() {
	max=2047;
	min=-2048;
	}
	void init(){ 
	}
	void work(){ 
	{
		int valx=In[0].x;
		if(valx>max)		{
		Out[0].x=max;
	}else	if(valx<min)		{
		Out[0].x=min;
	}else		{
		Out[0].x=valx;
	}
	}
		pushToken();
		popToken();
	}
};
#endif