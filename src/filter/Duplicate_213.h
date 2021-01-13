#ifndef _Duplicate_213_
#define _Duplicate_213_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Duplicate_213{
public:
	Duplicate_213(Buffer<streamData>& dup24_0,Buffer<streamData>& dup24_1,Buffer<streamData>& In):dup24_0(dup24_0),dup24_1(dup24_1),In(In){
		steadyScheduleCount = 32;
		initScheduleCount = 127;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		dup24_0.resetTail();
		dup24_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		dup24_0.resetTail();
		dup24_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> dup24_0;
	Producer<streamData> dup24_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		dup24_0.updatetail(1);
		dup24_1.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		dup24_0[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup24_1[i]=In[j++];
		j=0;
	}
		pushToken();
		popToken();
	}
};
#endif