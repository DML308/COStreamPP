#ifndef _DCT_1D_9_
#define _DCT_1D_9_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class DCT_1D_9{
public:
	DCT_1D_9(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
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
	double coeff[8][8];
	int xi,u;
	double Cu;
	void popToken() {
		In.updatehead(8);
	}
	void pushToken() {
		Out.updatetail(8);
	}
	void initVarAndState() {
	}
	void init(){ 
	{
		for(xi=0;xi<8;xi++)			{
		Cu=0.500000;
		if(xi==0)	Cu=sqrt(0.125000);
		for(u=0;u<8;u++)			{
		coeff[xi][u]=Cu*cos((pi/8.000000)*xi*(u+0.500000));
	}
	}
	}	}
	void work(){ 
	{
		int xi,u;
		double tmp;
		for(xi=0;xi<8;xi++)			{
		tmp=0.000000;
		for(u=0;u<8;u++)			{
		tmp+=(coeff[xi][u]*In[u].x);
	}
		Out[xi].x=tmp;
	}
	}
		pushToken();
		popToken();
	}
};
#endif