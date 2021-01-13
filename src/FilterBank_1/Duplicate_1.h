#ifndef _Duplicate_1_
#define _Duplicate_1_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Duplicate_1{
public:
	Duplicate_1(Buffer<streamData>& dup0_0,Buffer<streamData>& dup0_1,Buffer<streamData>& dup0_2,Buffer<streamData>& dup0_3,Buffer<streamData>& dup0_4,Buffer<streamData>& dup0_5,Buffer<streamData>& dup0_6,Buffer<streamData>& dup0_7,Buffer<streamData>& dup0_8,Buffer<streamData>& dup0_9,Buffer<streamData>& dup0_10,Buffer<streamData>& dup0_11,Buffer<streamData>& dup0_12,Buffer<streamData>& dup0_13,Buffer<streamData>& dup0_14,Buffer<streamData>& dup0_15,Buffer<streamData>& dup0_16,Buffer<streamData>& dup0_17,Buffer<streamData>& dup0_18,Buffer<streamData>& dup0_19,Buffer<streamData>& dup0_20,Buffer<streamData>& dup0_21,Buffer<streamData>& dup0_22,Buffer<streamData>& dup0_23,Buffer<streamData>& dup0_24,Buffer<streamData>& dup0_25,Buffer<streamData>& dup0_26,Buffer<streamData>& dup0_27,Buffer<streamData>& dup0_28,Buffer<streamData>& dup0_29,Buffer<streamData>& dup0_30,Buffer<streamData>& dup0_31,Buffer<streamData>& In):dup0_0(dup0_0),dup0_1(dup0_1),dup0_2(dup0_2),dup0_3(dup0_3),dup0_4(dup0_4),dup0_5(dup0_5),dup0_6(dup0_6),dup0_7(dup0_7),dup0_8(dup0_8),dup0_9(dup0_9),dup0_10(dup0_10),dup0_11(dup0_11),dup0_12(dup0_12),dup0_13(dup0_13),dup0_14(dup0_14),dup0_15(dup0_15),dup0_16(dup0_16),dup0_17(dup0_17),dup0_18(dup0_18),dup0_19(dup0_19),dup0_20(dup0_20),dup0_21(dup0_21),dup0_22(dup0_22),dup0_23(dup0_23),dup0_24(dup0_24),dup0_25(dup0_25),dup0_26(dup0_26),dup0_27(dup0_27),dup0_28(dup0_28),dup0_29(dup0_29),dup0_30(dup0_30),dup0_31(dup0_31),In(In){
		steadyScheduleCount = 32;
		initScheduleCount = 382;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		dup0_0.resetTail();
		dup0_1.resetTail();
		dup0_2.resetTail();
		dup0_3.resetTail();
		dup0_4.resetTail();
		dup0_5.resetTail();
		dup0_6.resetTail();
		dup0_7.resetTail();
		dup0_8.resetTail();
		dup0_9.resetTail();
		dup0_10.resetTail();
		dup0_11.resetTail();
		dup0_12.resetTail();
		dup0_13.resetTail();
		dup0_14.resetTail();
		dup0_15.resetTail();
		dup0_16.resetTail();
		dup0_17.resetTail();
		dup0_18.resetTail();
		dup0_19.resetTail();
		dup0_20.resetTail();
		dup0_21.resetTail();
		dup0_22.resetTail();
		dup0_23.resetTail();
		dup0_24.resetTail();
		dup0_25.resetTail();
		dup0_26.resetTail();
		dup0_27.resetTail();
		dup0_28.resetTail();
		dup0_29.resetTail();
		dup0_30.resetTail();
		dup0_31.resetTail();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		dup0_0.resetTail();
		dup0_1.resetTail();
		dup0_2.resetTail();
		dup0_3.resetTail();
		dup0_4.resetTail();
		dup0_5.resetTail();
		dup0_6.resetTail();
		dup0_7.resetTail();
		dup0_8.resetTail();
		dup0_9.resetTail();
		dup0_10.resetTail();
		dup0_11.resetTail();
		dup0_12.resetTail();
		dup0_13.resetTail();
		dup0_14.resetTail();
		dup0_15.resetTail();
		dup0_16.resetTail();
		dup0_17.resetTail();
		dup0_18.resetTail();
		dup0_19.resetTail();
		dup0_20.resetTail();
		dup0_21.resetTail();
		dup0_22.resetTail();
		dup0_23.resetTail();
		dup0_24.resetTail();
		dup0_25.resetTail();
		dup0_26.resetTail();
		dup0_27.resetTail();
		dup0_28.resetTail();
		dup0_29.resetTail();
		dup0_30.resetTail();
		dup0_31.resetTail();
		In.resetHead();
	}
