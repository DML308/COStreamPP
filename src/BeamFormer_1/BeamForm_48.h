#ifndef _BeamForm_48_
#define _BeamForm_48_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class BeamForm_48{
public:
	BeamForm_48(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
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
	int myBeamId;
	int numChannels;
	double real_weight[12];
	double imag_weight[12];
	void popToken() {
		In.updatehead(24);
	}
	void pushToken() {
		Out.updatetail(2);
	}
	void initVarAndState() {
	myBeamId=2;
	numChannels=12;
	}
	void init(){ 
	{
		int idx,j;
		for(j=0;j<numChannels;j++)			{
		idx=j+1;
		real_weight[j]=sin(idx)/((double)(myBeamId+idx));
		imag_weight[j]=cos(idx)/((double)(myBeamId+idx));
	}
	}	}
	void work(){ 
	{
		double real_curr=0;
		double imag_curr=0;
		int i;
		for(i=0;i<numChannels;i++)			{
		double real_pop=In[2*i].x;
		double imag_pop=In[2*i+1].x;
		real_curr+=real_weight[i]*real_pop-imag_weight[i]*imag_pop;
		imag_curr+=real_weight[i]*imag_pop+imag_weight[i]*real_pop;
	}
		Out[0].x=real_curr;
		Out[1].x=imag_curr;
	}
		pushToken();
		popToken();
	}
};
#endif