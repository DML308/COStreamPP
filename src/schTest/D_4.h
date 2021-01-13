#ifndef _D_4_
#define _D_4_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class D_4{
public:
	D_4(Buffer<streamData>& CDFT):CDFT(CDFT){
		steadyScheduleCount = 3;
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
	void popToken() {
		CDFT.updatehead(2);
	}
	void pushToken() {
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