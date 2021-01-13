#ifndef _join_107_
#define _join_107_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_107{
public:
	join_107(Buffer<streamData>& Out,Buffer<streamData>& Rstream8_0,Buffer<streamData>& Rstream8_1):Out(Out),Rstream8_0(Rstream8_0),Rstream8_1(Rstream8_1){
		steadyScheduleCount = 32;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream8_0.resetHead();
		Rstream8_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream8_0.resetHead();
		Rstream8_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Rstream8_0;
	Consumer<streamData> Rstream8_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Rstream8_0.updatehead(1);
		Rstream8_1.updatehead(1);
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
		for(i=0;i<1;++i)		Out[j++]=Rstream8_0[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream8_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif