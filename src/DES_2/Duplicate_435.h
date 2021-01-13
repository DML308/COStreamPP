#ifndef _Duplicate_435_
#define _Duplicate_435_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Duplicate_435{
public:
	Duplicate_435(Buffer<streamData>& dup72_0,Buffer<streamData>& dup72_1,Buffer<streamData>& In):dup72_0(dup72_0),dup72_1(dup72_1),In(In){
		steadyScheduleCount = 64;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		dup72_0.resetTail();
		dup72_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		dup72_0.resetTail();
		dup72_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> dup72_0;
	Producer<streamData> dup72_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		dup72_0.updatetail(1);
		dup72_1.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		dup72_0[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup72_1[i]=In[j++];
		j=0;
	}
		pushToken();
		popToken();
	}
};
#endif