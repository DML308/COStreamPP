#ifndef _join_38_
#define _join_38_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_38{
public:
	join_38(Buffer<streamData>& Out,Buffer<streamData>& Dstream0_0,Buffer<streamData>& Dstream0_1,Buffer<streamData>& Dstream0_2,Buffer<streamData>& Dstream0_3,Buffer<streamData>& Dstream0_4,Buffer<streamData>& Dstream0_5,Buffer<streamData>& Dstream0_6,Buffer<streamData>& Dstream0_7,Buffer<streamData>& Dstream0_8,Buffer<streamData>& Dstream0_9,Buffer<streamData>& Dstream0_10,Buffer<streamData>& Dstream0_11):Out(Out),Dstream0_0(Dstream0_0),Dstream0_1(Dstream0_1),Dstream0_2(Dstream0_2),Dstream0_3(Dstream0_3),Dstream0_4(Dstream0_4),Dstream0_5(Dstream0_5),Dstream0_6(Dstream0_6),Dstream0_7(Dstream0_7),Dstream0_8(Dstream0_8),Dstream0_9(Dstream0_9),Dstream0_10(Dstream0_10),Dstream0_11(Dstream0_11){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream0_0.resetHead();
		Dstream0_1.resetHead();
		Dstream0_2.resetHead();
		Dstream0_3.resetHead();
		Dstream0_4.resetHead();
		Dstream0_5.resetHead();
		Dstream0_6.resetHead();
		Dstream0_7.resetHead();
		Dstream0_8.resetHead();
		Dstream0_9.resetHead();
		Dstream0_10.resetHead();
		Dstream0_11.resetHead();
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
	void runSteadyScheduleWork() {
		timespec time1,time2;
		for(int i=0;i<steadyScheduleCount;i++)
			work();

		
		Out.resetTail();
		
		clock_gettime(CLOCK_REALTIME,&time1);
		Dstream0_0.resetHead();
		clock_gettime(CLOCK_REALTIME,&time2);
		Dstream0_1.resetHead();
		Dstream0_2.resetHead();
		Dstream0_3.resetHead();
		Dstream0_4.resetHead();
		Dstream0_5.resetHead();
		Dstream0_6.resetHead();
		Dstream0_7.resetHead();
		Dstream0_8.resetHead();
		Dstream0_9.resetHead();
		Dstream0_10.resetHead();
		Dstream0_11.resetHead();
		
		timespec temp=diff(time1,time2);
		worktime2.tv_nsec+=temp.tv_nsec;
		int sec=worktime2.tv_nsec/1000000000;
		if(sec>0)
		{
			worktime2.tv_sec+=temp.tv_sec+sec;
			worktime2.tv_nsec=worktime2.tv_nsec%1000000000;
		}
		else
		{
			worktime2.tv_sec+=temp.tv_sec;
		}
		
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Dstream0_0;
	Consumer<streamData> Dstream0_1;
	Consumer<streamData> Dstream0_2;
	Consumer<streamData> Dstream0_3;
	Consumer<streamData> Dstream0_4;
	Consumer<streamData> Dstream0_5;
	Consumer<streamData> Dstream0_6;
	Consumer<streamData> Dstream0_7;
	Consumer<streamData> Dstream0_8;
	Consumer<streamData> Dstream0_9;
	Consumer<streamData> Dstream0_10;
	Consumer<streamData> Dstream0_11;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Dstream0_0.updatehead(2);
		Dstream0_1.updatehead(2);
		Dstream0_2.updatehead(2);
		Dstream0_3.updatehead(2);
		Dstream0_4.updatehead(2);
		Dstream0_5.updatehead(2);
		Dstream0_6.updatehead(2);
		Dstream0_7.updatehead(2);
		Dstream0_8.updatehead(2);
		Dstream0_9.updatehead(2);
		Dstream0_10.updatehead(2);
		Dstream0_11.updatehead(2);
	}
	void pushToken() {
		Out.updatetail(24);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<2;++i)		Out[j++]=Dstream0_0[i];
		for(i=0;i<2;++i)		Out[j++]=Dstream0_1[i];
		for(i=0;i<2;++i)		Out[j++]=Dstream0_2[i];
		for(i=0;i<2;++i)		Out[j++]=Dstream0_3[i];
		for(i=0;i<2;++i)		Out[j++]=Dstream0_4[i];
		for(i=0;i<2;++i)		Out[j++]=Dstream0_5[i];
		for(i=0;i<2;++i)		Out[j++]=Dstream0_6[i];
		for(i=0;i<2;++i)		Out[j++]=Dstream0_7[i];
		for(i=0;i<2;++i)		Out[j++]=Dstream0_8[i];
		for(i=0;i<2;++i)		Out[j++]=Dstream0_9[i];
		for(i=0;i<2;++i)		Out[j++]=Dstream0_10[i];
		for(i=0;i<2;++i)		Out[j++]=Dstream0_11[i];
	}
		pushToken();
		popToken();
	}
};
#endif