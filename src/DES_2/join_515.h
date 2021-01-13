#ifndef _join_515_
#define _join_515_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_515{
public:
	join_515(Buffer<streamData>& Out,Buffer<streamData>& Rstream42_0,Buffer<streamData>& Rstream42_1):Out(Out),Rstream42_0(Rstream42_0),Rstream42_1(Rstream42_1){
		steadyScheduleCount = 32;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream42_0.resetHead();
		Rstream42_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream42_0.resetHead();
		Rstream42_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Rstream42_0;
	Consumer<streamData> Rstream42_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Rstream42_0.updatehead(1);
		Rstream42_1.updatehead(1);
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
		for(i=0;i<1;++i)		Out[j++]=Rstream42_0[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream42_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif