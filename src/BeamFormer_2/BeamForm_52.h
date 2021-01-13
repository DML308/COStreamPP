#ifndef _BeamForm_52_
#define _BeamForm_52_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class BeamForm_52{
public:
	BeamForm_52(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
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

	timespec diff(timespec start,timespec end)
	{
		timespec temp;
		if((end.tv_nsec-start.tv_nsec)<0)
		{
			temp.tv_sec=end.tv_sec-start.tv_sec-1;
			temp.tv_nsec=1000000000+end.tv_nsec-start.tv_nsec;
		}
		else
		{
			temp.tv_sec=end.tv_sec-start.tv_sec;
			temp.tv_nsec=end.tv_nsec-start.tv_nsec;
		}
		return temp;
		
	}

	void runSteadyScheduleWork() {
		timespec time1,time2;
		
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		clock_gettime(CLOCK_REALTIME,&time1);
		Out.resetTail();
		In.resetHead();
		clock_gettime(CLOCK_REALTIME,&time2);
		timespec tmp=diff(time1,time2);
		worktime1.tv_nsec+=tmp.tv_nsec;
		int sec=worktime1.tv_nsec/1000000000;
		if(sec>0)
		{
			worktime1.tv_sec+=tmp.tv_sec+sec;
			worktime1.tv_nsec+=tmp.tv_nsec%1000000000;
		}
		else
		{
			worktime1.tv_sec+=tmp.tv_sec;
		}
		
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
	myBeamId=3;
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