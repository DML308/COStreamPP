#ifndef _Sink_38_
#define _Sink_38_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Sink_38{
public:
	Sink_38(Buffer<streamData>& SBP):SBP(SBP){
		steadyScheduleCount = 450;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		SBP.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		SBP.resetHead();
	}
private:
	Consumer<streamData> SBP;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	int height;
	int i;
	int parse_or_process;
	int the_chroma_format;
	int width;
	void popToken() {
		SBP.updatehead(1);
	}
	void pushToken() {
	}
	void initVarAndState() {
	height=240;
	i=2;
	parse_or_process=5;
	the_chroma_format=1;
	width=352;
	}
	void init(){ 
	}
	void work(){ 
	{
		cout<<SBP[0].x<<endl;
	}
		pushToken();
		popToken();
	}
};
#endif