#ifndef _join_27_
#define _join_27_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_27{
public:
	join_27(Buffer<streamData>& Out,Buffer<streamData>& Dstream3_0,Buffer<streamData>& Dstream3_1):Out(Out),Dstream3_0(Dstream3_0),Dstream3_1(Dstream3_1){
		steadyScheduleCount = 32;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream3_0.resetHead();
		Dstream3_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream3_0.resetHead();
		Dstream3_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Dstream3_0;
	Consumer<streamData> Dstream3_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Dstream3_0.updatehead(1);
		Dstream3_1.updatehead(1);
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
		for(i=0;i<1;++i)		Out[j++]=Dstream3_0[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream3_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif