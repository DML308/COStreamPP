#ifndef _CombineDFTX_3_
#define _CombineDFTX_3_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
#include "sys/time.h"

using namespace std;
class CombineDFTX_3{

long long getCurrentTime(){//用于获取系统微秒时间
    struct timeval tv; 
    gettimeofday(&tv,NULL);
    return tv.tv_sec*1000*1000+tv.tv_usec;
}

public:
	CombineDFTX_3(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
		steadyScheduleCount = 4;
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
		long long t1=getCurrentTime();
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		long long t2=getCurrentTime();
		T1+=(t2-t1);
		//cout<<"the clock of work:"<<3100*(t2-t1)<<endl;
		long long t3=getCurrentTime();
		Out.resetTail();
		In.resetHead();
		long long t4=getCurrentTime();
		//cout<<"the clock of work:"<<3100*(t4-t3)<<endl;
		
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
		In.updatehead(4);
	}
	void pushToken() {
		Out.updatetail(4);
	}
	void initVarAndState() {
	TN=2;
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