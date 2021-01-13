#ifndef _InverseQuantization_AC_Coeff_8_
#define _InverseQuantization_AC_Coeff_8_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class InverseQuantization_AC_Coeff_8{
public:
	InverseQuantization_AC_Coeff_8(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
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
	int macroblock_intra;
	int quantiser_scale_code;
	int q_scale_type;
	static int intra_quantiser_matrix[64];
	static int non_intra_quantiser_matrix[64];
	static int quantiser_scale[2][32];
	void popToken() {
		In.updatehead(64);
	}
	void pushToken() {
		Out.updatetail(64);
	}
	void initVarAndState() {
	macroblock_intra=0;
	}
	void init(){ 
	{
		quantiser_scale_code=1;
		q_scale_type=0;
	}	}
	void work(){ 
	{
		int i,push=0,pop=0;
		if(quantiser_scale_code==0)	cout<<"Error - quantiser_scale_code not allowed to be 0 "+macroblock_intra<<endl;
		for(i=macroblock_intra;i<64;i++)			{
		int W=0,F;
		int QF=In[pop++].x;
		int k=0;
		if(macroblock_intra==1)		{
		k=0;
	}else		{
		if(QF>0)		{
		k=1;
	}else	if(QF<0)		{
		k=-1;
	}else		{
		k=0;
	}
	}
		if(macroblock_intra==1)		{
		W=intra_quantiser_matrix[i];
	}else		{
		W=non_intra_quantiser_matrix[i];
	}
		F=(2*QF+k)*W*quantiser_scale[q_scale_type][quantiser_scale_code]/32;
		Out[push++].x=F;
	}
	}
		pushToken();
		popToken();
	}
};
int InverseQuantization_AC_Coeff_8::intra_quantiser_matrix[64]={8,16,19,22,26,27,29,34,16,16,22,24,27,29,34,37,19,22,26,27,29,34,34,38,22,22,26,27,29,34,37,40,22,26,27,29,32,35,40,48,26,27,29,32,35,40,48,58,26,27,29,34,38,46,56,69,27,29,35,38,46,56,69,83};
int InverseQuantization_AC_Coeff_8::non_intra_quantiser_matrix[64]={16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16};
int InverseQuantization_AC_Coeff_8::quantiser_scale[2][32]={{0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62},{0,1,2,3,4,5,6,7,8,10,12,14,16,18,20,22,24,28,32,36,40,44,48,52,56,64,72,80,88,96,104,112}};
#endif