#ifndef _join_6_
#define _join_6_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_6{
public:
	join_6(Buffer<streamData>& Out,Buffer<streamData>& Rstream0_0,Buffer<streamData>& Rstream0_1,Buffer<streamData>& Rstream0_2,Buffer<streamData>& Rstream0_3):Out(Out),Rstream0_0(Rstream0_0),Rstream0_1(Rstream0_1),Rstream0_2(Rstream0_2),Rstream0_3(Rstream0_3){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream0_0.resetHead();
		Rstream0_1.resetHead();
		Rstream0_2.resetHead();
		Rstream0_3.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream0_0.resetHead();
		Rstream0_1.resetHead();
		Rstream0_2.resetHead();
		Rstream0_3.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Rstream0_0;
	Consumer<streamData> Rstream0_1;
	Consumer<streamData> Rstream0_2;
	Consumer<streamData> Rstream0_3;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Rstream0_0.updatehead(2);
		Rstream0_1.updatehead(2);
		Rstream0_2.updatehead(2);
		Rstream0_3.updatehead(2);
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
		for(i=0;i<2;++i)		Out[j++]=Rstream0_0[i];
		for(i=0;i<2;++i)		Out[j++]=Rstream0_1[i];
		for(i=0;i<2;++i)		Out[j++]=Rstream0_2[i];
		for(i=0;i<2;++i)		Out[j++]=Rstream0_3[i];
	}
		pushToken();
		popToken();
	}
};
#endif