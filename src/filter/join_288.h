#ifndef _join_288_
#define _join_288_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_288{
public:
	join_288(Buffer<streamData>& Out,Buffer<streamData>& Dstream32_0,Buffer<streamData>& Dstream32_1):Out(Out),Dstream32_0(Dstream32_0),Dstream32_1(Dstream32_1){
		steadyScheduleCount = 32;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream32_0.resetHead();
		Dstream32_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream32_0.resetHead();
		Dstream32_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Dstream32_0;
	Consumer<streamData> Dstream32_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Dstream32_0.updatehead(1);
		Dstream32_1.updatehead(1);
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
		for(i=0;i<1;++i)		Out[j++]=Dstream32_0[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream32_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif