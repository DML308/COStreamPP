#ifndef _KeySchedule_510_
#define _KeySchedule_510_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class KeySchedule_510{
public:
	KeySchedule_510(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
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
	int round;
	int vector;
	int keys[64][48];
	void popToken() {
		In.updatehead(48);
	}
	void pushToken() {
		Out.updatetail(96);
	}
	void initVarAndState() {
	round=42;
	vector=7;
	}
	void init(){ 
	{
		int k64[64],k56[56],bits[56];
		int w,r,i;
		for(w=1;w>=0;w--)			{
		int v=USERKEYS[vector][w];
		int m=1;
		for(i=0;i<32;i++)			{
		if(((v&m)>>i)!=0)	k64[((1-w)*32)+i]=1;else	k64[((1-w)*32)+i]=0;
		m=m<<1;
	}
	}
		for(i=0;i<56;i++)			{
		k56[i]=k64[64-PC1[i]];
	}
		for(r=0;r<MAXROUNDS;r++)			{
		for(i=0;i<28;i++)		bits[i]=k56[(i+RT[r])%28];
		for(i=28;i<56;i++)		bits[i]=k56[28+((i+RT[r])%28)];
		for(i=0;i<56;i++)		k56[i]=bits[i];
		for(i=47;i>=0;i--)			{
		keys[r][47-i]=k56[PC2[i]-1];
	}
	}
	}	}
	void work(){ 
	{
		int i;
		for(i=0;i<48;i++)			{
		Out[2*i].x=keys[round][i];
		Out[2*i+1].x=In[i].x;
	}
	}
		pushToken();
		popToken();
	}
};
#endif