#ifndef _BeamFirFilter_49_
#define _BeamFirFilter_49_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class BeamFirFilter_49{
public:
	BeamFirFilter_49(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
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
	int decimationRatio;
	int inputLength;
	int numTaps;
	double real_weight[128];
	double imag_weight[128];
	int numTapsMinusOne;
	double realBuffer[128];
	double imagBuffer[128];
	int Count;
	int pos;
	void popToken() {
		In.updatehead(2);
	}
	void pushToken() {
		Out.updatetail(2);
	}
	void initVarAndState() {
	decimationRatio=1;
	inputLength=128;
	numTaps=128;
	}
	void init(){ 
	{
		int i,j,idx;
		numTapsMinusOne=numTaps-1;
		pos=0;
		for(j=0;j<numTaps;j++)			{
		idx=j+1;
		real_weight[j]=sin(idx)/((double)idx);
		imag_weight[j]=cos(idx)/((double)idx);
	}
		for(i=0;i<numTaps;i++)			{
		realBuffer[i]=0;
		imagBuffer[i]=0;
	}
	}	}
	void work(){ 
	{
		double real_curr=0;
		double imag_curr=0;
		int i;
		int modPos;
		realBuffer[numTapsMinusOne-pos]=In[0].x;
		imagBuffer[numTapsMinusOne-pos]=In[1].x;
		modPos=numTapsMinusOne-pos;
		for(i=0;i<numTaps;i++)			{
		real_curr+=realBuffer[modPos]*real_weight[i]+imagBuffer[modPos]*imag_weight[i];
		imag_curr+=imagBuffer[modPos]*real_weight[i]+realBuffer[modPos]*imag_weight[i];
		modPos=(modPos+1)&numTapsMinusOne;
	}
		pos=(pos+1)&numTapsMinusOne;
		Out[0].x=real_curr;
		Out[1].x=imag_curr;
		Count+=decimationRatio;
		if(Count==inputLength)		{
		Count=0;
		pos=0;
		for(i=0;i<numTaps;i++)			{
		realBuffer[i]=0;
		imagBuffer[i]=0;
	}
	}
	}
		pushToken();
		popToken();
	}
};
#endif