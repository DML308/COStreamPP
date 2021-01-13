#ifndef _join_719_
#define _join_719_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_719{
public:
	join_719(Buffer<streamData>& Out,Buffer<streamData>& Rstream59_0,Buffer<streamData>& Rstream59_1):Out(Out),Rstream59_0(Rstream59_0),Rstream59_1(Rstream59_1){
		steadyScheduleCount = 32;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream59_0.resetHead();
		Rstream59_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Rstream59_0.resetHead();
		Rstream59_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Rstream59_0;
	Consumer<streamData> Rstream59_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Rstream59_0.updatehead(1);
		Rstream59_1.updatehead(1);
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
		for(i=0;i<1;++i)		Out[j++]=Rstream59_0[i];
		for(i=0;i<1;++i)		Out[j++]=Rstream59_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif