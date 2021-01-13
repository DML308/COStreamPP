#ifndef _join_46_
#define _join_46_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_46{
public:
	join_46(Buffer<streamData>& Out,Buffer<streamData>& Rstream10_0,Buffer<streamData>& Rstream10_1,Buffer<streamData>& Rstream10_2,Buffer<streamData>& Rstream10_3):Out(Out),Rstream10_0(Rstream10_0),Rstream10_1(Rstream10_1),Rstream10_2(Rstream10_2),Rstream10_3(Rstream10_3){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream10_0.resetHead();
		Rstream10_1.resetHead();
		Rstream10_2.resetHead();
		Rstream10_3.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream10_0.resetHead();
		Rstream10_1.resetHead();
		Rstream10_2.resetHead();
		Rstream10_3.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Rstream10_0;
	Consumer<streamData> Rstream10_1;
	Consumer<streamData> Rstream10_2;
	Consumer<streamData> Rstream10_3;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Rstream10_0.updatehead(2);
		Rstream10_1.updatehead(2);
		Rstream10_2.updatehead(2);
		Rstream10_3.updatehead(2);
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
		for(i=0;i<2;++i)		Out[j++]=Rstream10_0[i];
		for(i=0;i<2;++i)		Out[j++]=Rstream10_1[i];
		for(i=0;i<2;++i)		Out[j++]=Rstream10_2[i];
		for(i=0;i<2;++i)		Out[j++]=Rstream10_3[i];
	}
		pushToken();
		popToken();
	}
};
#endif