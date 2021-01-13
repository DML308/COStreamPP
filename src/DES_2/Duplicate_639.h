#ifndef _Duplicate_639_
#define _Duplicate_639_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Duplicate_639{
public:
	Duplicate_639(Buffer<streamData>& dup106_0,Buffer<streamData>& dup106_1,Buffer<streamData>& In):dup106_0(dup106_0),dup106_1(dup106_1),In(In){
		steadyScheduleCount = 64;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		dup106_0.resetTail();
		dup106_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		dup106_0.resetTail();
		dup106_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> dup106_0;
	Producer<streamData> dup106_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		dup106_0.updatetail(1);
		dup106_1.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		dup106_0[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup106_1[i]=In[j++];
		j=0;
	}
		pushToken();
		popToken();
	}
};
#endif