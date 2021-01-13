#ifndef _join_611_
#define _join_611_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_611{
public:
	join_611(Buffer<streamData>& Out,Buffer<streamData>& Rstream50_0,Buffer<streamData>& Rstream50_1):Out(Out),Rstream50_0(Rstream50_0),Rstream50_1(Rstream50_1){
		steadyScheduleCount = 32;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream50_0.resetHead();
		Rstream50_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream50_0.resetHead();
		Rstream50_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Rstream50_0;
	Consumer<streamData> Rstream50_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Rstream50_0.updatehead(1);
		Rstream50_1.updatehead(1);
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
		for(i=0;i<1;++i)		Out[j++]=Rstream50_0[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream50_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif