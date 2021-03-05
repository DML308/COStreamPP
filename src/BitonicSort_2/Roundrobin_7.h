#ifndef _Roundrobin_7_
#define _Roundrobin_7_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Roundrobin_7{
public:
	Roundrobin_7(Buffer<streamData>& round1_0,Buffer<streamData>& round1_1,Buffer<streamData>& In):round1_0(round1_0),round1_1(round1_1),In(In){
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
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		round1_0.resetTail();
		round1_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> round1_0;
	Producer<streamData> round1_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(8);
	}
	void pushToken() {
		round1_0.updatetail(4);
		round1_1.updatetail(4);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<4;++i)		round1_0[i]=In[j++];
		for(i=0;i<4;++i)		round1_1[i]=In[j++];
	}
		pushToken();
		popToken();
	}
};
#endif