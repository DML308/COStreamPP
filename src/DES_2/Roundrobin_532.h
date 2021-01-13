#ifndef _Roundrobin_532_
#define _Roundrobin_532_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Roundrobin_532{
public:
	Roundrobin_532(Buffer<streamData>& round89_0,Buffer<streamData>& round89_1,Buffer<streamData>& In):round89_0(round89_0),round89_1(round89_1),In(In){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		round89_0.resetTail();
		round89_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		round89_0.resetTail();
		round89_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> round89_0;
	Producer<streamData> round89_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(64);
	}
	void pushToken() {
		round89_0.updatetail(32);
		round89_1.updatetail(32);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<32;++i)		round89_0[i]=In[j++];
		for(i=0;i<32;++i)		round89_1[i]=In[j++];
	}
		pushToken();
		popToken();
	}
};
#endif