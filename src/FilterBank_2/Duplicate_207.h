#ifndef _Duplicate_207_
#define _Duplicate_207_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Duplicate_207{
public:
	Duplicate_207(Buffer<streamData>& dup21_0,Buffer<streamData>& dup21_1,Buffer<streamData>& In):dup21_0(dup21_0),dup21_1(dup21_1),In(In){
		steadyScheduleCount = 32;
		initScheduleCount = 127;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		dup21_0.resetTail();
		dup21_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		dup21_0.resetTail();
		dup21_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> dup21_0;
	Producer<streamData> dup21_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		dup21_0.updatetail(1);
		dup21_1.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		dup21_0[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup21_1[i]=In[j++];
		j=0;
	}
		pushToken();
		popToken();
	}
};
#endif