#ifndef _Source_0_
#define _Source_0_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Source_0{
public:
	Source_0(Buffer<streamData>& FloatOneSource):FloatOneSource(FloatOneSource){
		steadyScheduleCount = 5;
		initScheduleCount = 379;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		FloatOneSource.resetTail();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		FloatOneSource.resetTail();
	}
private:
	Producer<streamData> FloatOneSource;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	float x;
	void popToken() {
	}
	void pushToken() {
		FloatOneSource.updatetail(1);
	}
	void initVarAndState() {
		 x=0;
	}
	void init(){ 
	}
	void work(){ 
	{
		FloatOneSource[0].x=x;
		x+=1.000000;
	}
		pushToken();
		popToken();
	}
};
#endif