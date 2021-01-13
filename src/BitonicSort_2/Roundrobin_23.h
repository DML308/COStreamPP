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
	Roundrobin_23(Buffer<streamData>& round5_0,Buffer<streamData>& In):round5_0(round5_0),In(In){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		round5_0.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		round5_0.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> round5_0;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(8);
	}
	void pushToken() {
		round5_0.updatetail(8);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<8;++i)		round5_0[i]=In[j++];
	}
		pushToken();
		popToken();
	}
};
#endif