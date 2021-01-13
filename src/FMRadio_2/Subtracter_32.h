#ifndef _Subtracter_32_
#define _Subtracter_32_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Subtracter_32{
public:
	Subtracter_32(Buffer<streamData>& Out,Buffer<streamData>& Band):Out(Out),Band(Band){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Band.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Band.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Band;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	float high;
	float low;
	float rate;
	int taps;
	void popToken() {
		Band.updatehead(2);
	}
	void pushToken() {
		Out.updatetail(1);
	}
	void initVarAndState() {
	high=311.127014;
	low=220.000015;
	rate=250000000.000000;
	taps=64;
	}
	void init(){ 
	}
	void work(){ 
	{
		Out[0].x=Band[1].x-Band[0].x;
	}
		pushToken();
		popToken();
	}
};
#endif