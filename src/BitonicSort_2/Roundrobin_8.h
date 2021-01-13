#ifndef _Roundrobin_8_
#define _Roundrobin_8_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Roundrobin_8{
public:
	Roundrobin_8(Buffer<streamData>& round2_0,Buffer<streamData>& round2_1,Buffer<streamData>& In):round2_0(round2_0),round2_1(round2_1),In(In){
		steadyScheduleCount = 2;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		round2_0.resetTail();
		round2_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		round2_0.resetTail();
		round2_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> round2_0;
	Producer<streamData> round2_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(2);
	}
	void pushToken() {
		round2_0.updatetail(1);
		round2_1.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		round2_0[i]=In[j++];
		for(i=0;i<1;++i)		round2_1[i]=In[j++];
	}
		pushToken();
		popToken();
	}
};
#endif