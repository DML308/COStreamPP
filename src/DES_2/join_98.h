#ifndef _join_98_
#define _join_98_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_98{
public:
	join_98(Buffer<streamData>& Out,Buffer<streamData>& Dstream7_0,Buffer<streamData>& Dstream7_1):Out(Out),Dstream7_0(Dstream7_0),Dstream7_1(Dstream7_1){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream7_0.resetHead();
		Dstream7_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream7_0.resetHead();
		Dstream7_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Dstream7_0;
	Consumer<streamData> Dstream7_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Dstream7_0.updatehead(32);
		Dstream7_1.updatehead(32);
	}
	void pushToken() {
		Out.updatetail(64);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<32;++i)		Out[j++]=Dstream7_0[i];
		for(i=0;i<32;++i)		Out[j++]=Dstream7_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif