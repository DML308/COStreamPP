#ifndef _Roundrobin_736_
#define _Roundrobin_736_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Roundrobin_736{
public:
	Roundrobin_736(Buffer<streamData>& round123_0,Buffer<streamData>& round123_1,Buffer<streamData>& In):round123_0(round123_0),round123_1(round123_1),In(In){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		round123_0.resetTail();
		round123_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		round123_0.resetTail();
		round123_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> round123_0;
	Producer<streamData> round123_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(64);
	}
	void pushToken() {
		round123_0.updatetail(32);
		round123_1.updatetail(32);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<32;++i)		round123_0[i]=In[j++];
		for(i=0;i<32;++i)		round123_1[i]=In[j++];
	}
		pushToken();
		popToken();
	}
};
#endif