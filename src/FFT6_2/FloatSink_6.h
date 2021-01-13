#ifndef _FloatSink_6_
#define _FloatSink_6_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class FloatSink_6{
public:
	FloatSink_6(Buffer<streamData>& CDFT):CDFT(CDFT){
		steadyScheduleCount = 16;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		CDFT.resetHead();
	}
	timespec diff(timespec start,timespec end)
	{
		timespec temp;
		if((end.tv_nsec-start.tv_nsec)<0)
		{
			temp.tv_sec=end.tv_sec-start.tv_sec-1;
			temp.tv_nsec=1000000000+end.tv_nsec-start.tv_nsec;
		}
		else
		{
			temp.tv_sec=end.tv_sec-start.tv_sec;
			temp.tv_nsec=end.tv_nsec-start.tv_nsec;
		}
		return temp;
		
	};
	void runSteadyScheduleWork() {
		timespec time1,time2;
		timespec time3,time4;
		clock_gettime(CLOCK_REALTIME,&time1);
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		clock_gettime(CLOCK_REALTIME,&time2);
		timespec tmp=diff(time1,time2);
		workTime6.tv_nsec+=tmp.tv_nsec;
		int sec_add=workTime6.tv_nsec/1000000000;
		if(sec_add>0)
		{
			workTime6.tv_sec+=(tmp.tv_sec+sec_add);
			workTime6.tv_nsec=workTime6.tv_nsec%1000000000;
		}
		else
		{
			workTime6.tv_sec+=tmp.tv_sec;
		}
		clock_gettime(CLOCK_REALTIME,&time3);
		CDFT.resetHead();
		clock_gettime(CLOCK_REALTIME,&time4);
		timespec temp0=diff(time3,time4);
		worktime6.tv_nsec+=temp0.tv_nsec;
		int sec=worktime6.tv_nsec/1000000000;
		if(sec>0)
		{
			worktime6.tv_sec+=temp0.tv_sec+sec;
			worktime6.tv_nsec=worktime6.tv_nsec%1000000000;
		}
		else
		{
			worktime6.tv_sec+=temp0.tv_sec;
		}
	}
private:
	Consumer<streamData> CDFT;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	int N;
	void popToken() {
		CDFT.updatehead(1);
	}
	void pushToken() {
	}
	void initVarAndState() {
	N=8;
	}
	void init(){ 
	}
	void work(){ 
	{
		cout<<CDFT[0].x<<endl;
	}
		pushToken();
		popToken();
	}
};
#endif