#ifndef _join_20_
#define _join_20_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_20{
public:
	join_20(Buffer<streamData>& Out,Buffer<streamData>& Rstream1_0,Buffer<streamData>& Rstream1_1,Buffer<streamData>& Rstream1_2,Buffer<streamData>& Rstream1_3,Buffer<streamData>& Rstream1_4,Buffer<streamData>& Rstream1_5,Buffer<streamData>& Rstream1_6,Buffer<streamData>& Rstream1_7):Out(Out),Rstream1_0(Rstream1_0),Rstream1_1(Rstream1_1),Rstream1_2(Rstream1_2),Rstream1_3(Rstream1_3),Rstream1_4(Rstream1_4),Rstream1_5(Rstream1_5),Rstream1_6(Rstream1_6),Rstream1_7(Rstream1_7){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream1_0.resetHead();
		Rstream1_1.resetHead();
		Rstream1_2.resetHead();
		Rstream1_3.resetHead();
		Rstream1_4.resetHead();
		Rstream1_5.resetHead();
		Rstream1_6.resetHead();
		Rstream1_7.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream1_0.resetHead();
		Rstream1_1.resetHead();
		Rstream1_2.resetHead();
		Rstream1_3.resetHead();
		Rstream1_4.resetHead();
		Rstream1_5.resetHead();
		Rstream1_6.resetHead();
		Rstream1_7.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Rstream1_0;
	Consumer<streamData> Rstream1_1;
	Consumer<streamData> Rstream1_2;
	Consumer<streamData> Rstream1_3;
	Consumer<streamData> Rstream1_4;
	Consumer<streamData> Rstream1_5;
	Consumer<streamData> Rstream1_6;
	Consumer<streamData> Rstream1_7;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Rstream1_0.updatehead(8);
		Rstream1_1.updatehead(8);
		Rstream1_2.updatehead(8);
		Rstream1_3.updatehead(8);
		Rstream1_4.updatehead(8);
		Rstream1_5.updatehead(8);
		Rstream1_6.updatehead(8);
		Rstream1_7.updatehead(8);
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
		for(i=0;i<8;++i)		Out[j++]=Rstream1_0[i];
		for(i=0;i<8;++i)		Out[j++]=Rstream1_1[i];
		for(i=0;i<8;++i)		Out[j++]=Rstream1_2[i];
		for(i=0;i<8;++i)		Out[j++]=Rstream1_3[i];
		for(i=0;i<8;++i)		Out[j++]=Rstream1_4[i];
		for(i=0;i<8;++i)		Out[j++]=Rstream1_5[i];
		for(i=0;i<8;++i)		Out[j++]=Rstream1_6[i];
		for(i=0;i<8;++i)		Out[j++]=Rstream1_7[i];
	}
		pushToken();
		popToken();
	}
};
#endif