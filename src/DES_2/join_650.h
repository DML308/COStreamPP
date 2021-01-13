#ifndef _join_650_
#define _join_650_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_650{
public:
	join_650(Buffer<streamData>& Out,Buffer<streamData>& Dstream53_0,Buffer<streamData>& Dstream53_1):Out(Out),Dstream53_0(Dstream53_0),Dstream53_1(Dstream53_1){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream53_0.resetHead();
		Dstream53_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream53_0.resetHead();
		Dstream53_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Dstream53_0;
	Consumer<streamData> Dstream53_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Dstream53_0.updatehead(32);
		Dstream53_1.updatehead(32);
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
		for(i=0;i<32;++i)		Out[j++]=Dstream53_0[i];
		for(i=0;i<32;++i)		Out[j++]=Dstream53_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif