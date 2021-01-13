#ifndef _Roundrobin_11_
#define _Roundrobin_11_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Roundrobin_11{
public:
	Roundrobin_11(Buffer<streamData>& round1_0,Buffer<streamData>& round1_1,Buffer<streamData>& round1_2,Buffer<streamData>& round1_3,Buffer<streamData>& round1_4,Buffer<streamData>& round1_5,Buffer<streamData>& round1_6,Buffer<streamData>& round1_7,Buffer<streamData>& In):round1_0(round1_0),round1_1(round1_1),round1_2(round1_2),round1_3(round1_3),round1_4(round1_4),round1_5(round1_5),round1_6(round1_6),round1_7(round1_7),In(In){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		round1_0.resetTail();
		round1_1.resetTail();
		round1_2.resetTail();
		round1_3.resetTail();
		round1_4.resetTail();
		round1_5.resetTail();
		round1_6.resetTail();
		round1_7.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		round1_0.resetTail();
		round1_1.resetTail();
		round1_2.resetTail();
		round1_3.resetTail();
		round1_4.resetTail();
		round1_5.resetTail();
		round1_6.resetTail();
		round1_7.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> round1_0;
	Producer<streamData> round1_1;
	Producer<streamData> round1_2;
	Producer<streamData> round1_3;
	Producer<streamData> round1_4;
	Producer<streamData> round1_5;
	Producer<streamData> round1_6;
	Producer<streamData> round1_7;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(64);
	}
	void pushToken() {
		round1_0.updatetail(8);
		round1_1.updatetail(8);
		round1_2.updatetail(8);
		round1_3.updatetail(8);
		round1_4.updatetail(8);
		round1_5.updatetail(8);
		round1_6.updatetail(8);
		round1_7.updatetail(8);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<8;++i)		round1_0[i]=In[j++];
		for(i=0;i<8;++i)		round1_1[i]=In[j++];
		for(i=0;i<8;++i)		round1_2[i]=In[j++];
		for(i=0;i<8;++i)		round1_3[i]=In[j++];
		for(i=0;i<8;++i)		round1_4[i]=In[j++];
		for(i=0;i<8;++i)		round1_5[i]=In[j++];
		for(i=0;i<8;++i)		round1_6[i]=In[j++];
		for(i=0;i<8;++i)		round1_7[i]=In[j++];
	}
		pushToken();
		popToken();
	}
};
#endif