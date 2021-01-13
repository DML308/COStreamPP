#ifndef _Roundrobin_448_
#define _Roundrobin_448_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Roundrobin_448{
public:
	Roundrobin_448(Buffer<streamData>& round75_0,Buffer<streamData>& round75_1,Buffer<streamData>& In):round75_0(round75_0),round75_1(round75_1),In(In){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		round75_0.resetTail();
		round75_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		round75_0.resetTail();
		round75_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> round75_0;
	Producer<streamData> round75_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(64);
	}
	void pushToken() {
		round75_0.updatetail(32);
		round75_1.updatetail(32);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<32;++i)		round75_0[i]=In[j++];
		for(i=0;i<32;++i)		round75_1[i]=In[j++];
	}
		pushToken();
		popToken();
	}
};
#endif