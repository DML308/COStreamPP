#ifndef _Sboxes_608_
#define _Sboxes_608_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Sboxes_608{
public:
	Sboxes_608(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
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
	void popToken() {
		In.updatehead(48);
	}
	void pushToken() {
		Out.updatetail(32);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i,r,c,out,tmp;
		for(i=1;i<=8;i++)			{
		r=In[6*(i-1)+0].x;
		c=In[6*(i-1)+1].x;
		c=(In[6*(i-1)+2].x<<1)|c;
		c=(In[6*(i-1)+3].x<<2)|c;
		c=(In[6*(i-1)+4].x<<3)|c;
		r=(In[6*(i-1)+5].x<<1)|r;
		out=0;
		if(i==1)	out=S8[r][c];else	if(i==2)	out=S7[r][c];else	if(i==3)	out=S6[r][c];else	if(i==4)	out=S5[r][c];else	if(i==5)	out=S4[r][c];else	if(i==6)	out=S3[r][c];else	if(i==7)	out=S2[r][c];else	if(i==8)	out=S1[r][c];
		tmp=((int)((out&1)>>0));
		Out[4*(i-1)+0].x=tmp;
		tmp=((int)((out&2)>>1));
		Out[4*(i-1)+1].x=tmp;
		tmp=((int)((out&4)>>2));
		Out[4*(i-1)+2].x=tmp;
		tmp=((int)((out&8)>>3));
		Out[4*(i-1)+3].x=tmp;
	}
	}
		pushToken();
		popToken();
	}
};
#endif