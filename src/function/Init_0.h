#ifndef _Init_0_
#define _Init_0_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Init_0{
public:
	Init_0(Buffer<streamData>& Source):Source(Source){
		steadyScheduleCount = 18;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Source.resetTail();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Source.resetTail();
	}
private:
	Producer<streamData> Source;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
	}
	void pushToken() {
		Source.updatetail(2);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
	}
		pushToken();
		popToken();
	}
};
#endif