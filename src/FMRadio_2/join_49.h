#ifndef _join_49_
#define _join_49_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_49{
public:
	join_49(Buffer<streamData>& Out,Buffer<streamData>& Dstream8_0,Buffer<streamData>& Dstream8_1):Out(Out),Dstream8_0(Dstream8_0),Dstream8_1(Dstream8_1){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream8_0.resetHead();
		Dstream8_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream8_0.resetHead();
		Dstream8_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Dstream8_0;
	Consumer<streamData> Dstream8_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Dstream8_0.updatehead(1);
		Dstream8_1.updatehead(1);
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
		for(i=0;i<1;++i)		Out[j++]=Dstream8_0[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream8_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif