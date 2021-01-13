#ifndef _AssignmentX_1_
#define _AssignmentX_1_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class AssignmentX_1{
	//timespec diff(timespec start,timespec end);
public:
	AssignmentX_1(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
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
		workTime1.tv_nsec+=tmp.tv_nsec;
		int sec_add=workTime1.tv_nsec/1000000000;
		if(sec_add>0)
		{
			workTime1.tv_sec+=(tmp.tv_sec+sec_add);
			workTime1.tv_nsec=workTime1.tv_nsec%1000000000;
		}
		else
		{
			workTime1.tv_sec+=tmp.tv_sec;
		}
		//cout<<"time passed:"<<"节点一"<<(time2.tv_sec-time1.tv_sec)*1000000000+(time2.tv_nsec - time1.tv_nsec) << "ns" <<endl;
		clock_gettime(CLOCK_REALTIME,&time3);
		Out.resetTail();
		In.resetHead();
		clock_gettime(CLOCK_REALTIME,&time4);
		timespec temp=diff(time3,time4);
		worktime1.tv_nsec+=temp.tv_nsec;
		int sec=worktime1.tv_nsec/1000000000;
		if(sec>0)
		{
			worktime1.tv_sec+=temp.tv_sec+sec;
			worktime1.tv_nsec=worktime1.tv_nsec%1000000000;
		}
		else
		{
			worktime1.tv_sec+=temp.tv_sec;
		}
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	int N;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		Out.updatetail(16);
	}
	void initVarAndState() {
	N=8;
	}
	void init(){ 
	}
	void work(){ 
	{
		int i;
		cout<<"\nOut:"<<endl;
		for(i=0;i<N*2;i++)			{
		Out[i].x=In[0].x+i;
		cout<<Out[i].x;
	}
		cout<<"\n"<<endl;
	}
		pushToken();
		popToken();
	}
};
#endif