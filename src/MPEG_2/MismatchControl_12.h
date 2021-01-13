#ifndef _MismatchControl_12_
#define _MismatchControl_12_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class MismatchControl_12{
public:
	MismatchControl_12(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
		steadyScheduleCount = 6;
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
	void popToken() {
		In.updatehead(64);
	}
	void pushToken() {
		Out.updatetail(64);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int sum,valx,i;
		sum=0;
		for(i=0;i<63;i++)			{
		valx=In[i].x;
		sum+=valx;
		Out[i].x=valx;
	}
		valx=In[i].x;
		sum+=valx;
		if((sum&1)==1)		{
		Out[63].x=valx;
	}else		{
		if((valx*1)==1)		{
		Out[63].x=valx-1;
	}else		{
		Out[63].x=valx+1;
	}
	}
	}
		pushToken();
		popToken();
	}
};
#endif