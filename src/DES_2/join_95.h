#ifndef _join_95_
#define _join_95_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_95{
public:
	join_95(Buffer<streamData>& Out,Buffer<streamData>& Rstream7_0,Buffer<streamData>& Rstream7_1):Out(Out),Rstream7_0(Rstream7_0),Rstream7_1(Rstream7_1){
		steadyScheduleCount = 32;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream7_0.resetHead();
		Rstream7_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream7_0.resetHead();
		Rstream7_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Rstream7_0;
	Consumer<streamData> Rstream7_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Rstream7_0.updatehead(1);
		Rstream7_1.updatehead(1);
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
		for(i=0;i<1;++i)		Out[j++]=Rstream7_0[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream7_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif