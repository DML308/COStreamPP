#ifndef _Duplicate_197_
#define _Duplicate_197_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Duplicate_197{
public:
	Duplicate_197(Buffer<streamData>& dup20_0,Buffer<streamData>& dup20_1,Buffer<streamData>& In):dup20_0(dup20_0),dup20_1(dup20_1),In(In){
		steadyScheduleCount = 32;
		initScheduleCount = 127;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		dup20_0.resetTail();
		dup20_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		dup20_0.resetTail();
		dup20_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> dup20_0;
	Producer<streamData> dup20_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		dup20_0.updatetail(1);
		dup20_1.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		dup20_0[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup20_1[i]=In[j++];
		j=0;
	}
		pushToken();
		popToken();
	}
};
#endif