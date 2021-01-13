#ifndef _FloatSink_6_
#define _FloatSink_6_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class FloatSink_6{
public:
	FloatSink_6(Buffer<streamData>& CDFT):CDFT(CDFT){
		steadyScheduleCount = 16;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		CDFT.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		CDFT.resetHead();
	}
private:
	Consumer<streamData> CDFT;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	int N;
	void popToken() {
		CDFT.updatehead(1);
	}
	void pushToken() {
	}
	void initVarAndState() {
	N=8;
	}
	void init(){ 
	}
	void work(){ 
	{
		cout<<CDFT[0].x<<endl;
	}
		pushToken();
		popToken();
	}
};
#endif