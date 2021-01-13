#ifndef _IntSource_0_
#define _IntSource_0_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class IntSource_0{
public:
	IntSource_0(Buffer<streamData>& Out):Out(Out){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
	}
private:
	Producer<streamData> Out;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
	}
	void pushToken() {
		Out.updatetail(256);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int a;
		int i,j;
		int k=0;
		double ddd=(double)k;
		for(i=0;i<16;++i)			{
		for(j=0;j<16;++j)			{
		Out[k].x=Mutrix[i][j]-128;
		k++;
	}
	}
	}
		pushToken();
		popToken();
	}
};
#endif