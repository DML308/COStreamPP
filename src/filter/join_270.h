#ifndef _join_270_
#define _join_270_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_270{
public:
	join_270(Buffer<streamData>& Out,Buffer<streamData>& Dstream30_0,Buffer<streamData>& Dstream30_1):Out(Out),Dstream30_0(Dstream30_0),Dstream30_1(Dstream30_1){
		steadyScheduleCount = 32;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream30_0.resetHead();
		Dstream30_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream30_0.resetHead();
		Dstream30_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Dstream30_0;
	Consumer<streamData> Dstream30_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Dstream30_0.updatehead(1);
		Dstream30_1.updatehead(1);
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
		for(i=0;i<1;++i)		Out[j++]=Dstream30_0[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream30_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif