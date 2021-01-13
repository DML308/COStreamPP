#ifndef _Duplicate_231_
#define _Duplicate_231_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Duplicate_231{
public:
	Duplicate_231(Buffer<streamData>& dup38_0,Buffer<streamData>& dup38_1,Buffer<streamData>& In):dup38_0(dup38_0),dup38_1(dup38_1),In(In){
		steadyScheduleCount = 64;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		dup38_0.resetTail();
		dup38_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		dup38_0.resetTail();
		dup38_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> dup38_0;
	Producer<streamData> dup38_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		dup38_0.updatetail(1);
		dup38_1.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		dup38_0[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup38_1[i]=In[j++];
		j=0;
	}
		pushToken();
		popToken();
	}
};
#endif