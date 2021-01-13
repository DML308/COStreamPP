#ifndef _LowPassFilterX_59_
#define _LowPassFilterX_59_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class LowPassFilterX_59{
public:
	LowPassFilterX_59(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
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
	float cutoff;
	int decimation;
	float rate;
	int taps;
	float pi;
	float coeff[64];
	float m;
	float w;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		Out.updatetail(1);
	}
	void initVarAndState() {
	cutoff=1244.508057;
	decimation=0;
	rate=250000000.000000;
	taps=64;
		 pi=3.141593;
		 m=taps-1;
		 w=2*pi*cutoff/rate;
	}
	void init(){ 
	{
		int i;
		for(i=0;i<taps;i++)			{
		if(i-m/2==0)	coeff[i]=w/pi;else	coeff[i]=sin(w*(i-m/2))/pi/(i-m/2)*(0.540000-0.460000*cos(2*pi*i/m));
	}
	}	}
	void work(){ 
	{
		int i;
		float sum=0;
		for(i=0;i<taps;i++)		sum+=(In[i].x*coeff[i]);
		Out[0].x=sum;
	}
		pushToken();
		popToken();
	}
};
#endif