#ifndef _join_26_
#define _join_26_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_26{
public:
	join_26(Buffer<streamData>& Out,Buffer<streamData>& Dstream1_0,Buffer<streamData>& Dstream1_1):Out(Out),Dstream1_0(Dstream1_0),Dstream1_1(Dstream1_1){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream1_0.resetHead();
		Dstream1_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream1_0.resetHead();
		Dstream1_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Dstream1_0;
	Consumer<streamData> Dstream1_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Dstream1_0.updatehead(32);
		Dstream1_1.updatehead(32);
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
		for(i=0;i<32;++i)		Out[j++]=Dstream1_0[i];
		for(i=0;i<32;++i)		Out[j++]=Dstream1_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif