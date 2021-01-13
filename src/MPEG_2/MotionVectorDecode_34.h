#ifndef _MotionVectorDecode_34_
#define _MotionVectorDecode_34_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class MotionVectorDecode_34{
public:
	MotionVectorDecode_34(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
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
	int PMV[8];
	int mv_format;
	int picture_structure;
	void popToken() {
		In.updatehead(16);
	}
	void pushToken() {
		Out.updatetail(8);
	}
	void initVarAndState() {
	}
	void init(){ 
	{
		mv_format=1;
		picture_structure=1;
	}	}
	void work(){ 
	{
		int r,s,t;
		int pop=0;
		int push=0;
		int motion_code[8]={0,0,0,0,0,0,0,0};
		int motion_residual[8]={0,0,0,0,0,0,0,0};
		int vectorp[8]={0,0,0,0,0,0,0,0};
		for(r=0;r<2;r++)		for(s=0;s<2;s++)		for(t=0;t<2;t++)			{
		motion_code[r*4+s*2+t]=In[pop].x;
		pop++;
	}
		for(r=0;r<2;r++)		for(s=0;s<2;s++)		for(t=0;t<2;t++)			{
		motion_residual[r*4+s*2+t]=In[pop].x;
		pop++;
	}
		for(r=0;r<1;r++)			{
		for(s=0;s<2;s++)			{
		for(t=0;t<2;t++)			{
		int r_size=14;
		int f=1<<r_size;
		int high=(16*f)-1;
		int low=((-16)*f);
		int range=(32*f);
		int delta;
		int prediction;
		if((f==1)||(motion_code[r*4+s*2+t]==0))		{
		delta=motion_code[r*4+s*2+t];
	}else		{
		delta=((int)(abs(motion_code[r*4+s*2+t])-1)*f)+motion_residual[r*4+s*2+t]+1;
		if(motion_code[r*4+s*2+t]<0)	delta=-delta;
	}
		prediction=PMV[r*4+s*2+t];
		if((mv_format==0)&&(t==1)&&(picture_structure==3))	cout<<"Error - Program Limitation: May not be correct in decoding motion vectors"<<endl;
		vectorp[r*4+s*2+t]=prediction+delta;
		if(vectorp[r*4+s*2+t]<low)	vectorp[r*4+s*2+t]=vectorp[r*4+s*2+t]+range;
		if(vectorp[r*4+s*2+t]>high)	vectorp[r*4+s*2+t]=vectorp[r*4+s*2+t]-range;
		if((mv_format==0)&&(t==1)&&(picture_structure==3))	cout<<"Error - Program Limitation: May not be correct in decoding motion vectors"<<endl;else	PMV[r*4+s*2+t]=vectorp[r*4+s*2+t];
	}
	}
	}
		for(r=0;r<2;r++)		for(s=0;s<2;s++)		for(t=0;t<2;t++)			{
		Out[push].x=vectorp[r*4+s*2+t];
		push++;
	}
	}
		pushToken();
		popToken();
	}
};
#endif