#ifndef _Sink_47_
#define _Sink_47_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Sink_47{
public:
	Sink_47(Buffer<streamData>& BSK):BSK(BSK){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		BSK.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		BSK.resetHead();
	}
private:
	Consumer<streamData> BSK;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	int i;
	void popToken() {
		BSK.updatehead(8);
	}
	void pushToken() {
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		for(i=0;i<8;i++)		cout<<BSK[i].x<<endl;
	}
		pushToken();
		popToken();
	}
};
#endif