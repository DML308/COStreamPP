#ifndef _join_290_
#define _join_290_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class join_290{
public:
	join_290(Buffer<streamData>& Out,Buffer<streamData>& Dstream0_0,Buffer<streamData>& Dstream0_1,Buffer<streamData>& Dstream0_2,Buffer<streamData>& Dstream0_3,Buffer<streamData>& Dstream0_4,Buffer<streamData>& Dstream0_5,Buffer<streamData>& Dstream0_6,Buffer<streamData>& Dstream0_7,Buffer<streamData>& Dstream0_8,Buffer<streamData>& Dstream0_9,Buffer<streamData>& Dstream0_10,Buffer<streamData>& Dstream0_11,Buffer<streamData>& Dstream0_12,Buffer<streamData>& Dstream0_13,Buffer<streamData>& Dstream0_14,Buffer<streamData>& Dstream0_15,Buffer<streamData>& Dstream0_16,Buffer<streamData>& Dstream0_17,Buffer<streamData>& Dstream0_18,Buffer<streamData>& Dstream0_19,Buffer<streamData>& Dstream0_20,Buffer<streamData>& Dstream0_21,Buffer<streamData>& Dstream0_22,Buffer<streamData>& Dstream0_23,Buffer<streamData>& Dstream0_24,Buffer<streamData>& Dstream0_25,Buffer<streamData>& Dstream0_26,Buffer<streamData>& Dstream0_27,Buffer<streamData>& Dstream0_28,Buffer<streamData>& Dstream0_29,Buffer<streamData>& Dstream0_30,Buffer<streamData>& Dstream0_31):Out(Out),Dstream0_0(Dstream0_0),Dstream0_1(Dstream0_1),Dstream0_2(Dstream0_2),Dstream0_3(Dstream0_3),Dstream0_4(Dstream0_4),Dstream0_5(Dstream0_5),Dstream0_6(Dstream0_6),Dstream0_7(Dstream0_7),Dstream0_8(Dstream0_8),Dstream0_9(Dstream0_9),Dstream0_10(Dstream0_10),Dstream0_11(Dstream0_11),Dstream0_12(Dstream0_12),Dstream0_13(Dstream0_13),Dstream0_14(Dstream0_14),Dstream0_15(Dstream0_15),Dstream0_16(Dstream0_16),Dstream0_17(Dstream0_17),Dstream0_18(Dstream0_18),Dstream0_19(Dstream0_19),Dstream0_20(Dstream0_20),Dstream0_21(Dstream0_21),Dstream0_22(Dstream0_22),Dstream0_23(Dstream0_23),Dstream0_24(Dstream0_24),Dstream0_25(Dstream0_25),Dstream0_26(Dstream0_26),Dstream0_27(Dstream0_27),Dstream0_28(Dstream0_28),Dstream0_29(Dstream0_29),Dstream0_30(Dstream0_30),Dstream0_31(Dstream0_31){
		steadyScheduleCount = 32;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream0_0.resetHead();
		Dstream0_1.resetHead();
		Dstream0_2.resetHead();
		Dstream0_3.resetHead();
		Dstream0_4.resetHead();
		Dstream0_5.resetHead();
		Dstream0_6.resetHead();
		Dstream0_7.resetHead();
		Dstream0_8.resetHead();
		Dstream0_9.resetHead();
		Dstream0_10.resetHead();
		Dstream0_11.resetHead();
		Dstream0_12.resetHead();
		Dstream0_13.resetHead();
		Dstream0_14.resetHead();
		Dstream0_15.resetHead();
		Dstream0_16.resetHead();
		Dstream0_17.resetHead();
		Dstream0_18.resetHead();
		Dstream0_19.resetHead();
		Dstream0_20.resetHead();
		Dstream0_21.resetHead();
		Dstream0_22.resetHead();
		Dstream0_23.resetHead();
		Dstream0_24.resetHead();
		Dstream0_25.resetHead();
		Dstream0_26.resetHead();
		Dstream0_27.resetHead();
		Dstream0_28.resetHead();
		Dstream0_29.resetHead();
		Dstream0_30.resetHead();
		Dstream0_31.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		Out.resetTail();
		Dstream0_0.resetHead();
		Dstream0_1.resetHead();
		Dstream0_2.resetHead();
		Dstream0_3.resetHead();
		Dstream0_4.resetHead();
		Dstream0_5.resetHead();
		Dstream0_6.resetHead();
		Dstream0_7.resetHead();
		Dstream0_8.resetHead();
		Dstream0_9.resetHead();
		Dstream0_10.resetHead();
		Dstream0_11.resetHead();
		Dstream0_12.resetHead();
		Dstream0_13.resetHead();
		Dstream0_14.resetHead();
		Dstream0_15.resetHead();
		Dstream0_16.resetHead();
		Dstream0_17.resetHead();
		Dstream0_18.resetHead();
		Dstream0_19.resetHead();
		Dstream0_20.resetHead();
		Dstream0_21.resetHead();
		Dstream0_22.resetHead();
		Dstream0_23.resetHead();
		Dstream0_24.resetHead();
		Dstream0_25.resetHead();
		Dstream0_26.resetHead();
		Dstream0_27.resetHead();
		Dstream0_28.resetHead();
		Dstream0_29.resetHead();
		Dstream0_30.resetHead();
		Dstream0_31.resetHead();
	}
