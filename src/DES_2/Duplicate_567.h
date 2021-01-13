#ifndef _Duplicate_567_
#define _Duplicate_567_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Duplicate_567{
public:
	Duplicate_567(Buffer<streamData>& dup94_0,Buffer<streamData>& dup94_1,Buffer<streamData>& In):dup94_0(dup94_0),dup94_1(dup94_1),In(In){
		steadyScheduleCount = 64;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		dup94_0.resetTail();
		dup94_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		dup94_0.resetTail();
		dup94_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> dup94_0;
	Producer<streamData> dup94_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		dup94_0.updatetail(1);
		dup94_1.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		dup94_0[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup94_1[i]=In[j++];
		j=0;
	}
		pushToken();
		popToken();
	}
};
#endif