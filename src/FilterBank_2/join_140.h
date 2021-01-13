#ifndef _join_140_
#define _join_140_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_140{
public:
	join_140(Buffer<streamData>& Out,Buffer<streamData>& Dstream14_0,Buffer<streamData>& Dstream14_1):Out(Out),Dstream14_0(Dstream14_0),Dstream14_1(Dstream14_1){
		steadyScheduleCount = 32;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream14_0.resetHead();
		Dstream14_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream14_0.resetHead();
		Dstream14_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Dstream14_0;
	Consumer<streamData> Dstream14_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Dstream14_0.updatehead(1);
		Dstream14_1.updatehead(1);
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
		for(i=0;i<1;++i)		Out[j++]=Dstream14_0[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream14_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif