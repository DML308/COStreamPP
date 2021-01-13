#ifndef _Roundrobin_556_
#define _Roundrobin_556_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Roundrobin_556{
public:
	Roundrobin_556(Buffer<streamData>& round93_0,Buffer<streamData>& round93_1,Buffer<streamData>& In):round93_0(round93_0),round93_1(round93_1),In(In){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		round93_0.resetTail();
		round93_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		round93_0.resetTail();
		round93_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> round93_0;
	Producer<streamData> round93_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(64);
	}
	void pushToken() {
		round93_0.updatetail(32);
		round93_1.updatetail(32);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<32;++i)		round93_0[i]=In[j++];
		for(i=0;i<32;++i)		round93_1[i]=In[j++];
	}
		pushToken();
		popToken();
	}
};
#endif