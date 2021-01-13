#ifndef _Duplicate_219_
#define _Duplicate_219_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Duplicate_219{
public:
	Duplicate_219(Buffer<streamData>& dup36_0,Buffer<streamData>& dup36_1,Buffer<streamData>& In):dup36_0(dup36_0),dup36_1(dup36_1),In(In){
		steadyScheduleCount = 64;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		dup36_0.resetTail();
		dup36_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		dup36_0.resetTail();
		dup36_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> dup36_0;
	Producer<streamData> dup36_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		dup36_0.updatetail(1);
		dup36_1.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		dup36_0[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup36_1[i]=In[j++];
		j=0;
	}
		pushToken();
		popToken();
	}
};
#endif