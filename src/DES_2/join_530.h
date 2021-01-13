#ifndef _join_530_
#define _join_530_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_530{
public:
	join_530(Buffer<streamData>& Out,Buffer<streamData>& Dstream43_0,Buffer<streamData>& Dstream43_1):Out(Out),Dstream43_0(Dstream43_0),Dstream43_1(Dstream43_1){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream43_0.resetHead();
		Dstream43_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream43_0.resetHead();
		Dstream43_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Dstream43_0;
	Consumer<streamData> Dstream43_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Dstream43_0.updatehead(32);
		Dstream43_1.updatehead(32);
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
		for(i=0;i<32;++i)		Out[j++]=Dstream43_0[i];
		for(i=0;i<32;++i)		Out[j++]=Dstream43_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif