#ifndef _join_45_
#define _join_45_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_45{
public:
	join_45(Buffer<streamData>& Out,Buffer<streamData>& Dstream5_0,Buffer<streamData>& Dstream5_1):Out(Out),Dstream5_0(Dstream5_0),Dstream5_1(Dstream5_1){
		steadyScheduleCount = 32;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream5_0.resetHead();
		Dstream5_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream5_0.resetHead();
		Dstream5_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Dstream5_0;
	Consumer<streamData> Dstream5_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Dstream5_0.updatehead(1);
		Dstream5_1.updatehead(1);
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
		for(i=0;i<1;++i)		Out[j++]=Dstream5_0[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream5_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif