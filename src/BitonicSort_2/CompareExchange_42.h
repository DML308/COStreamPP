#ifndef _CompareExchange_42_
#define _CompareExchange_42_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class CompareExchange_42{
public:
	CompareExchange_42(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
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
	int sortdir;
	void popToken() {
		In.updatehead(2);
	}
	void pushToken() {
		Out.updatetail(2);
	}
	void initVarAndState() {
	sortdir=1;
	}
	void init(){ 
	}
	void work(){ 
	{
		int k1,k2,mink,maxk;
		k1=In[0].x;
		k2=In[1].x;
		if(k1<=k2)		{
		mink=k1;
		maxk=k2;
	}else		{
		mink=k2;
		maxk=k1;
	}
		if(sortdir!=0)		{
		Out[0].x=mink;
		Out[1].x=maxk;
	}else		{
		Out[0].x=maxk;
		Out[1].x=mink;
	}
	}
		pushToken();
		popToken();
	}
};
#endif