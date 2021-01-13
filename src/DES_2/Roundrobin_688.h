#ifndef _Roundrobin_688_
#define _Roundrobin_688_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Roundrobin_688{
public:
	Roundrobin_688(Buffer<streamData>& round115_0,Buffer<streamData>& round115_1,Buffer<streamData>& In):round115_0(round115_0),round115_1(round115_1),In(In){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		round115_0.resetTail();
		round115_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		round115_0.resetTail();
		round115_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> round115_0;
	Producer<streamData> round115_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(64);
	}
	void pushToken() {
		round115_0.updatetail(32);
		round115_1.updatetail(32);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<32;++i)		round115_0[i]=In[j++];
		for(i=0;i<32;++i)		round115_1[i]=In[j++];
	}
		pushToken();
		popToken();
	}
};
#endif