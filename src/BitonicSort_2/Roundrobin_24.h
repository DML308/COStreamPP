#ifndef _Roundrobin_24_
#define _Roundrobin_24_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Roundrobin_24{
public:
	Roundrobin_24(Buffer<streamData>& round6_0,Buffer<streamData>& round6_1,Buffer<streamData>& round6_2,Buffer<streamData>& round6_3,Buffer<streamData>& In):round6_0(round6_0),round6_1(round6_1),round6_2(round6_2),round6_3(round6_3),In(In){
		steadyScheduleCount = 2;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		round6_0.resetTail();
		round6_1.resetTail();
		round6_2.resetTail();
		round6_3.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		round6_0.resetTail();
		round6_1.resetTail();
		round6_2.resetTail();
		round6_3.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> round6_0;
	Producer<streamData> round6_1;
	Producer<streamData> round6_2;
	Producer<streamData> round6_3;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(4);
	}
	void pushToken() {
		round6_0.updatetail(1);
		round6_1.updatetail(1);
		round6_2.updatetail(1);
		round6_3.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		round6_0[i]=In[j++];
		for(i=0;i<1;++i)		round6_1[i]=In[j++];
		for(i=0;i<1;++i)		round6_2[i]=In[j++];
		for(i=0;i<1;++i)		round6_3[i]=In[j++];
	}
		pushToken();
		popToken();
	}
};
#endif