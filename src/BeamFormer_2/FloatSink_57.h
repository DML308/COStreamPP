#ifndef _FloatSink_57_
#define _FloatSink_57_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class FloatSink_57{
public:
	FloatSink_57(Buffer<streamData>& Out_2):Out_2(Out_2){
		steadyScheduleCount = 4;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out_2.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out_2.resetHead();
	}
private:
	Consumer<streamData> Out_2;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Out_2.updatehead(1);
	}
	void pushToken() {
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		cout<<Out_2[0].x<<endl;
	}
		pushToken();
		popToken();
	}
};
#endif