#ifndef _Xor_480_
#define _Xor_480_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Xor_480{
public:
	Xor_480(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
		steadyScheduleCount = 32;
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
	int n;
	void popToken() {
		In.updatehead(2);
	}
	void pushToken() {
		Out.updatetail(1);
	}
	void initVarAndState() {
	n=2;
	}
	void init(){ 
	}
	void work(){ 
	{
		int x=In[0].x;
		int i;
		for(i=1;i<n;i++)			{
		int y=In[i].x;
		x=x^y;
	}
		Out[0].x=x;
	}
		pushToken();
		popToken();
	}
};
#endif