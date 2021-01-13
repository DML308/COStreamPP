#ifndef _join_99_
#define _join_99_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_99{
public:
	join_99(Buffer<streamData>& Out,Buffer<streamData>& Dstream11_0,Buffer<streamData>& Dstream11_1):Out(Out),Dstream11_0(Dstream11_0),Dstream11_1(Dstream11_1){
		steadyScheduleCount = 32;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream11_0.resetHead();
		Dstream11_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream11_0.resetHead();
		Dstream11_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Dstream11_0;
	Consumer<streamData> Dstream11_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Dstream11_0.updatehead(1);
		Dstream11_1.updatehead(1);
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
		for(i=0;i<1;++i)		Out[j++]=Dstream11_0[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream11_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif