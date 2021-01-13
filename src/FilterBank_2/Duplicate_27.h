#ifndef _Duplicate_27_
#define _Duplicate_27_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Duplicate_27{
public:
	Duplicate_27(Buffer<streamData>& dup3_0,Buffer<streamData>& dup3_1,Buffer<streamData>& In):dup3_0(dup3_0),dup3_1(dup3_1),In(In){
		steadyScheduleCount = 32;
		initScheduleCount = 127;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		dup3_0.resetTail();
		dup3_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		dup3_0.resetTail();
		dup3_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> dup3_0;
	Producer<streamData> dup3_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		dup3_0.updatetail(1);
		dup3_1.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		dup3_0[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup3_1[i]=In[j++];
		j=0;
	}
		pushToken();
		popToken();
	}
};
#endif