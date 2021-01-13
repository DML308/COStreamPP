#ifndef _Duplicate_195_
#define _Duplicate_195_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Duplicate_195{
public:
	Duplicate_195(Buffer<streamData>& dup22_0,Buffer<streamData>& dup22_1,Buffer<streamData>& In):dup22_0(dup22_0),dup22_1(dup22_1),In(In){
		steadyScheduleCount = 32;
		initScheduleCount = 127;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		dup22_0.resetTail();
		dup22_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		dup22_0.resetTail();
		dup22_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> dup22_0;
	Producer<streamData> dup22_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		dup22_0.updatetail(1);
		dup22_1.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		dup22_0[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup22_1[i]=In[j++];
		j=0;
	}
		pushToken();
		popToken();
	}
};
#endif