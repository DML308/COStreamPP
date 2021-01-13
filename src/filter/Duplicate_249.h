#ifndef _Duplicate_249_
#define _Duplicate_249_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Duplicate_249{
public:
	Duplicate_249(Buffer<streamData>& dup28_0,Buffer<streamData>& dup28_1,Buffer<streamData>& In):dup28_0(dup28_0),dup28_1(dup28_1),In(In){
		steadyScheduleCount = 32;
		initScheduleCount = 127;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		dup28_0.resetTail();
		dup28_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		dup28_0.resetTail();
		dup28_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> dup28_0;
	Producer<streamData> dup28_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		dup28_0.updatetail(1);
		dup28_1.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		dup28_0[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup28_1[i]=In[j++];
		j=0;
	}
		pushToken();
		popToken();
	}
};
#endif