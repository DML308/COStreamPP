#ifndef _join_335_
#define _join_335_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_335{
public:
	join_335(Buffer<streamData>& Out,Buffer<streamData>& Rstream27_0,Buffer<streamData>& Rstream27_1):Out(Out),Rstream27_0(Rstream27_0),Rstream27_1(Rstream27_1){
		steadyScheduleCount = 32;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream27_0.resetHead();
		Rstream27_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream27_0.resetHead();
		Rstream27_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Rstream27_0;
	Consumer<streamData> Rstream27_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Rstream27_0.updatehead(1);
		Rstream27_1.updatehead(1);
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
		for(i=0;i<1;++i)		Out[j++]=Rstream27_0[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream27_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif