#ifndef _A_1_
#define _A_1_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class A_1{
public:
	A_1(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
		steadyScheduleCount = 12;
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
	int size;
	void popToken() {
		In.updatehead(3);
	}
	void pushToken() {
		Out.updatetail(5);
	}
	void initVarAndState() {
	size=8;
	}
	void init(){ 
	}
	void work(){ 
	{
		int x=1;
		int y;
		func();
	}
		pushToken();
		popToken();
	}
};
#endif