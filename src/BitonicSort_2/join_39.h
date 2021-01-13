#ifndef _join_39_
#define _join_39_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_39{
public:
	join_39(Buffer<streamData>& Out,Buffer<streamData>& Rstream9_0,Buffer<streamData>& Rstream9_1):Out(Out),Rstream9_0(Rstream9_0),Rstream9_1(Rstream9_1){
		steadyScheduleCount = 2;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream9_0.resetHead();
		Rstream9_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream9_0.resetHead();
		Rstream9_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Rstream9_0;
	Consumer<streamData> Rstream9_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Rstream9_0.updatehead(1);
		Rstream9_1.updatehead(1);
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
		for(i=0;i<1;++i)		Out[j++]=Rstream9_0[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream9_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif