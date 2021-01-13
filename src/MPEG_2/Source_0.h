#ifndef _Source_0_
#define _Source_0_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Source_0{
public:
	Source_0(Buffer<streamData>& SourceOut):SourceOut(SourceOut){
		steadyScheduleCount = 403;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		SourceOut.resetTail();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		SourceOut.resetTail();
	}
private:
	Producer<streamData> SourceOut;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	int height;
	int i;
	int parse_or_process;
	int the_chroma_format;
	int width;
	int num;
	void popToken() {
	}
	void pushToken() {
		SourceOut.updatetail(1);
	}
	void initVarAndState() {
	height=240;
	i=2;
	parse_or_process=5;
	the_chroma_format=1;
	width=352;
		 num=0;
	}
	void init(){ 
	}
	void work(){ 
	{
		SourceOut[0].x=source[num];
		num++;
		num=num%403;
	}
		pushToken();
		popToken();
	}
};
#endif