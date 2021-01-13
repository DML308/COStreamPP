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
	Duplicate_3(Buffer<streamData>& dup0_0,Buffer<streamData>& dup0_1,Buffer<streamData>& dup0_2,Buffer<streamData>& dup0_3,Buffer<streamData>& dup0_4,Buffer<streamData>& dup0_5,Buffer<streamData>& dup0_6,Buffer<streamData>& dup0_7,Buffer<streamData>& dup0_8,Buffer<streamData>& dup0_9,Buffer<streamData>& In):dup0_0(dup0_0),dup0_1(dup0_1),dup0_2(dup0_2),dup0_3(dup0_3),dup0_4(dup0_4),dup0_5(dup0_5),dup0_6(dup0_6),dup0_7(dup0_7),dup0_8(dup0_8),dup0_9(dup0_9),In(In){
		steadyScheduleCount = 1;
		initScheduleCount = 63;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		dup0_0.resetTail();
		dup0_1.resetTail();
		dup0_2.resetTail();
		dup0_3.resetTail();
		dup0_4.resetTail();
		dup0_5.resetTail();
		dup0_6.resetTail();
		dup0_7.resetTail();
		dup0_8.resetTail();
		dup0_9.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		dup0_0.resetTail();
		dup0_1.resetTail();
		dup0_2.resetTail();
		dup0_3.resetTail();
		dup0_4.resetTail();
		dup0_5.resetTail();
		dup0_6.resetTail();
		dup0_7.resetTail();
		dup0_8.resetTail();
		dup0_9.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> dup0_0;
	Producer<streamData> dup0_1;
	Producer<streamData> dup0_2;
	Producer<streamData> dup0_3;
	Producer<streamData> dup0_4;
	Producer<streamData> dup0_5;
	Producer<streamData> dup0_6;
	Producer<streamData> dup0_7;
	Producer<streamData> dup0_8;
	Producer<streamData> dup0_9;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		dup0_0.updatetail(1);
		dup0_1.updatetail(1);
		dup0_2.updatetail(1);
		dup0_3.updatetail(1);
		dup0_4.updatetail(1);
		dup0_5.updatetail(1);
		dup0_6.updatetail(1);
		dup0_7.updatetail(1);
		dup0_8.updatetail(1);
		dup0_9.updatetail(1);
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
		for(i=0;i<1;++i)		dup0_2[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_3[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_4[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_5[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_6[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_7[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_8[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_9[i]=In[j++];
		j=0;
	}
		pushToken();
		popToken();
	}
};
#endif