#ifndef _join_234_
#define _join_234_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_234{
public:
	join_234(Buffer<streamData>& Out,Buffer<streamData>& Dstream26_0,Buffer<streamData>& Dstream26_1):Out(Out),Dstream26_0(Dstream26_0),Dstream26_1(Dstream26_1){
		steadyScheduleCount = 32;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream26_0.resetHead();
		Dstream26_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream26_0.resetHead();
		Dstream26_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Dstream26_0;
	Consumer<streamData> Dstream26_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Dstream26_0.updatehead(1);
		Dstream26_1.updatehead(1);
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
		for(i=0;i<1;++i)		Out[j++]=Dstream26_0[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream26_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif