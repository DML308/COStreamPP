#ifndef _InverseQuantizationJoinerSubstitute_10_
#define _InverseQuantizationJoinerSubstitute_10_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class InverseQuantizationJoinerSubstitute_10{
public:
	InverseQuantizationJoinerSubstitute_10(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
		steadyScheduleCount = 6;
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
	int macroblock_intra;
	void popToken() {
		In.updatehead(128);
	}
	void pushToken() {
		Out.updatetail(64);
	}
	void initVarAndState() {
	}
	void init(){ 
	{
		macroblock_intra=1;
	}	}
	void work(){ 
	{
		int i;
		if(macroblock_intra==-1)		{
		cout<<"  Error: macroblock_intra should not be -1, should have recieved update message"<<endl;
	}else	if(macroblock_intra==1)		{
		for(i=0;i<64;i++)			{
		Out[i].x=In[i].x;
	}
	}else		{
		for(i=0;i<64;i++)			{
		Out[i].x=In[i+64].x;
	}
	}
	}
		pushToken();
		popToken();
	}
};
#endif