#ifndef _ProcessFilter_175_
#define _ProcessFilter_175_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class ProcessFilter_175{
public:
	ProcessFilter_175(Buffer<streamData>& PF,Buffer<streamData>& BC):PF(PF),BC(BC){
		steadyScheduleCount = 1;
		initScheduleCount = 4;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		PF.resetTail();
		BC.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		PF.resetTail();
		BC.resetHead();
	}
private:
	Producer<streamData> PF;
	Consumer<streamData> BC;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	int M;
	int i;
	void popToken() {
		BC.updatehead(1);
	}
	void pushToken() {
		PF.updatetail(1);
	}
	void initVarAndState() {
	M=32;
	i=17;
	}
	void init(){ 
	}
	void work(){ 
	{
		PF[0].x=BC[0].x;
	}
		pushToken();
		popToken();
	}
};
#endif