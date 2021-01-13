#ifndef _join_191_
#define _join_191_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_191{
public:
	join_191(Buffer<streamData>& Out,Buffer<streamData>& Rstream15_0,Buffer<streamData>& Rstream15_1):Out(Out),Rstream15_0(Rstream15_0),Rstream15_1(Rstream15_1){
		steadyScheduleCount = 32;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream15_0.resetHead();
		Rstream15_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream15_0.resetHead();
		Rstream15_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Rstream15_0;
	Consumer<streamData> Rstream15_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Rstream15_0.updatehead(1);
		Rstream15_1.updatehead(1);
	}
	void pushToken() {
		Out.updatetail(2);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		Out[j++]=Rstream15_0[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream15_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif