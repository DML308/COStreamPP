#ifndef _FFTReorderSimpleX_2_
#define _FFTReorderSimpleX_2_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class FFTReorderSimpleX_2{
public:
	FFTReorderSimpleX_2(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
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
	int n;
	void popToken() {
		In.updatehead(16);
	}
	void pushToken() {
		Out.updatetail(16);
	}
	void initVarAndState() {
	n=8;
	}
	void init(){ 
	}
	void work(){ 
	{
		int i,j,k;
		int m=0;
		int final[n];
		for(i=0;i<n;i++)		final[i]=0;
		for(i=0;i<n;i++)			{
		int c=i;
		for(j=0;j<log(n);j++)			{
		final[i]<<=1;
		final[i]|=(c&1);
		c>>=1;
	}
	}
		for(i=0;i<n;i++)			{
		Out[final[i]*2].x=In[i*2].x;
		Out[final[i]*2+1].x=In[i*2+1].x;
	}
	}
		pushToken();
		popToken();
	}
};
#endif