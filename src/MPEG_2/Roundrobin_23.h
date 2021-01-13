#ifndef _Roundrobin_23_
#define _Roundrobin_23_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Roundrobin_23{
public:
	Roundrobin_23(Buffer<streamData>& round4_0,Buffer<streamData>& round4_1,Buffer<streamData>& round4_2,Buffer<streamData>& round4_3,Buffer<streamData>& round4_4,Buffer<streamData>& round4_5,Buffer<streamData>& round4_6,Buffer<streamData>& round4_7,Buffer<streamData>& In):round4_0(round4_0),round4_1(round4_1),round4_2(round4_2),round4_3(round4_3),round4_4(round4_4),round4_5(round4_5),round4_6(round4_6),round4_7(round4_7),In(In){
		steadyScheduleCount = 48;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		round4_0.resetTail();
		round4_1.resetTail();
		round4_2.resetTail();
		round4_3.resetTail();
		round4_4.resetTail();
		round4_5.resetTail();
		round4_6.resetTail();
		round4_7.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		round4_0.resetTail();
		round4_1.resetTail();
		round4_2.resetTail();
		round4_3.resetTail();
		round4_4.resetTail();
		round4_5.resetTail();
		round4_6.resetTail();
		round4_7.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> round4_0;
	Producer<streamData> round4_1;
	Producer<streamData> round4_2;
	Producer<streamData> round4_3;
	Producer<streamData> round4_4;
	Producer<streamData> round4_5;
	Producer<streamData> round4_6;
	Producer<streamData> round4_7;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(8);
	}
	void pushToken() {
		round4_0.updatetail(1);
		round4_1.updatetail(1);
		round4_2.updatetail(1);
		round4_3.updatetail(1);
		round4_4.updatetail(1);
		round4_5.updatetail(1);
		round4_6.updatetail(1);
		round4_7.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		round4_0[i]=In[j++];
		for(i=0;i<1;++i)		round4_1[i]=In[j++];
		for(i=0;i<1;++i)		round4_2[i]=In[j++];
		for(i=0;i<1;++i)		round4_3[i]=In[j++];
		for(i=0;i<1;++i)		round4_4[i]=In[j++];
		for(i=0;i<1;++i)		round4_5[i]=In[j++];
		for(i=0;i<1;++i)		round4_6[i]=In[j++];
		for(i=0;i<1;++i)		round4_7[i]=In[j++];
	}
		pushToken();
		popToken();
	}
};
#endif