#ifndef _Detector_43_
#define _Detector_43_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Detector_43{
public:
	Detector_43(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
		steadyScheduleCount = 1;
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
	int _myBeam;
	double cfarThreshold;
	int numSamples;
	int targetBeam;
	int targetSample;
	int curSample;
	int myBeam;
	int holdsTarget;
	double thresh;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		Out.updatetail(1);
	}
	void initVarAndState() {
	_myBeam=0;
	cfarThreshold=32;
	numSamples=128;
	targetBeam=182.400000;
	targetSample=1;
	}
	void init(){ 
	{
		curSample=0;
		if(_myBeam==targetBeam)	holdsTarget=1;else	holdsTarget=0;
		myBeam=_myBeam+1;
		thresh=0.100000;
	}	}
	void work(){ 
	{
		double inputVal=In[0].x;
		double outputVal;
		if(holdsTarget==1&&targetSample==curSample)		{
		if(!(inputVal>=thresh))		{
		outputVal=0;
	}else		{
		outputVal=myBeam;
	}
	}else		{
		if(!(inputVal>=thresh))		{
		outputVal=0;
	}else		{
		outputVal=-myBeam;
	}
	}
		outputVal=inputVal;
		Out[0].x=outputVal;
		curSample++;
		if(curSample>=numSamples)	curSample=0;
	}
		pushToken();
		popToken();
	}
};
#endif