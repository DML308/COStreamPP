#ifndef _Roundrobin_17_
#define _Roundrobin_17_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Roundrobin_17{
public:
	Roundrobin_17(Buffer<streamData>& round4_0,Buffer<streamData>& round4_1,Buffer<streamData>& round4_2,Buffer<streamData>& round4_3,Buffer<streamData>& In):round4_0(round4_0),round4_1(round4_1),round4_2(round4_2),round4_3(round4_3),In(In){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		round4_0.resetTail();
		round4_1.resetTail();
		round4_2.resetTail();
		round4_3.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		round4_0.resetTail();
		round4_1.resetTail();
		round4_2.resetTail();
		round4_3.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> round4_0;
	Producer<streamData> round4_1;
	Producer<streamData> round4_2;
	Producer<streamData> round4_3;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(8);
	}
	void pushToken() {
		round4_0.updatetail(2);
		round4_1.updatetail(2);
		round4_2.updatetail(2);
		round4_3.updatetail(2);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<2;++i)		round4_0[i]=In[j++];
		for(i=0;i<2;++i)		round4_1[i]=In[j++];
		for(i=0;i<2;++i)		round4_2[i]=In[j++];
		for(i=0;i<2;++i)		round4_3[i]=In[j++];
	}
		pushToken();
		popToken();
	}
};
#endif