#ifndef _Duplicate_3_
#define _Duplicate_3_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Duplicate_3{
public:
	Duplicate_3(Buffer<streamData>& dup0_0,Buffer<streamData>& dup0_1,Buffer<streamData>& In):dup0_0(dup0_0),dup0_1(dup0_1),In(In){
		steadyScheduleCount = 64;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		dup0_0.resetTail();
		dup0_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		dup0_0.resetTail();
		dup0_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> dup0_0;
	Producer<streamData> dup0_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		dup0_0.updatetail(1);
		dup0_1.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		dup0_0[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_1[i]=In[j++];
		j=0;
	}
		pushToken();
		popToken();
	}
};
#endif