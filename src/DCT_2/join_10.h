#ifndef _join_10_
#define _join_10_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_10{
public:
	join_10(Buffer<streamData>& Out,Buffer<streamData>& Rstream0_0,Buffer<streamData>& Rstream0_1,Buffer<streamData>& Rstream0_2,Buffer<streamData>& Rstream0_3,Buffer<streamData>& Rstream0_4,Buffer<streamData>& Rstream0_5,Buffer<streamData>& Rstream0_6,Buffer<streamData>& Rstream0_7):Out(Out),Rstream0_0(Rstream0_0),Rstream0_1(Rstream0_1),Rstream0_2(Rstream0_2),Rstream0_3(Rstream0_3),Rstream0_4(Rstream0_4),Rstream0_5(Rstream0_5),Rstream0_6(Rstream0_6),Rstream0_7(Rstream0_7){
		steadyScheduleCount = 8;
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
		Rstream0_4.resetHead();
		Rstream0_5.resetHead();
		Rstream0_6.resetHead();
		Rstream0_7.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream0_0.resetHead();
		Rstream0_1.resetHead();
		Rstream0_2.resetHead();
		Rstream0_3.resetHead();
		Rstream0_4.resetHead();
		Rstream0_5.resetHead();
		Rstream0_6.resetHead();
		Rstream0_7.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Rstream0_0;
	Consumer<streamData> Rstream0_1;
	Consumer<streamData> Rstream0_2;
	Consumer<streamData> Rstream0_3;
	Consumer<streamData> Rstream0_4;
	Consumer<streamData> Rstream0_5;
	Consumer<streamData> Rstream0_6;
	Consumer<streamData> Rstream0_7;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Rstream0_0.updatehead(1);
		Rstream0_1.updatehead(1);
		Rstream0_2.updatehead(1);
		Rstream0_3.updatehead(1);
		Rstream0_4.updatehead(1);
		Rstream0_5.updatehead(1);
		Rstream0_6.updatehead(1);
		Rstream0_7.updatehead(1);
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
		for(i=0;i<1;++i)		Out[j++]=Rstream0_0[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream0_1[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream0_2[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream0_3[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream0_4[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream0_5[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream0_6[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream0_7[i];
	}
		pushToken();
		popToken();
	}
};
#endif