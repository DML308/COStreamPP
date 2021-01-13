#ifndef _InverseQuantization_DC_Intra_Coeff_5_
#define _InverseQuantization_DC_Intra_Coeff_5_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class InverseQuantization_DC_Intra_Coeff_5{
public:
	InverseQuantization_DC_Intra_Coeff_5(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
		steadyScheduleCount = 6;
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
	int intra_dc_mult[4];
	int intra_dc_precision;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		Out.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	{
		intra_dc_mult[0]=8;
		intra_dc_mult[1]=4;
		intra_dc_mult[2]=2;
		intra_dc_mult[3]=1;
		intra_dc_precision=0;
	}	}
	void work(){ 
	{
		Out[0].x=intra_dc_mult[intra_dc_precision]*In[0].x;
	}
		pushToken();
		popToken();
	}
};
#endif