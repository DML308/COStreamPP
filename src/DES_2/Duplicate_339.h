#ifndef _Duplicate_339_
#define _Duplicate_339_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Duplicate_339{
public:
	Duplicate_339(Buffer<streamData>& dup56_0,Buffer<streamData>& dup56_1,Buffer<streamData>& In):dup56_0(dup56_0),dup56_1(dup56_1),In(In){
		steadyScheduleCount = 64;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		dup56_0.resetTail();
		dup56_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		dup56_0.resetTail();
		dup56_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> dup56_0;
	Producer<streamData> dup56_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		dup56_0.updatetail(1);
		dup56_1.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		dup56_0[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup56_1[i]=In[j++];
		j=0;
	}
		pushToken();
		popToken();
	}
};
#endif