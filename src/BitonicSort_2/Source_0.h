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
	Source_0(Buffer<streamData>& KeySource):KeySource(KeySource){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		KeySource.resetTail();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		KeySource.resetTail();
	}
private:
	Producer<streamData> KeySource;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	static int A[8];
	int i;
	void popToken() {
	}
	void pushToken() {
		KeySource.updatetail(8);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		for(i=0;i<8;i++)		KeySource[i].x=A[i];
	}
		pushToken();
		popToken();
	}
};
int Source_0::A[8]={5,3,6,7,9,4,8,2};
#endif