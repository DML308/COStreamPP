#ifndef _FloatSink_324_
#define _FloatSink_324_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class FloatSink_324{
public:
	FloatSink_324(Buffer<streamData>& FBP):FBP(FBP){
		steadyScheduleCount = 32;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		FBP.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		FBP.resetHead();
	}
private:
	Consumer<streamData> FBP;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		FBP.updatehead(1);
	}
	void pushToken() {
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		cout<<FBP[0].x<<endl;
	}
		pushToken();
		popToken();
	}
};
#endif