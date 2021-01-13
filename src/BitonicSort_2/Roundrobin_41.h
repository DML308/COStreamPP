#ifndef _Roundrobin_41_
#define _Roundrobin_41_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Roundrobin_41{
public:
	Roundrobin_41(Buffer<streamData>& round10_0,Buffer<streamData>& round10_1,Buffer<streamData>& round10_2,Buffer<streamData>& round10_3,Buffer<streamData>& In):round10_0(round10_0),round10_1(round10_1),round10_2(round10_2),round10_3(round10_3),In(In){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		round10_0.resetTail();
		round10_1.resetTail();
		round10_2.resetTail();
		round10_3.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		round10_0.resetTail();
		round10_1.resetTail();
		round10_2.resetTail();
		round10_3.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> round10_0;
	Producer<streamData> round10_1;
	Producer<streamData> round10_2;
	Producer<streamData> round10_3;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(8);
	}
	void pushToken() {
		round10_0.updatetail(2);
		round10_1.updatetail(2);
		round10_2.updatetail(2);
		round10_3.updatetail(2);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<2;++i)		round10_0[i]=In[j++];
		for(i=0;i<2;++i)		round10_1[i]=In[j++];
		for(i=0;i<2;++i)		round10_2[i]=In[j++];
		for(i=0;i<2;++i)		round10_3[i]=In[j++];
	}
		pushToken();
		popToken();
	}
};
#endif