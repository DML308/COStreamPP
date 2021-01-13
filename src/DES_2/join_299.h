#ifndef _join_299_
#define _join_299_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_299{
public:
	join_299(Buffer<streamData>& Out,Buffer<streamData>& Rstream24_0,Buffer<streamData>& Rstream24_1):Out(Out),Rstream24_0(Rstream24_0),Rstream24_1(Rstream24_1){
		steadyScheduleCount = 32;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream24_0.resetHead();
		Rstream24_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream24_0.resetHead();
		Rstream24_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Rstream24_0;
	Consumer<streamData> Rstream24_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Rstream24_0.updatehead(1);
		Rstream24_1.updatehead(1);
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
		for(i=0;i<1;++i)		Out[j++]=Rstream24_0[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream24_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif