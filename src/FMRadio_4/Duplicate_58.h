#ifndef _Duplicate_58_
#define _Duplicate_58_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Duplicate_58{
public:
	Duplicate_58(Buffer<streamData>& dup10_0,Buffer<streamData>& dup10_1,Buffer<streamData>& In):dup10_0(dup10_0),dup10_1(dup10_1),In(In){
		steadyScheduleCount = 1;
		initScheduleCount = 63;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		dup10_0.resetTail();
		dup10_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		dup10_0.resetTail();
		dup10_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> dup10_0;
	Producer<streamData> dup10_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		dup10_0.updatetail(1);
		dup10_1.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		dup10_0[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup10_1[i]=In[j++];
		j=0;
	}
		pushToken();
		popToken();
	}
};
#endif