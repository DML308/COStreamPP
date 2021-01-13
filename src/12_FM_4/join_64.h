#ifndef _join_64_
#define _join_64_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_64{
public:
	join_64(Buffer<streamData>& Out,Buffer<streamData>& Dstream0_0,Buffer<streamData>& Dstream0_1,Buffer<streamData>& Dstream0_2,Buffer<streamData>& Dstream0_3,Buffer<streamData>& Dstream0_4,Buffer<streamData>& Dstream0_5,Buffer<streamData>& Dstream0_6,Buffer<streamData>& Dstream0_7,Buffer<streamData>& Dstream0_8,Buffer<streamData>& Dstream0_9):Out(Out),Dstream0_0(Dstream0_0),Dstream0_1(Dstream0_1),Dstream0_2(Dstream0_2),Dstream0_3(Dstream0_3),Dstream0_4(Dstream0_4),Dstream0_5(Dstream0_5),Dstream0_6(Dstream0_6),Dstream0_7(Dstream0_7),Dstream0_8(Dstream0_8),Dstream0_9(Dstream0_9){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream0_0.resetHead();
		Dstream0_1.resetHead();
		Dstream0_2.resetHead();
		Dstream0_3.resetHead();
		Dstream0_4.resetHead();
		Dstream0_5.resetHead();
		Dstream0_6.resetHead();
		Dstream0_7.resetHead();
		Dstream0_8.resetHead();
		Dstream0_9.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream0_0.resetHead();
		Dstream0_1.resetHead();
		Dstream0_2.resetHead();
		Dstream0_3.resetHead();
		Dstream0_4.resetHead();
		Dstream0_5.resetHead();
		Dstream0_6.resetHead();
		Dstream0_7.resetHead();
		Dstream0_8.resetHead();
		Dstream0_9.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Dstream0_0;
	Consumer<streamData> Dstream0_1;
	Consumer<streamData> Dstream0_2;
	Consumer<streamData> Dstream0_3;
	Consumer<streamData> Dstream0_4;
	Consumer<streamData> Dstream0_5;
	Consumer<streamData> Dstream0_6;
	Consumer<streamData> Dstream0_7;
	Consumer<streamData> Dstream0_8;
	Consumer<streamData> Dstream0_9;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Dstream0_0.updatehead(1);
		Dstream0_1.updatehead(1);
		Dstream0_2.updatehead(1);
		Dstream0_3.updatehead(1);
		Dstream0_4.updatehead(1);
		Dstream0_5.updatehead(1);
		Dstream0_6.updatehead(1);
		Dstream0_7.updatehead(1);
		Dstream0_8.updatehead(1);
		Dstream0_9.updatehead(1);
	}
	void pushToken() {
		Out.updatetail(10);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		Out[j++]=Dstream0_0[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_1[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_2[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_3[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_4[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_5[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_6[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_7[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_8[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_9[i];
	}
		pushToken();
		popToken();
	}
};
#endif