#ifndef _Roundrobin_652_
#define _Roundrobin_652_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Roundrobin_652{
public:
	Roundrobin_652(Buffer<streamData>& round109_0,Buffer<streamData>& round109_1,Buffer<streamData>& In):round109_0(round109_0),round109_1(round109_1),In(In){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		round109_0.resetTail();
		round109_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		round109_0.resetTail();
		round109_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> round109_0;
	Producer<streamData> round109_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(64);
	}
	void pushToken() {
		round109_0.updatetail(32);
		round109_1.updatetail(32);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<32;++i)		round109_0[i]=In[j++];
		for(i=0;i<32;++i)		round109_1[i]=In[j++];
	}
		pushToken();
		popToken();
	}
};
#endif