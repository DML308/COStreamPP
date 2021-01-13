#ifndef _join_16_
#define _join_16_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_16{
public:
	join_16(Buffer<streamData>& Out,Buffer<streamData>& Rstream1_0,Buffer<streamData>& Rstream1_1):Out(Out),Rstream1_0(Rstream1_0),Rstream1_1(Rstream1_1){
		steadyScheduleCount = 1;
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
		Rstream1_0.updatehead(4);
		Rstream1_1.updatehead(4);
	}
	void pushToken() {
		Out.updatetail(8);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<4;++i)		Out[j++]=Rstream1_0[i];
		for(i=0;i<4;++i)		Out[j++]=Rstream1_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif