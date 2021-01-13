#ifndef _join_135_
#define _join_135_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_135{
public:
	join_135(Buffer<streamData>& Out,Buffer<streamData>& Dstream15_0,Buffer<streamData>& Dstream15_1):Out(Out),Dstream15_0(Dstream15_0),Dstream15_1(Dstream15_1){
		steadyScheduleCount = 32;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream15_0.resetHead();
		Dstream15_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream15_0.resetHead();
		Dstream15_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Dstream15_0;
	Consumer<streamData> Dstream15_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Dstream15_0.updatehead(1);
		Dstream15_1.updatehead(1);
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
		for(i=0;i<1;++i)		Out[j++]=Dstream15_0[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream15_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif