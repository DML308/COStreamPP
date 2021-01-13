#ifndef _join_90_
#define _join_90_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_90{
public:
	join_90(Buffer<streamData>& Out,Buffer<streamData>& Dstream10_0,Buffer<streamData>& Dstream10_1):Out(Out),Dstream10_0(Dstream10_0),Dstream10_1(Dstream10_1){
		steadyScheduleCount = 32;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream10_0.resetHead();
		Dstream10_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream10_0.resetHead();
		Dstream10_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Dstream10_0;
	Consumer<streamData> Dstream10_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Dstream10_0.updatehead(1);
		Dstream10_1.updatehead(1);
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
		for(i=0;i<1;++i)		Out[j++]=Dstream10_0[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream10_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif