#ifndef _join_131_
#define _join_131_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_131{
public:
	join_131(Buffer<streamData>& Out,Buffer<streamData>& Rstream10_0,Buffer<streamData>& Rstream10_1):Out(Out),Rstream10_0(Rstream10_0),Rstream10_1(Rstream10_1){
		steadyScheduleCount = 32;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream10_0.resetHead();
		Rstream10_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream10_0.resetHead();
		Rstream10_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Rstream10_0;
	Consumer<streamData> Rstream10_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Rstream10_0.updatehead(1);
		Rstream10_1.updatehead(1);
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
		for(i=0;i<1;++i)		Out[j++]=Rstream10_0[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream10_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif