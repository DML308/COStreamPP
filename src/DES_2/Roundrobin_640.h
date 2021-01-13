#ifndef _Roundrobin_640_
#define _Roundrobin_640_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Roundrobin_640{
public:
	Roundrobin_640(Buffer<streamData>& round107_0,Buffer<streamData>& round107_1,Buffer<streamData>& In):round107_0(round107_0),round107_1(round107_1),In(In){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		round107_0.resetTail();
		round107_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		round107_0.resetTail();
		round107_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> round107_0;
	Producer<streamData> round107_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(64);
	}
	void pushToken() {
		round107_0.updatetail(32);
		round107_1.updatetail(32);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<32;++i)		round107_0[i]=In[j++];
		for(i=0;i<32;++i)		round107_1[i]=In[j++];
	}
		pushToken();
		popToken();
	}
};
#endif