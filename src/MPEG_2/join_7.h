#ifndef _join_7_
#define _join_7_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_7{
public:
	join_7(Buffer<streamData>& Out,Buffer<streamData>& Rstream1_0,Buffer<streamData>& Rstream1_1):Out(Out),Rstream1_0(Rstream1_0),Rstream1_1(Rstream1_1){
		steadyScheduleCount = 6;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream1_0.resetHead();
		Rstream1_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream1_0.resetHead();
		Rstream1_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Rstream1_0;
	Consumer<streamData> Rstream1_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Rstream1_0.updatehead(1);
		Rstream1_1.updatehead(63);
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
		for(i=0;i<1;++i)		Out[j++]=Rstream1_0[i];
		for(i=0;i<63;++i)		Out[j++]=Rstream1_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif