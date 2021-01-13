#ifndef _join_11_
#define _join_11_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_11{
public:
	join_11(Buffer<streamData>& Out,Buffer<streamData>& Rstream0_0,Buffer<streamData>& Rstream0_1):Out(Out),Rstream0_0(Rstream0_0),Rstream0_1(Rstream0_1){
		steadyScheduleCount = 32;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream0_0.resetHead();
		Rstream0_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream0_0.resetHead();
		Rstream0_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Rstream0_0;
	Consumer<streamData> Rstream0_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Rstream0_0.updatehead(1);
		Rstream0_1.updatehead(1);
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
		for(i=0;i<1;++i)		Out[j++]=Rstream0_0[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream0_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif