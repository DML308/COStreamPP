#ifndef _join_22_
#define _join_22_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_22{
public:
	join_22(Buffer<streamData>& Out,Buffer<streamData>& Rstream4_0,Buffer<streamData>& Rstream4_1,Buffer<streamData>& Rstream4_2,Buffer<streamData>& Rstream4_3):Out(Out),Rstream4_0(Rstream4_0),Rstream4_1(Rstream4_1),Rstream4_2(Rstream4_2),Rstream4_3(Rstream4_3){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream4_0.resetHead();
		Rstream4_1.resetHead();
		Rstream4_2.resetHead();
		Rstream4_3.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream4_0.resetHead();
		Rstream4_1.resetHead();
		Rstream4_2.resetHead();
		Rstream4_3.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Rstream4_0;
	Consumer<streamData> Rstream4_1;
	Consumer<streamData> Rstream4_2;
	Consumer<streamData> Rstream4_3;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Rstream4_0.updatehead(2);
		Rstream4_1.updatehead(2);
		Rstream4_2.updatehead(2);
		Rstream4_3.updatehead(2);
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
		for(i=0;i<2;++i)		Out[j++]=Rstream4_0[i];
		for(i=0;i<2;++i)		Out[j++]=Rstream4_1[i];
		for(i=0;i<2;++i)		Out[j++]=Rstream4_2[i];
		for(i=0;i<2;++i)		Out[j++]=Rstream4_3[i];
	}
		pushToken();
		popToken();
	}
};
#endif