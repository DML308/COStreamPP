#ifndef _Duplicate_507_
#define _Duplicate_507_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Duplicate_507{
public:
	Duplicate_507(Buffer<streamData>& dup84_0,Buffer<streamData>& dup84_1,Buffer<streamData>& In):dup84_0(dup84_0),dup84_1(dup84_1),In(In){
		steadyScheduleCount = 64;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		dup84_0.resetTail();
		dup84_1.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		dup84_0.resetTail();
		dup84_1.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> dup84_0;
	Producer<streamData> dup84_1;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		dup84_0.updatetail(1);
		dup84_1.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		dup84_0[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup84_1[i]=In[j++];
		j=0;
	}
		pushToken();
		popToken();
	}
};
#endif