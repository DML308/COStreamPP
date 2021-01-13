#ifndef _AnnoyX_65_
#define _AnnoyX_65_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class AnnoyX_65{
public:
	AnnoyX_65(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
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
	int bands;
	void popToken() {
		In.updatehead(10);
	}
	void pushToken() {
		Out.updatetail(1);
	}
	void initVarAndState() {
	bands=11;
	}
	void init(){ 
	}
	void work(){ 
	{
		float sum=0;
		int i;
		for(i=0;i<bands-1;i++)		sum+=In[i].x;
		Out[0].x=sum;
	}
		pushToken();
		popToken();
	}
};
#endif