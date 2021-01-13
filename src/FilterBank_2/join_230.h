#ifndef _join_230_
#define _join_230_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_230{
public:
	join_230(Buffer<streamData>& Out,Buffer<streamData>& Dstream23_0,Buffer<streamData>& Dstream23_1):Out(Out),Dstream23_0(Dstream23_0),Dstream23_1(Dstream23_1){
		steadyScheduleCount = 32;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream23_0.resetHead();
		Dstream23_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream23_0.resetHead();
		Dstream23_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Dstream23_0;
	Consumer<streamData> Dstream23_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Dstream23_0.updatehead(1);
		Dstream23_1.updatehead(1);
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
		for(i=0;i<1;++i)		Out[j++]=Dstream23_0[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream23_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif