#ifndef _Roundrobin_484_
#define _Roundrobin_484_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Roundrobin_484{
public:
	Roundrobin_484(Buffer<streamData>& round81_0,Buffer<streamData>& round81_1,Buffer<streamData>& In):round81_0(round81_0),round81_1(round81_1),In(In){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		round81_0.resetTail();
		round81_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		round81_0.resetTail();
		round81_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> round81_0;
	Producer<streamData> round81_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(64);
	}
	void pushToken() {
		round81_0.updatetail(32);
		round81_1.updatetail(32);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<32;++i)		round81_0[i]=In[j++];
		for(i=0;i<32;++i)		round81_1[i]=In[j++];
	}
		pushToken();
		popToken();
	}
};
#endif