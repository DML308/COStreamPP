#ifndef _BoundedSaturation_33_
#define _BoundedSaturation_33_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class BoundedSaturation_33{
public:
	BoundedSaturation_33(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
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
	int worst_input_max;
	int worst_input_min;
	int range;
	int n;
	int saturate[1042];
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		Out.updatetail(1);
	}
	void initVarAndState() {
	max=255;
	min=-256;
	worst_input_max=260;
	worst_input_min=-260;
		 n=2*(worst_input_max-worst_input_min+1);
	}
	void init(){ 
	{
		int i;
		range=worst_input_max-worst_input_min+1;
		for(i=0;i<range*2;i++)			{
		int valx=i+worst_input_min;
		if(valx<min)		{
		saturate[i]=min;
	}else	if(valx>max)		{
		saturate[i]=max;
	}else		{
		saturate[i]=valx;
	}
	}
	}	}
	void work(){ 
	{
		Out[0].x=saturate[In[0].x-worst_input_min];
	}
		pushToken();
		popToken();
	}
};
#endif