private:
	Producer<streamData> Out;
	Consumer<streamData> Dstream0_0;
	Consumer<streamData> Dstream0_1;
	Consumer<streamData> Dstream0_2;
	Consumer<streamData> Dstream0_3;
	Consumer<streamData> Dstream0_4;
	Consumer<streamData> Dstream0_5;
	Consumer<streamData> Dstream0_6;
	Consumer<streamData> Dstream0_7;
	Consumer<streamData> Dstream0_8;
	Consumer<streamData> Dstream0_9;
	Consumer<streamData> Dstream0_10;
	Consumer<streamData> Dstream0_11;
	Consumer<streamData> Dstream0_12;
	Consumer<streamData> Dstream0_13;
	Consumer<streamData> Dstream0_14;
	Consumer<streamData> Dstream0_15;
	Consumer<streamData> Dstream0_16;
	Consumer<streamData> Dstream0_17;
	Consumer<streamData> Dstream0_18;
	Consumer<streamData> Dstream0_19;
	Consumer<streamData> Dstream0_20;
	Consumer<streamData> Dstream0_21;
	Consumer<streamData> Dstream0_22;
	Consumer<streamData> Dstream0_23;
	Consumer<streamData> Dstream0_24;
	Consumer<streamData> Dstream0_25;
	Consumer<streamData> Dstream0_26;
	Consumer<streamData> Dstream0_27;
	Consumer<streamData> Dstream0_28;
	Consumer<streamData> Dstream0_29;
	Consumer<streamData> Dstream0_30;
	Consumer<streamData> Dstream0_31;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	void popToken() {
		Dstream0_0.updatehead(1);
		Dstream0_1.updatehead(1);
		Dstream0_2.updatehead(1);
		Dstream0_3.updatehead(1);
		Dstream0_4.updatehead(1);
		Dstream0_5.updatehead(1);
		Dstream0_6.updatehead(1);
		Dstream0_7.updatehead(1);
		Dstream0_8.updatehead(1);
		Dstream0_9.updatehead(1);
		Dstream0_10.updatehead(1);
		Dstream0_11.updatehead(1);
		Dstream0_12.updatehead(1);
		Dstream0_13.updatehead(1);
		Dstream0_14.updatehead(1);
		Dstream0_15.updatehead(1);
		Dstream0_16.updatehead(1);
		Dstream0_17.updatehead(1);
		Dstream0_18.updatehead(1);
		Dstream0_19.updatehead(1);
		Dstream0_20.updatehead(1);
		Dstream0_21.updatehead(1);
		Dstream0_22.updatehead(1);
		Dstream0_23.updatehead(1);
		Dstream0_24.updatehead(1);
		Dstream0_25.updatehead(1);
		Dstream0_26.updatehead(1);
		Dstream0_27.updatehead(1);
		Dstream0_28.updatehead(1);
		Dstream0_29.updatehead(1);
		Dstream0_30.updatehead(1);
		Dstream0_31.updatehead(1);
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
		int i=0;
		int j=0;
		for(i=0;i<1;++i)		Out[j++]=Dstream0_0[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_1[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_2[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_3[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_4[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_5[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_6[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_7[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_8[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_9[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_10[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_11[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_12[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_13[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_14[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_15[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_16[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_17[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_18[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_19[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_20[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_21[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_22[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_23[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_24[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_25[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_26[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_27[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_28[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_29[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_30[i];
		for(i=0;i<1;++i)		Out[j++]=Dstream0_31[i];
	}
		pushToken();
		popToken();
	}
};
#endif