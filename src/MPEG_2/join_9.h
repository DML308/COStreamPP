#ifndef _join_9_
#define _join_9_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_9{
public:
	join_9(Buffer<streamData>& OutSplit,Buffer<streamData>& Dstream0_0,Buffer<streamData>& Dstream0_1):OutSplit(OutSplit),Dstream0_0(Dstream0_0),Dstream0_1(Dstream0_1){
		steadyScheduleCount = 6;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		OutSplit.resetTail();
		Dstream0_0.resetHead();
		Dstream0_1.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		OutSplit.resetTail();
		Dstream0_0.resetHead();
		Dstream0_1.resetHead();
	}
private:
	Producer<streamData> OutSplit;
	Consumer<streamData> Dstream0_0;
	Consumer<streamData> Dstream0_1;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Dstream0_0.updatehead(64);
		Dstream0_1.updatehead(64);
	}
	void pushToken() {
		OutSplit.updatetail(128);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<64;++i)		OutSplit[j++]=Dstream0_0[i];
		for(i=0;i<64;++i)		OutSplit[j++]=Dstream0_1[i];
	}
		pushToken();
		popToken();
	}
};
#endif