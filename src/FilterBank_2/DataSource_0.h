#ifndef _DataSource_0_
#define _DataSource_0_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class DataSource_0{
public:
	DataSource_0(Buffer<streamData>& Source):Source(Source){
		steadyScheduleCount = 32;
		initScheduleCount = 382;
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
	int n;
	double w1;
	double w2;
	double w3;
	void popToken() {
	}
	void pushToken() {
		Source.updatetail(1);
	}
	void initVarAndState() {
		 n=0;
		 w1=pi/10.000000;
		 w2=pi/20.000000;
		 w3=pi/30.000000;
	}
	void init(){ 
	}
	void work(){ 
	{
		Source[0].x=(cos(w1*n)+cos(w2*n)+cos(w3*n));
		n++;
	}
		pushToken();
		popToken();
	}
};
#endif