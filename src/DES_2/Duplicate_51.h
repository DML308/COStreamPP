#ifndef _Duplicate_51_
#define _Duplicate_51_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Duplicate_51{
public:
	Duplicate_51(Buffer<streamData>& dup8_0,Buffer<streamData>& dup8_1,Buffer<streamData>& In):dup8_0(dup8_0),dup8_1(dup8_1),In(In){
		steadyScheduleCount = 64;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		dup8_0.resetTail();
		dup8_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		dup8_0.resetTail();
		dup8_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> dup8_0;
	Producer<streamData> dup8_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		dup8_0.updatetail(1);
		dup8_1.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		dup8_0[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup8_1[i]=In[j++];
		j=0;
	}
		pushToken();
		popToken();
	}
};
#endif