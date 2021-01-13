#ifndef _join_30_
#define _join_30_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_30{
public:
	join_30(Buffer<streamData>& Out,Buffer<streamData>& Rstream5_0):Out(Out),Rstream5_0(Rstream5_0){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream5_0.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream5_0.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Rstream5_0;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Rstream5_0.updatehead(8);
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
		for(i=0;i<8;++i)		Out[j++]=Rstream5_0[i];
	}
		pushToken();
		popToken();
	}
};
#endif