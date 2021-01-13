#ifndef _join_491_
#define _join_491_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_491{
public:
	join_491(Buffer<streamData>& Out,Buffer<streamData>& Rstream40_0,Buffer<streamData>& Rstream40_1):Out(Out),Rstream40_0(Rstream40_0),Rstream40_1(Rstream40_1){
		steadyScheduleCount = 32;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream40_0.resetHead();
		Rstream40_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream40_0.resetHead();
		Rstream40_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Rstream40_0;
	Consumer<streamData> Rstream40_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Rstream40_0.updatehead(1);
		Rstream40_1.updatehead(1);
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
		for(i=0;i<1;++i)		Out[j++]=Rstream40_0[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream40_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif