#ifndef _Duplicate_555_
#define _Duplicate_555_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Duplicate_555{
public:
	Duplicate_555(Buffer<streamData>& dup92_0,Buffer<streamData>& dup92_1,Buffer<streamData>& In):dup92_0(dup92_0),dup92_1(dup92_1),In(In){
		steadyScheduleCount = 64;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		dup92_0.resetTail();
		dup92_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		dup92_0.resetTail();
		dup92_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> dup92_0;
	Producer<streamData> dup92_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		dup92_0.updatetail(1);
		dup92_1.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		dup92_0[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup92_1[i]=In[j++];
		j=0;
	}
		pushToken();
		popToken();
	}
};
#endif