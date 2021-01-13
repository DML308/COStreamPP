#ifndef _Roundrobin_412_
#define _Roundrobin_412_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Roundrobin_412{
public:
	Roundrobin_412(Buffer<streamData>& round69_0,Buffer<streamData>& round69_1,Buffer<streamData>& In):round69_0(round69_0),round69_1(round69_1),In(In){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		round69_0.resetTail();
		round69_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		round69_0.resetTail();
		round69_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> round69_0;
	Producer<streamData> round69_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(64);
	}
	void pushToken() {
		round69_0.updatetail(32);
		round69_1.updatetail(32);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<32;++i)		round69_0[i]=In[j++];
		for(i=0;i<32;++i)		round69_1[i]=In[j++];
	}
		pushToken();
		popToken();
	}
};
#endif