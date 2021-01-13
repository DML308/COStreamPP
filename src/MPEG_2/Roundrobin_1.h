#ifndef _Roundrobin_1_
#define _Roundrobin_1_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Roundrobin_1{
public:
	Roundrobin_1(Buffer<streamData>& round0_0,Buffer<streamData>& round0_1,Buffer<streamData>& round0_2,Buffer<streamData>& SourceOut):round0_0(round0_0),round0_1(round0_1),round0_2(round0_2),SourceOut(SourceOut){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		round0_0.resetTail();
		round0_1.resetTail();
		round0_2.resetTail();
		SourceOut.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		round0_0.resetTail();
		round0_1.resetTail();
		round0_2.resetTail();
		SourceOut.resetHead();
	}
private:
	Producer<streamData> round0_0;
	Producer<streamData> round0_1;
	Producer<streamData> round0_2;
	Consumer<streamData> SourceOut;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		SourceOut.updatehead(403);
	}
	void pushToken() {
		round0_0.updatetail(384);
		round0_1.updatetail(16);
		round0_2.updatetail(3);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<384;++i)		round0_0[i]=SourceOut[j++];
		for(i=0;i<16;++i)		round0_1[i]=SourceOut[j++];
		for(i=0;i<3;++i)		round0_2[i]=SourceOut[j++];
	}
		pushToken();
		popToken();
	}
};
#endif