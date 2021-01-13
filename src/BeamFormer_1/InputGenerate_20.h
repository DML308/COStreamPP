#ifndef _InputGenerate_20_
#define _InputGenerate_20_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class InputGenerate_20{
public:
	InputGenerate_20(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
		steadyScheduleCount = 2;
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
	int myChannel;
	int numberOfSamples;
	int tarBeam;
	int targetSample;
	double thresh;
	int curSample;
	int holdsTarget;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		Out.updatetail(2);
	}
	void initVarAndState() {
	myChannel=6;
	numberOfSamples=256;
	tarBeam=1;
	targetSample=64;
	thresh=182.400000;
	}
	void init(){ 
	{
		curSample=0;
		if(tarBeam==myChannel)	holdsTarget=1;else	holdsTarget=0;
	}	}
	void work(){ 
	{
		double tmp=curSample*myChannel;
		if(holdsTarget==1&&(curSample==targetSample))		{
		Out[0].x=sqrt(tmp);
		Out[1].x=sqrt(tmp)+1;
	}else		{
		Out[0].x=-sqrt(tmp);
		Out[1].x=-(sqrt(tmp)+1);
	}
		curSample++;
		if(curSample>=numberOfSamples)		{
		curSample=0;
	}
	}
		pushToken();
		popToken();
	}
};
#endif