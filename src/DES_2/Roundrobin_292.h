#ifndef _Roundrobin_292_
#define _Roundrobin_292_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Roundrobin_292{
public:
	Roundrobin_292(Buffer<streamData>& round49_0,Buffer<streamData>& round49_1,Buffer<streamData>& In):round49_0(round49_0),round49_1(round49_1),In(In){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		round49_0.resetTail();
		round49_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		round49_0.resetTail();
		round49_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> round49_0;
	Producer<streamData> round49_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(64);
	}
	void pushToken() {
		round49_0.updatetail(32);
		round49_1.updatetail(32);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<32;++i)		round49_0[i]=In[j++];
		for(i=0;i<32;++i)		round49_1[i]=In[j++];
	}
		pushToken();
		popToken();
	}
};
#endif