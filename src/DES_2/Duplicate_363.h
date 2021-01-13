#ifndef _Duplicate_363_
#define _Duplicate_363_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Duplicate_363{
public:
	Duplicate_363(Buffer<streamData>& dup60_0,Buffer<streamData>& dup60_1,Buffer<streamData>& In):dup60_0(dup60_0),dup60_1(dup60_1),In(In){
		steadyScheduleCount = 64;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		dup60_0.resetTail();
		dup60_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		dup60_0.resetTail();
		dup60_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> dup60_0;
	Producer<streamData> dup60_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		dup60_0.updatetail(1);
		dup60_1.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		dup60_0[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup60_1[i]=In[j++];
		j=0;
	}
		pushToken();
		popToken();
	}
};
#endif