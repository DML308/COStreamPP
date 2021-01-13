#ifndef _join_15_
#define _join_15_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_15{
public:
	join_15(Buffer<streamData>& Out,Buffer<streamData>& Rstream3_0,Buffer<streamData>& Rstream3_1):Out(Out),Rstream3_0(Rstream3_0),Rstream3_1(Rstream3_1){
		steadyScheduleCount = 2;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream3_0.resetHead();
		Rstream3_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream3_0.resetHead();
		Rstream3_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Rstream3_0;
	Consumer<streamData> Rstream3_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Rstream3_0.updatehead(1);
		Rstream3_1.updatehead(1);
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
		for(i=0;i<1;++i)		Out[j++]=Rstream3_0[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream3_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif