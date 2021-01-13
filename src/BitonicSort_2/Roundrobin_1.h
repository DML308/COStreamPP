#ifndef _Roundrobin_1_
#define _Roundrobin_1_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Roundrobin_1{
public:
	Roundrobin_1(Buffer<streamData>& round0_0,Buffer<streamData>& round0_1,Buffer<streamData>& round0_2,Buffer<streamData>& round0_3,Buffer<streamData>& In):round0_0(round0_0),round0_1(round0_1),round0_2(round0_2),round0_3(round0_3),In(In){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		round0_0.resetTail();
		round0_1.resetTail();
		round0_2.resetTail();
		round0_3.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		round0_0.resetTail();
		round0_1.resetTail();
		round0_2.resetTail();
		round0_3.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> round0_0;
	Producer<streamData> round0_1;
	Producer<streamData> round0_2;
	Producer<streamData> round0_3;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(8);
	}
	void pushToken() {
		round0_0.updatetail(2);
		round0_1.updatetail(2);
		round0_2.updatetail(2);
		round0_3.updatetail(2);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<2;++i)		round0_0[i]=In[j++];
		for(i=0;i<2;++i)		round0_1[i]=In[j++];
		for(i=0;i<2;++i)		round0_2[i]=In[j++];
		for(i=0;i<2;++i)		round0_3[i]=In[j++];
	}
		pushToken();
		popToken();
	}
};
#endif