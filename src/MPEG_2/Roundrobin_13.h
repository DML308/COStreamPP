#ifndef _Roundrobin_13_
#define _Roundrobin_13_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Roundrobin_13{
public:
	Roundrobin_13(Buffer<streamData>& round3_0,Buffer<streamData>& round3_1,Buffer<streamData>& round3_2,Buffer<streamData>& round3_3,Buffer<streamData>& round3_4,Buffer<streamData>& round3_5,Buffer<streamData>& round3_6,Buffer<streamData>& round3_7,Buffer<streamData>& In):round3_0(round3_0),round3_1(round3_1),round3_2(round3_2),round3_3(round3_3),round3_4(round3_4),round3_5(round3_5),round3_6(round3_6),round3_7(round3_7),In(In){
		steadyScheduleCount = 6;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		round3_0.resetTail();
		round3_1.resetTail();
		round3_2.resetTail();
		round3_3.resetTail();
		round3_4.resetTail();
		round3_5.resetTail();
		round3_6.resetTail();
		round3_7.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		round3_0.resetTail();
		round3_1.resetTail();
		round3_2.resetTail();
		round3_3.resetTail();
		round3_4.resetTail();
		round3_5.resetTail();
		round3_6.resetTail();
		round3_7.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> round3_0;
	Producer<streamData> round3_1;
	Producer<streamData> round3_2;
	Producer<streamData> round3_3;
	Producer<streamData> round3_4;
	Producer<streamData> round3_5;
	Producer<streamData> round3_6;
	Producer<streamData> round3_7;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(64);
	}
	void pushToken() {
		round3_0.updatetail(8);
		round3_1.updatetail(8);
		round3_2.updatetail(8);
		round3_3.updatetail(8);
		round3_4.updatetail(8);
		round3_5.updatetail(8);
		round3_6.updatetail(8);
		round3_7.updatetail(8);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<8;++i)		round3_0[i]=In[j++];
		for(i=0;i<8;++i)		round3_1[i]=In[j++];
		for(i=0;i<8;++i)		round3_2[i]=In[j++];
		for(i=0;i<8;++i)		round3_3[i]=In[j++];
		for(i=0;i<8;++i)		round3_4[i]=In[j++];
		for(i=0;i<8;++i)		round3_5[i]=In[j++];
		for(i=0;i<8;++i)		round3_6[i]=In[j++];
		for(i=0;i<8;++i)		round3_7[i]=In[j++];
	}
		pushToken();
		popToken();
	}
};
#endif