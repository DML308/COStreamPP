#ifndef _Duplicate_132_
#define _Duplicate_132_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Duplicate_132{
public:
	Duplicate_132(Buffer<streamData>& dup15_0,Buffer<streamData>& dup15_1,Buffer<streamData>& In):dup15_0(dup15_0),dup15_1(dup15_1),In(In){
		steadyScheduleCount = 32;
		initScheduleCount = 127;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		dup15_0.resetTail();
		dup15_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		dup15_0.resetTail();
		dup15_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> dup15_0;
	Producer<streamData> dup15_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		dup15_0.updatetail(1);
		dup15_1.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		dup15_0[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup15_1[i]=In[j++];
		j=0;
	}
		pushToken();
		popToken();
	}
};
#endif