#ifndef _FloatSource_0_
#define _FloatSource_0_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
#include "sys/time.h"
using namespace std;



class FloatSource_0{

	long long getCurrentTime(){//用于获取系统微秒时间
    struct timeval tv; 
    gettimeofday(&tv,NULL);
    return tv.tv_sec*1000*1000+tv.tv_usec;
}
public:
	
	

	FloatSource_0(Buffer<streamData>& Source):Source(Source){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Source.resetTail();
	}

	void runSteadyScheduleWork() {
		timespec time1,time2;
		timespec time3,time4;
		int temp=0;
		clock_gettime(CLOCK_REALTIME,&time1);
		long long t1=getCurrentTime();
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		long long t2=getCurrentTime();
		temp=(t2-t1)*3100;
		clks1+=temp;
		clock_gettime(CLOCK_REALTIME,&time2);
		timespec tmp=diff(time1,time2);
		workTime0.tv_nsec+=tmp.tv_nsec;
		int sec_add=workTime0.tv_nsec/1000000000;
		if(sec_add>0)
		{
			workTime0.tv_sec+=(tmp.tv_sec+sec_add);
			workTime0.tv_nsec=workTime0.tv_nsec%1000000000;
		}
		else
		{
			workTime0.tv_sec+=tmp.tv_sec;
		}
		//cout<<"the time & clocks of FloatSource_0: "<<(t2-t1)*3100<<endl;
		//cout<<"time passed:"<<"FFTReorderSimple:"<<(time2.tv_sec-time1.tv_sec)*1000<<"ms"<<(time2.tv_nsec - time1.tv_nsec)<< "ns" <<endl;
		//cout<<"time passed:"<<(time2.tv_sec-time1.tv_sec)*1000+(time2.tv_nsec - time1.tv_nsec)/1000000 << "ms" <<endl;
		
		
		clock_gettime(CLOCK_REALTIME,&time3);
		Source.resetTail();
		clock_gettime(CLOCK_REALTIME,&time4);
		timespec temp0=diff(time3,time4);
		worktime0.tv_nsec+=temp0.tv_nsec;
		int sec=worktime0.tv_nsec/1000000000;
		if(sec>0)
		{
			worktime0.tv_sec+=temp0.tv_sec+sec;
			worktime0.tv_nsec=worktime0.tv_nsec%1000000000;
		}
		else
		{
			worktime0.tv_sec+=temp0.tv_sec;
		}
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
		
	}
private:
	Producer<streamData> Source;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	int N;
	double max;
	double current;
	void popToken() {
	}
	void pushToken() {
		Source.updatetail(1);
	}
	void initVarAndState() {
	N=8;
		 max=1000.000000;
		 current=0.000000;
	}
	void init(){ 
	}
	void work(){ 
	{
		if(current>max)	current=0.000000;
		Source[0].x=current;
		current+=2*N;
	}
		pushToken();
		popToken();
	}
};
#endif