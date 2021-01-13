#ifndef _Quantification_21_
#define _Quantification_21_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Quantification_21{
public:
	Quantification_21(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
		steadyScheduleCount = 256;
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
	int i,t;
	double d;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		Out.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	{
		i=0;
	}	}
	void work(){ 
	{
		i=i%64;
		d=In[0].x;
		d/=standard[i++];
		t=2+(int)floor(d+0.499999);
		Out[0].x=t;
	}
		pushToken();
		popToken();
	}
};
#endif