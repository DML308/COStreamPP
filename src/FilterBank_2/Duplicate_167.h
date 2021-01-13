#ifndef _Duplicate_167_
#define _Duplicate_167_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Duplicate_167{
public:
	Duplicate_167(Buffer<streamData>& dup17_0,Buffer<streamData>& dup17_1,Buffer<streamData>& In):dup17_0(dup17_0),dup17_1(dup17_1),In(In){
		steadyScheduleCount = 32;
		initScheduleCount = 127;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		dup17_0.resetTail();
		dup17_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		dup17_0.resetTail();
		dup17_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> dup17_0;
	Producer<streamData> dup17_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		dup17_0.updatetail(1);
		dup17_1.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		dup17_0[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup17_1[i]=In[j++];
		j=0;
	}
		pushToken();
		popToken();
	}
};
#endif