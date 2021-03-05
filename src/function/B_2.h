#ifndef _B_2_
#define _B_2_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class B_2{
public:
	B_2(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
		steadyScheduleCount = 20;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(3);
	}
	void pushToken() {
		Out.updatetail(3);
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