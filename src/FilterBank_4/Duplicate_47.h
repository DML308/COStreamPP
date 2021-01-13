#ifndef _Duplicate_47_
#define _Duplicate_47_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Duplicate_47{
public:
	Duplicate_47(Buffer<streamData>& dup5_0,Buffer<streamData>& dup5_1,Buffer<streamData>& In):dup5_0(dup5_0),dup5_1(dup5_1),In(In){
		steadyScheduleCount = 32;
		initScheduleCount = 127;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		dup5_0.resetTail();
		dup5_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		dup5_0.resetTail();
		dup5_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> dup5_0;
	Producer<streamData> dup5_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		dup5_0.updatetail(1);
		dup5_1.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		dup5_0[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup5_1[i]=In[j++];
		j=0;
	}
		pushToken();
		popToken();
	}
};
#endif