#ifndef _FFTReorderSimpleX_2_
#define _FFTReorderSimpleX_2_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class FFTReorderSimpleX_2{
	
	//timespec diff(timespec start,timespec end);
public:
	FFTReorderSimpleX_2(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
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
		
	}
	void runSteadyScheduleWork() {
		timespec time1,time2;
		timespec time3,time4;
		clock_gettime(CLOCK_REALTIME,&time1);
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		clock_gettime(CLOCK_REALTIME,&time2);
		timespec tmp=diff(time1,time2);
		workTime2.tv_nsec+=tmp.tv_nsec;
		int sec_add=workTime2.tv_nsec/1000000000;
		if(sec_add>0)
		{
			workTime2.tv_sec+=(tmp.tv_sec+sec_add);
			workTime2.tv_nsec=workTime2.tv_nsec%1000000000;
		}
		else
		{
			workTime2.tv_sec+=tmp.tv_sec;
		}
		
		//cout<<"time passed:"<<"FFTReorderSimple:"<<(time2.tv_sec-time1.tv_sec)*1000<<"ms"<<(time2.tv_nsec - time1.tv_nsec)<< "ns" <<endl;
		clock_gettime(CLOCK_REALTIME,&time3);
		Out.resetTail();
		In.resetHead();
		clock_gettime(CLOCK_REALTIME,&time4);
		timespec temp=diff(time3,time4);
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
		//cout<<""
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	int n;
	void popToken() {
		In.updatehead(16);
	}
	void pushToken() {
		Out.updatetail(16);
	}
	void initVarAndState() {
	n=8;
	}
	void init(){ 
	}
	void work(){ 
	{
		int i,j,k;
		int m=0;
		int final[n];
		for(i=0;i<n;i++)		final[i]=0;
		for(i=0;i<n;i++)			{
		int c=i;
		for(j=0;j<log(n);j++)			{
		final[i]<<=1;
		final[i]|=(c&1);
		c>>=1;
	}
	}
		for(i=0;i<n;i++)			{
		Out[final[i]*2].x=In[i*2].x;
		Out[final[i]*2+1].x=In[i*2+1].x;
	}
	}
		pushToken();
		popToken();
	}
};
#endif