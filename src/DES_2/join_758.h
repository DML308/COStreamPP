#ifndef _join_758_
#define _join_758_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_758{
public:
	join_758(Buffer<streamData>& Out,Buffer<streamData>& Dstream62_0,Buffer<streamData>& Dstream62_1):Out(Out),Dstream62_0(Dstream62_0),Dstream62_1(Dstream62_1){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream62_0.resetHead();
		Dstream62_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream62_0.resetHead();
		Dstream62_1.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Dstream62_0;
	Consumer<streamData> Dstream62_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Dstream62_0.updatehead(32);
		Dstream62_1.updatehead(32);
	}
	void pushToken() {
		Out.updatetail(64);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<32;++i)		Out[j++]=Dstream62_0[i];
		for(i=0;i<32;++i)		Out[j++]=Dstream62_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif