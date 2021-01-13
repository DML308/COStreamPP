#ifndef _Roundrobin_12_
#define _Roundrobin_12_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Roundrobin_12{
public:
	Roundrobin_12(Buffer<streamData>& round3_0,Buffer<streamData>& round3_1,Buffer<streamData>& In):round3_0(round3_0),round3_1(round3_1),In(In){
		steadyScheduleCount = 2;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		round3_0.resetTail();
		round3_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		round3_0.resetTail();
		round3_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> round3_0;
	Producer<streamData> round3_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(2);
	}
	void pushToken() {
		round3_0.updatetail(1);
		round3_1.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		round3_0[i]=In[j++];
		for(i=0;i<1;++i)		round3_1[i]=In[j++];
	}
		pushToken();
		popToken();
	}
};
#endif