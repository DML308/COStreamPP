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
	size=4;
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=1,j=1,k=0,x=2;
		K>>=1;
		double res[2*size];
		x|=(i&1);
		if(k==5)	k++;
		for(i=x;i<size;i+=1)			{
		if(i==0)	k+=1;
	}
		x==1?i++:j++;
	}
		pushToken();
		popToken();
	}
};
#endif