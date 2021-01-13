#ifndef _Duplicate_579_
#define _Duplicate_579_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Duplicate_579{
public:
	Duplicate_579(Buffer<streamData>& dup96_0,Buffer<streamData>& dup96_1,Buffer<streamData>& In):dup96_0(dup96_0),dup96_1(dup96_1),In(In){
		steadyScheduleCount = 64;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		dup96_0.resetTail();
		dup96_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		dup96_0.resetTail();
		dup96_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> dup96_0;
	Producer<streamData> dup96_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		dup96_0.updatetail(1);
		dup96_1.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		dup96_0[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup96_1[i]=In[j++];
		j=0;
	}
		pushToken();
		popToken();
	}
};
#endif