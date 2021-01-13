#ifndef _Repeat_36_
#define _Repeat_36_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Repeat_36{
public:
	Repeat_36(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
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
	int numitems;
	int numtimes;
	void popToken() {
		In.updatehead(3);
	}
	void pushToken() {
		Out.updatetail(18);
	}
	void initVarAndState() {
	numitems=3;
	numtimes=6;
	}
	void init(){ 
	}
	void work(){ 
	{
		int dataArray[numitems];
		int i,j,push=0;
		for(i=0;i<numitems;i++)			{
		dataArray[i]=In[i].x;
	}
		for(j=0;j<numtimes;j++)			{
		for(i=0;i<numitems;i++)			{
		Out[push++].x=dataArray[i];
	}
	}
	}
		pushToken();
		popToken();
	}
};
#endif