#ifndef _CombineDFTX_5_
#define _CombineDFTX_5_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class CombineDFTX_5{
public:
	CombineDFTX_5(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
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
		timespec time1,time2,time3,time4;
		clock_gettime(CLOCK_REALTIME,&time1);
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		clock_gettime(CLOCK_REALTIME,&time2);
		timespec tmp=diff(time1,time2);
		workTime5.tv_nsec+=tmp.tv_nsec;
		int sec_add=workTime5.tv_nsec/1000000000;
		if(sec_add>0)
		{
			workTime5.tv_sec+=(tmp.tv_sec+sec_add);
			workTime5.tv_nsec=workTime5.tv_nsec%1000000000;
		}
		else
		{
			workTime5.tv_sec+=tmp.tv_sec;
		}
		
		clock_gettime(CLOCK_REALTIME,&time3);
		Out.resetTail();
		In.resetHead();
		clock_gettime(CLOCK_REALTIME,&time4);
		timespec temp=diff(time3,time4);
		worktime5.tv_nsec+=temp.tv_nsec;
		int sec=worktime5.tv_nsec/1000000000;
		if(sec>0)
		{
			worktime5.tv_sec+=temp.tv_sec+sec;
			worktime5.tv_nsec=worktime5.tv_nsec%1000000000;
		}
		else
		{
			worktime5.tv_sec+=temp.tv_sec;
		}
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	int TN;
	double wn_r;
	double wn_i;
	void popToken() {
		In.updatehead(16);
	}
	void pushToken() {
		Out.updatetail(16);
	}
	void initVarAndState() {
	TN=8;
		 wn_r=cos(2*PI/TN);
		 wn_i=sin(2*PI/TN);
	}
	void init(){ 
	}
	void work(){ 
	{
		int i;
		double w_r=1,w_i=0;
		double results[2*TN];
		double y0_r,y0_i,y1_r,y1_i;
		double y1w_r,y1w_i,w_r_next,w_i_next;
		for(i=0;i<TN;i+=2)			{
		y0_r=In[i].x;
		y0_i=In[i+1].x;
		y1_r=In[TN+i].x;
		y1_i=In[TN+i+1].x;
		y1w_r=y1_r*w_r-y1_i*w_i;
		y1w_i=y1_r*w_i+y1_i*w_r;
		results[i]=y0_r+y1w_r;
		results[i+1]=y0_i+y1w_i;
		results[TN+i]=y0_r-y1w_r;
		results[TN+i+1]=y0_i-y1w_i;
		w_r_next=w_r*wn_r-w_i*wn_i;
		w_i_next=w_r*wn_i+w_i*wn_r;
		w_r=w_r_next;
		w_i=w_i_next;
	}
		for(i=0;i<2*TN;i++)			{
		Out[i].x=results[i];
	}
	}
		pushToken();
		popToken();
	}
};
#endif