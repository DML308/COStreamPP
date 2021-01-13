#ifndef _join_371_
#define _join_371_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_371{
public:
	join_371(Buffer<streamData>& Out,Buffer<streamData>& Rstream30_0,Buffer<streamData>& Rstream30_1):Out(Out),Rstream30_0(Rstream30_0),Rstream30_1(Rstream30_1){
		steadyScheduleCount = 32;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream30_0.resetHead();
		Rstream30_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream30_0.resetHead();
		Rstream30_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Rstream30_0;
	Consumer<streamData> Rstream30_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Rstream30_0.updatehead(1);
		Rstream30_1.updatehead(1);
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
		for(i=0;i<1;++i)		Out[j++]=Rstream30_0[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream30_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif