#ifndef _iDCT8x8_1D_col_fast_fine_30_
#define _iDCT8x8_1D_col_fast_fine_30_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class iDCT8x8_1D_col_fast_fine_30{
public:
	iDCT8x8_1D_col_fast_fine_30(Buffer<streamData>& Out,Buffer<streamData>& In):Out(Out),In(In){
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
	int size;
	int W1;
	int W2;
	int W3;
	int W5;
	int W6;
	int W7;
	void popToken() {
		In.updatehead(8);
	}
	void pushToken() {
		Out.updatetail(8);
	}
	void initVarAndState() {
	size=8;
		 W1=2841;
		 W2=2676;
		 W3=2408;
		 W5=1609;
		 W6=1108;
		 W7=565;
	}
	void init(){ 
	}
	void work(){ 
	{
		int x0=In[0].x;
		int x1=In[4].x<<8;
		int x2=In[6].x;
		int x3=In[2].x;
		int x4=In[1].x;
		int x5=In[7].x;
		int x6=In[5].x;
		int x7=In[3].x;
		int x8;
		int i;
		if((x1==0)&&(x2==0)&&(x3==0)&&(x4==0)&&(x5==0)&&(x6==0)&&(x7==0))		{
		x0=(x0+32)>>6;
		for(i=0;i<size;i++)			{
		Out[i].x=x0;
	}
	}else		{
		x0=(x0<<8)+8192;
		x8=W7*(x4+x5)+4;
		x4=(x8+(W1-W7)*x4)>>3;
		x5=(x8-(W1+W7)*x5)>>3;
		x8=W3*(x6+x7)+4;
		x6=(x8-(W3-W5)*x6)>>3;
		x7=(x8-(W3+W5)*x7)>>3;
		x8=x0+x1;
		x0=x0-x1;
		x1=W6*(x3+x2)+4;
		x2=(x1-(W2+W6)*x2)>>3;
		x3=(x1+(W2-W6)*x3)>>3;
		x1=x4+x6;
		x4=x4-x6;
		x6=x5+x7;
		x5=x5-x7;
		x7=x8+x3;
		x8=x8-x3;
		x3=x0+x2;
		x0=x0-x2;
		x2=(181*(x4+x5)+128)>>8;
		x4=(181*(x4-x5)+128)>>8;
		Out[0].x=(x7+x1)>>14;
		Out[1].x=(x3+x2)>>14;
		Out[2].x=(x0+x4)>>14;
		Out[3].x=(x8+x6)>>14;
		Out[4].x=(x8-x6)>>14;
		Out[5].x=(x0-x4)>>14;
		Out[6].x=(x3-x2)>>14;
		Out[7].x=(x7-x1)>>14;
	}
	}
		pushToken();
		popToken();
	}
};
#endif