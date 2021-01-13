#ifndef _FloatSource_0_
#define _FloatSource_0_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class FloatSource_0{
public:
	FloatSource_0(Buffer<streamData>& Source):Source(Source){
		steadyScheduleCount = 2;
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
		Source.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		Source[0].x=1;
	}
		pushToken();
		popToken();
	}
};
#endif