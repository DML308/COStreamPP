#ifndef _Duplicate_39_
#define _Duplicate_39_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Duplicate_39{
public:
	Duplicate_39(Buffer<streamData>& dup1_0,Buffer<streamData>& dup1_1,Buffer<streamData>& dup1_2,Buffer<streamData>& dup1_3,Buffer<streamData>& In):dup1_0(dup1_0),dup1_1(dup1_1),dup1_2(dup1_2),dup1_3(dup1_3),In(In){
		steadyScheduleCount = 24;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		dup1_0.resetTail();
		dup1_1.resetTail();
		dup1_2.resetTail();
		dup1_3.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		dup1_0.resetTail();
		dup1_1.resetTail();
		dup1_2.resetTail();
		dup1_3.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> dup1_0;
	Producer<streamData> dup1_1;
	Producer<streamData> dup1_2;
	Producer<streamData> dup1_3;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		dup1_0.updatetail(1);
		dup1_1.updatetail(1);
		dup1_2.updatetail(1);
		dup1_3.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		dup1_0[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup1_1[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup1_2[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup1_3[i]=In[j++];
		j=0;
	}
		pushToken();
		popToken();
	}
};
#endif