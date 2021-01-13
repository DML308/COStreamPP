#ifndef _join_37_
#define _join_37_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_37{
public:
	join_37(Buffer<streamData>& SBP,Buffer<streamData>& Rstream0_0,Buffer<streamData>& Rstream0_1,Buffer<streamData>& Rstream0_2):SBP(SBP),Rstream0_0(Rstream0_0),Rstream0_1(Rstream0_1),Rstream0_2(Rstream0_2){
		steadyScheduleCount = 6;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		SBP.resetTail();
		Rstream0_0.resetHead();
		Rstream0_1.resetHead();
		Rstream0_2.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		SBP.resetTail();
		Rstream0_0.resetHead();
		Rstream0_1.resetHead();
		Rstream0_2.resetHead();
	}
private:
	Producer<streamData> SBP;
	Consumer<streamData> Rstream0_0;
	Consumer<streamData> Rstream0_1;
	Consumer<streamData> Rstream0_2;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Rstream0_0.updatehead(64);
		Rstream0_1.updatehead(8);
		Rstream0_2.updatehead(3);
	}
	void pushToken() {
		SBP.updatetail(75);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<64;++i)		SBP[j++]=Rstream0_0[i];
		for(i=0;i<8;++i)		SBP[j++]=Rstream0_1[i];
		for(i=0;i<3;++i)		SBP[j++]=Rstream0_2[i];
	}
		pushToken();
		popToken();
	}
};
#endif