private:
	Producer<streamData> dup0_0;
	Producer<streamData> dup0_1;
	Producer<streamData> dup0_2;
	Producer<streamData> dup0_3;
	Producer<streamData> dup0_4;
	Producer<streamData> dup0_5;
	Producer<streamData> dup0_6;
	Producer<streamData> dup0_7;
	Producer<streamData> dup0_8;
	Producer<streamData> dup0_9;
	Producer<streamData> dup0_10;
	Producer<streamData> dup0_11;
	Producer<streamData> dup0_12;
	Producer<streamData> dup0_13;
	Producer<streamData> dup0_14;
	Producer<streamData> dup0_15;
	Producer<streamData> dup0_16;
	Producer<streamData> dup0_17;
	Producer<streamData> dup0_18;
	Producer<streamData> dup0_19;
	Producer<streamData> dup0_20;
	Producer<streamData> dup0_21;
	Producer<streamData> dup0_22;
	Producer<streamData> dup0_23;
	Producer<streamData> dup0_24;
	Producer<streamData> dup0_25;
	Producer<streamData> dup0_26;
	Producer<streamData> dup0_27;
	Producer<streamData> dup0_28;
	Producer<streamData> dup0_29;
	Producer<streamData> dup0_30;
	Producer<streamData> dup0_31;
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		In.updatehead(1);
	}
	void pushToken() {
		dup0_0.updatetail(1);
		dup0_1.updatetail(1);
		dup0_2.updatetail(1);
		dup0_3.updatetail(1);
		dup0_4.updatetail(1);
		dup0_5.updatetail(1);
		dup0_6.updatetail(1);
		dup0_7.updatetail(1);
		dup0_8.updatetail(1);
		dup0_9.updatetail(1);
		dup0_10.updatetail(1);
		dup0_11.updatetail(1);
		dup0_12.updatetail(1);
		dup0_13.updatetail(1);
		dup0_14.updatetail(1);
		dup0_15.updatetail(1);
		dup0_16.updatetail(1);
		dup0_17.updatetail(1);
		dup0_18.updatetail(1);
		dup0_19.updatetail(1);
		dup0_20.updatetail(1);
		dup0_21.updatetail(1);
		dup0_22.updatetail(1);
		dup0_23.updatetail(1);
		dup0_24.updatetail(1);
		dup0_25.updatetail(1);
		dup0_26.updatetail(1);
		dup0_27.updatetail(1);
		dup0_28.updatetail(1);
		dup0_29.updatetail(1);
		dup0_30.updatetail(1);
		dup0_31.updatetail(1);
	}
	void initVarAndState() {
	}
	void init(){ 
	}
	void work(){ 
	{
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		dup0_0[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_1[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_2[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_3[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_4[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_5[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_6[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_7[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_8[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_9[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_10[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_11[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_12[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_13[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_14[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_15[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_16[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_17[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_18[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_19[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_20[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_21[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_22[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_23[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_24[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_25[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_26[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_27[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_28[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_29[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_30[i]=In[j++];
		j=0;
		for(i=0;i<1;++i)		dup0_31[i]=In[j++];
		j=0;
	}
		pushToken();
		popToken();
	}
};
#endif