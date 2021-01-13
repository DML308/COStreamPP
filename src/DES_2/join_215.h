#ifndef _join_215_
#define _join_215_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_215{
public:
	join_215(Buffer<streamData>& Out,Buffer<streamData>& Rstream17_0,Buffer<streamData>& Rstream17_1):Out(Out),Rstream17_0(Rstream17_0),Rstream17_1(Rstream17_1){
		steadyScheduleCount = 32;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream17_0.resetHead();
		Rstream17_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream17_0.resetHead();
		Rstream17_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Rstream17_0;
	Consumer<streamData> Rstream17_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Rstream17_0.updatehead(1);
		Rstream17_1.updatehead(1);
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
		for(i=0;i<1;++i)		Out[j++]=Rstream17_0[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream17_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif