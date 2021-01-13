#ifndef _Adder_291_
#define _Adder_291_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Adder_291{
public:
	Adder_291(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
		steadyScheduleCount = 32;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	int M;
	void popToken() {
		In.updatehead(32);
	}
	void pushToken() {
		Out.updatetail(1);
	}
	void initVarAndState() {
	M=32;
	}
	void init(){ 
	}
	void work(){ 
	{
		double sum=0;
		int i;
		for(i=0;i<M;i++)		sum+=In[i].x;
		Out[0].x=sum;
	}
		pushToken();
		popToken();
	}
};
#endif