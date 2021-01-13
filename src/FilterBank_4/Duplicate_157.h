#ifndef _Duplicate_157_
#define _Duplicate_157_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Duplicate_157{
public:
	Duplicate_157(Buffer<streamData>& dup16_0,Buffer<streamData>& dup16_1,Buffer<streamData>& In):dup16_0(dup16_0),dup16_1(dup16_1),In(In){
		steadyScheduleCount = 32;
		initScheduleCount = 127;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		dup16_0.resetTail();
		dup16_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		dup16_0.resetTail();
		dup16_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> dup16_0;
	Producer<streamData> dup16_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		dup16_0.updatetail(1);
		dup16_1.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		dup16_0[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup16_1[i]=In[j++];
		j=0;
	}
		pushToken();
		popToken();
	}
};
#endif