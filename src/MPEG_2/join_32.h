#ifndef _join_32_
#define _join_32_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_32{
public:
	join_32(Buffer<streamData>& Out,Buffer<streamData>& Rstream3_0,Buffer<streamData>& Rstream3_1,Buffer<streamData>& Rstream3_2,Buffer<streamData>& Rstream3_3,Buffer<streamData>& Rstream3_4,Buffer<streamData>& Rstream3_5,Buffer<streamData>& Rstream3_6,Buffer<streamData>& Rstream3_7):Out(Out),Rstream3_0(Rstream3_0),Rstream3_1(Rstream3_1),Rstream3_2(Rstream3_2),Rstream3_3(Rstream3_3),Rstream3_4(Rstream3_4),Rstream3_5(Rstream3_5),Rstream3_6(Rstream3_6),Rstream3_7(Rstream3_7){
		steadyScheduleCount = 48;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream3_0.resetHead();
		Rstream3_1.resetHead();
		Rstream3_2.resetHead();
		Rstream3_3.resetHead();
		Rstream3_4.resetHead();
		Rstream3_5.resetHead();
		Rstream3_6.resetHead();
		Rstream3_7.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream3_0.resetHead();
		Rstream3_1.resetHead();
		Rstream3_2.resetHead();
		Rstream3_3.resetHead();
		Rstream3_4.resetHead();
		Rstream3_5.resetHead();
		Rstream3_6.resetHead();
		Rstream3_7.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Rstream3_0;
	Consumer<streamData> Rstream3_1;
	Consumer<streamData> Rstream3_2;
	Consumer<streamData> Rstream3_3;
	Consumer<streamData> Rstream3_4;
	Consumer<streamData> Rstream3_5;
	Consumer<streamData> Rstream3_6;
	Consumer<streamData> Rstream3_7;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Rstream3_0.updatehead(1);
		Rstream3_1.updatehead(1);
		Rstream3_2.updatehead(1);
		Rstream3_3.updatehead(1);
		Rstream3_4.updatehead(1);
		Rstream3_5.updatehead(1);
		Rstream3_6.updatehead(1);
		Rstream3_7.updatehead(1);
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
		for(i=0;i<1;++i)		Out[j++]=Rstream3_0[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream3_1[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream3_2[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream3_3[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream3_4[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream3_5[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream3_6[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream3_7[i];
	}
		pushToken();
		popToken();
	}
};
#endif