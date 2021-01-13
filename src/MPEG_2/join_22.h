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
	join_22(Buffer<streamData>& Out,Buffer<streamData>& Rstream2_0,Buffer<streamData>& Rstream2_1,Buffer<streamData>& Rstream2_2,Buffer<streamData>& Rstream2_3,Buffer<streamData>& Rstream2_4,Buffer<streamData>& Rstream2_5,Buffer<streamData>& Rstream2_6,Buffer<streamData>& Rstream2_7):Out(Out),Rstream2_0(Rstream2_0),Rstream2_1(Rstream2_1),Rstream2_2(Rstream2_2),Rstream2_3(Rstream2_3),Rstream2_4(Rstream2_4),Rstream2_5(Rstream2_5),Rstream2_6(Rstream2_6),Rstream2_7(Rstream2_7){
		steadyScheduleCount = 6;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream2_0.resetHead();
		Rstream2_1.resetHead();
		Rstream2_2.resetHead();
		Rstream2_3.resetHead();
		Rstream2_4.resetHead();
		Rstream2_5.resetHead();
		Rstream2_6.resetHead();
		Rstream2_7.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream2_0.resetHead();
		Rstream2_1.resetHead();
		Rstream2_2.resetHead();
		Rstream2_3.resetHead();
		Rstream2_4.resetHead();
		Rstream2_5.resetHead();
		Rstream2_6.resetHead();
		Rstream2_7.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Rstream2_0;
	Consumer<streamData> Rstream2_1;
	Consumer<streamData> Rstream2_2;
	Consumer<streamData> Rstream2_3;
	Consumer<streamData> Rstream2_4;
	Consumer<streamData> Rstream2_5;
	Consumer<streamData> Rstream2_6;
	Consumer<streamData> Rstream2_7;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Rstream2_0.updatehead(8);
		Rstream2_1.updatehead(8);
		Rstream2_2.updatehead(8);
		Rstream2_3.updatehead(8);
		Rstream2_4.updatehead(8);
		Rstream2_5.updatehead(8);
		Rstream2_6.updatehead(8);
		Rstream2_7.updatehead(8);
	}
	void pushToken() {
		Out.updatetail(64);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<8;++i)		Out[j++]=Rstream2_0[i];
		for(i=0;i<8;++i)		Out[j++]=Rstream2_1[i];
		for(i=0;i<8;++i)		Out[j++]=Rstream2_2[i];
		for(i=0;i<8;++i)		Out[j++]=Rstream2_3[i];
		for(i=0;i<8;++i)		Out[j++]=Rstream2_4[i];
		for(i=0;i<8;++i)		Out[j++]=Rstream2_5[i];
		for(i=0;i<8;++i)		Out[j++]=Rstream2_6[i];
		for(i=0;i<8;++i)		Out[j++]=Rstream2_7[i];
	}
		pushToken();
		popToken();
	}
};
#endif