#ifndef _Repeat_35_
#define _Repeat_35_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Repeat_35{
public:
	Repeat_35(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
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
		In.updatehead(8);
	}
	void pushToken() {
		Out.updatetail(48);
	}
	void initVarAndState() {
	numitems=8;
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