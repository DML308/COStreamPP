#ifndef _Duplicate_603_
#define _Duplicate_603_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Duplicate_603{
public:
	Duplicate_603(Buffer<streamData>& dup100_0,Buffer<streamData>& dup100_1,Buffer<streamData>& In):dup100_0(dup100_0),dup100_1(dup100_1),In(In){
		steadyScheduleCount = 64;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		dup100_0.resetTail();
		dup100_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		dup100_0.resetTail();
		dup100_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> dup100_0;
	Producer<streamData> dup100_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		dup100_0.updatetail(1);
		dup100_1.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		dup100_0[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup100_1[i]=In[j++];
		j=0;
	}
		pushToken();
		popToken();
	}
};
#endif