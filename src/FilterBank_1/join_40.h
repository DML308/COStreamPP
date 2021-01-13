#ifndef _join_40_
#define _join_40_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_40{
public:
	join_40(Buffer<streamData>& Out,Buffer<streamData>& Dstream4_0,Buffer<streamData>& Dstream4_1):Out(Out),Dstream4_0(Dstream4_0),Dstream4_1(Dstream4_1){
		steadyScheduleCount = 32;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream4_0.resetHead();
		Dstream4_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream4_0.resetHead();
		Dstream4_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Dstream4_0;
	Consumer<streamData> Dstream4_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Dstream4_0.updatehead(1);
		Dstream4_1.updatehead(1);
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
		for(i=0;i<1;++i)		Out[j++]=Dstream4_0[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream4_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif