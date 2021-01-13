#ifndef _join_563_
#define _join_563_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_563{
public:
	join_563(Buffer<streamData>& Out,Buffer<streamData>& Rstream46_0,Buffer<streamData>& Rstream46_1):Out(Out),Rstream46_0(Rstream46_0),Rstream46_1(Rstream46_1){
		steadyScheduleCount = 32;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream46_0.resetHead();
		Rstream46_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream46_0.resetHead();
		Rstream46_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Rstream46_0;
	Consumer<streamData> Rstream46_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Rstream46_0.updatehead(1);
		Rstream46_1.updatehead(1);
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
		for(i=0;i<1;++i)		Out[j++]=Rstream46_0[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream46_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif