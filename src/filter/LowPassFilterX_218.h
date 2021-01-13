#ifndef _LowPassFilterX_218_
#define _LowPassFilterX_218_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class LowPassFilterX_218{
public:
	LowPassFilterX_218(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
		steadyScheduleCount = 32;
		initScheduleCount = 255;
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
	int N;
	double cutoffFreq;
	double g;
	double h[128];
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		Out.updatetail(1);
	}
	void initVarAndState() {
	N=128;
	cutoffFreq=2.454369;
	g=1;
	}
	void init(){ 
	{
		int OFFSET=N/2;
		int i,idx;
		for(i=0;i<N;i++)			{
		idx=i+1;
		if(idx==OFFSET)	h[i]=g*cutoffFreq/pi;else	h[i]=g*sin(cutoffFreq*(idx-OFFSET))/(pi*(idx-OFFSET));
	}
	}	}
	void work(){ 
	{
		double sum=0;
		int i;
		for(i=0;i<N;i++)			{
		sum+=h[i]*In[i].x;
	}
		Out[0].x=sum;
	}
		pushToken();
		popToken();
	}
};
#endif