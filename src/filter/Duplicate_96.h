#ifndef _Duplicate_96_
#define _Duplicate_96_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Duplicate_96{
public:
	Duplicate_96(Buffer<streamData>& dup11_0,Buffer<streamData>& dup11_1,Buffer<streamData>& In):dup11_0(dup11_0),dup11_1(dup11_1),In(In){
		steadyScheduleCount = 32;
		initScheduleCount = 127;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		dup11_0.resetTail();
		dup11_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		dup11_0.resetTail();
		dup11_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> dup11_0;
	Producer<streamData> dup11_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		dup11_0.updatetail(1);
		dup11_1.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		dup11_0[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup11_1[i]=In[j++];
		j=0;
	}
		pushToken();
		popToken();
	}
};
#endif