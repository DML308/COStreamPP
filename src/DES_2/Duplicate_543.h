#ifndef _Duplicate_543_
#define _Duplicate_543_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Duplicate_543{
public:
	Duplicate_543(Buffer<streamData>& dup90_0,Buffer<streamData>& dup90_1,Buffer<streamData>& In):dup90_0(dup90_0),dup90_1(dup90_1),In(In){
		steadyScheduleCount = 64;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		dup90_0.resetTail();
		dup90_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		dup90_0.resetTail();
		dup90_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> dup90_0;
	Producer<streamData> dup90_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		dup90_0.updatetail(1);
		dup90_1.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		dup90_0[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup90_1[i]=In[j++];
		j=0;
	}
		pushToken();
		popToken();
	}
};
#endif