#ifndef _join_29_
#define _join_29_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_29{
public:
	join_29(Buffer<streamData>& Out,Buffer<streamData>& Rstream6_0,Buffer<streamData>& Rstream6_1,Buffer<streamData>& Rstream6_2,Buffer<streamData>& Rstream6_3):Out(Out),Rstream6_0(Rstream6_0),Rstream6_1(Rstream6_1),Rstream6_2(Rstream6_2),Rstream6_3(Rstream6_3){
		steadyScheduleCount = 2;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream6_0.resetHead();
		Rstream6_1.resetHead();
		Rstream6_2.resetHead();
		Rstream6_3.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream6_0.resetHead();
		Rstream6_1.resetHead();
		Rstream6_2.resetHead();
		Rstream6_3.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Rstream6_0;
	Consumer<streamData> Rstream6_1;
	Consumer<streamData> Rstream6_2;
	Consumer<streamData> Rstream6_3;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Rstream6_0.updatehead(1);
		Rstream6_1.updatehead(1);
		Rstream6_2.updatehead(1);
		Rstream6_3.updatehead(1);
	}
	void pushToken() {
		Out.updatetail(4);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		Out[j++]=Rstream6_0[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream6_1[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream6_2[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream6_3[i];
	}
		pushToken();
		popToken();
	}
};
#endif