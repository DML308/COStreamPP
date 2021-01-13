#ifndef _AssignmentX_1_
#define _AssignmentX_1_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class AssignmentX_1{
public:
	AssignmentX_1(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
		steadyScheduleCount = 1;
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
	int N;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		Out.updatetail(16);
	}
	void initVarAndState() {
	N=8;
	}
	void init(){ 
	}
	void work(){ 
	{
		int i;
		cout<<"\nOut:"<<endl;
		for(i=0;i<N*2;i++)			{
		Out[i].x=In[0].x+i;
		cout<<Out[i].x;
	}
		cout<<"\n"<<endl;
	}
		pushToken();
		popToken();
	}
};
#endif