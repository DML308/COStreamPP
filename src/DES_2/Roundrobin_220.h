#ifndef _Roundrobin_220_
#define _Roundrobin_220_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Roundrobin_220{
public:
	Roundrobin_220(Buffer<streamData>& round37_0,Buffer<streamData>& round37_1,Buffer<streamData>& In):round37_0(round37_0),round37_1(round37_1),In(In){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		round37_0.resetTail();
		round37_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		round37_0.resetTail();
		round37_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> round37_0;
	Producer<streamData> round37_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(64);
	}
	void pushToken() {
		round37_0.updatetail(32);
		round37_1.updatetail(32);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<32;++i)		round37_0[i]=In[j++];
		for(i=0;i<32;++i)		round37_1[i]=In[j++];
	}
		pushToken();
		popToken();
	}
};
#endif