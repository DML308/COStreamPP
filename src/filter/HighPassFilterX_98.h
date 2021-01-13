#ifndef _HighPassFilterX_98_
#define _HighPassFilterX_98_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class HighPassFilterX_98{
public:
	HighPassFilterX_98(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
		steadyScheduleCount = 32;
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
	int N;
	double g;
	double ws;
	double h[128];
	double cutoffFreq;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		Out.updatetail(1);
	}
	void initVarAndState() {
	N=128;
	g=32;
	ws=1.079922;
	}
	void init(){ 
	{
		int OFFSET=N/2;
		int i,idx,sign;
		cutoffFreq=pi-ws;
		for(i=0;i<N;i++)			{
		idx=i+1;
		if((i%2)==0)		{
		sign=1;
	}else		{
		sign=-1;
	}
		if(idx==OFFSET)	h[i]=sign*g*cutoffFreq/pi;else	h[i]=sign*g*sin(cutoffFreq*(idx-OFFSET))/(pi*(idx-OFFSET));
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