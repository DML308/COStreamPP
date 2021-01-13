/*该文件定义各thread的入口函数，在函数内部完成软件流水迭代*/
#include "Buffer.h"
#include "Producer.h"
#include "Consumer.h"
#include "Global.h"
#include "AllActorHeader.h"	//包含所有actor的头文件
#include "lock_free_barrier.h"	//包含barrier函数
#include "rdtsc.h"
#include <fstream>
extern int MAX_ITER;
void thread_0_fun()
{
	masterSync(1);
	IntSource_0 IntSource_0_obj(IntSource_0_Roundrobin_1);
	Roundrobin_1 Roundrobin_1_obj(Roundrobin_1_DCT_1D_2,Roundrobin_1_DCT_1D_3,Roundrobin_1_DCT_1D_4,Roundrobin_1_DCT_1D_5,Roundrobin_1_DCT_1D_6,Roundrobin_1_DCT_1D_7,Roundrobin_1_DCT_1D_8,Roundrobin_1_DCT_1D_9,IntSource_0_Roundrobin_1);
	DCT_1D_2 DCT_1D_2_obj(DCT_1D_2_join_10,Roundrobin_1_DCT_1D_2);
	DCT_1D_3 DCT_1D_3_obj(DCT_1D_3_join_10,Roundrobin_1_DCT_1D_3);
	DCT_1D_4 DCT_1D_4_obj(DCT_1D_4_join_10,Roundrobin_1_DCT_1D_4);
	DCT_1D_5 DCT_1D_5_obj(DCT_1D_5_join_10,Roundrobin_1_DCT_1D_5);
	DCT_1D_6 DCT_1D_6_obj(DCT_1D_6_join_10,Roundrobin_1_DCT_1D_6);
	DCT_1D_7 DCT_1D_7_obj(DCT_1D_7_join_10,Roundrobin_1_DCT_1D_7);
	DCT_1D_8 DCT_1D_8_obj(DCT_1D_8_join_10,Roundrobin_1_DCT_1D_8);
	DCT_1D_9 DCT_1D_9_obj(DCT_1D_9_join_10,Roundrobin_1_DCT_1D_9);
	join_10 join_10_obj(join_10_Roundrobin_11,DCT_1D_2_join_10,DCT_1D_3_join_10,DCT_1D_4_join_10,DCT_1D_5_join_10,DCT_1D_6_join_10,DCT_1D_7_join_10,DCT_1D_8_join_10,DCT_1D_9_join_10);
	Roundrobin_11 Roundrobin_11_obj(Roundrobin_11_DCT_1D_12,Roundrobin_11_DCT_1D_13,Roundrobin_11_DCT_1D_14,Roundrobin_11_DCT_1D_15,Roundrobin_11_DCT_1D_16,Roundrobin_11_DCT_1D_17,Roundrobin_11_DCT_1D_18,Roundrobin_11_DCT_1D_19,join_10_Roundrobin_11);
	DCT_1D_12 DCT_1D_12_obj(DCT_1D_12_join_20,Roundrobin_11_DCT_1D_12);
	DCT_1D_13 DCT_1D_13_obj(DCT_1D_13_join_20,Roundrobin_11_DCT_1D_13);
	DCT_1D_14 DCT_1D_14_obj(DCT_1D_14_join_20,Roundrobin_11_DCT_1D_14);
	DCT_1D_15 DCT_1D_15_obj(DCT_1D_15_join_20,Roundrobin_11_DCT_1D_15);
	DCT_1D_16 DCT_1D_16_obj(DCT_1D_16_join_20,Roundrobin_11_DCT_1D_16);
	DCT_1D_17 DCT_1D_17_obj(DCT_1D_17_join_20,Roundrobin_11_DCT_1D_17);
	DCT_1D_18 DCT_1D_18_obj(DCT_1D_18_join_20,Roundrobin_11_DCT_1D_18);
	DCT_1D_19 DCT_1D_19_obj(DCT_1D_19_join_20,Roundrobin_11_DCT_1D_19);
	join_20 join_20_obj(join_20_DCT_Result_21,DCT_1D_12_join_20,DCT_1D_13_join_20,DCT_1D_14_join_20,DCT_1D_15_join_20,DCT_1D_16_join_20,DCT_1D_17_join_20,DCT_1D_18_join_20,DCT_1D_19_join_20);
	DCT_Result_21 DCT_Result_21_obj(DCT_Result_21_Sink_22,join_20_DCT_Result_21);
	Sink_22 Sink_22_obj(DCT_Result_21_Sink_22);
	char stage[1]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<1;_stageNum++)
	{
		if(0==_stageNum)
		{
			IntSource_0_obj.runInitScheduleWork();
			Roundrobin_1_obj.runInitScheduleWork();
			DCT_1D_2_obj.runInitScheduleWork();
			DCT_1D_3_obj.runInitScheduleWork();
			DCT_1D_4_obj.runInitScheduleWork();
			DCT_1D_5_obj.runInitScheduleWork();
			DCT_1D_6_obj.runInitScheduleWork();
			DCT_1D_7_obj.runInitScheduleWork();
			DCT_1D_8_obj.runInitScheduleWork();
			DCT_1D_9_obj.runInitScheduleWork();
			join_10_obj.runInitScheduleWork();
			Roundrobin_11_obj.runInitScheduleWork();
			DCT_1D_12_obj.runInitScheduleWork();
			DCT_1D_13_obj.runInitScheduleWork();
			DCT_1D_14_obj.runInitScheduleWork();
			DCT_1D_15_obj.runInitScheduleWork();
			DCT_1D_16_obj.runInitScheduleWork();
			DCT_1D_17_obj.runInitScheduleWork();
			DCT_1D_18_obj.runInitScheduleWork();
			DCT_1D_19_obj.runInitScheduleWork();
			join_20_obj.runInitScheduleWork();
			DCT_Result_21_obj.runInitScheduleWork();
			Sink_22_obj.runInitScheduleWork();
		}
	
		masterSync(1);
	}
	for(int _stageNum=1;_stageNum<2*1+MAX_ITER-1;_stageNum++)
	{
		if(stage[0])
		{
			IntSource_0_obj.runSteadyScheduleWork();
			Roundrobin_1_obj.runSteadyScheduleWork();
			DCT_1D_2_obj.runSteadyScheduleWork();
			DCT_1D_3_obj.runSteadyScheduleWork();
			DCT_1D_4_obj.runSteadyScheduleWork();
			DCT_1D_5_obj.runSteadyScheduleWork();
			DCT_1D_6_obj.runSteadyScheduleWork();
			DCT_1D_7_obj.runSteadyScheduleWork();
			DCT_1D_8_obj.runSteadyScheduleWork();
			DCT_1D_9_obj.runSteadyScheduleWork();
			join_10_obj.runSteadyScheduleWork();
			Roundrobin_11_obj.runSteadyScheduleWork();
			DCT_1D_12_obj.runSteadyScheduleWork();
			DCT_1D_13_obj.runSteadyScheduleWork();
			DCT_1D_14_obj.runSteadyScheduleWork();
			DCT_1D_15_obj.runSteadyScheduleWork();
			DCT_1D_16_obj.runSteadyScheduleWork();
			DCT_1D_17_obj.runSteadyScheduleWork();
			DCT_1D_18_obj.runSteadyScheduleWork();
			DCT_1D_19_obj.runSteadyScheduleWork();
			join_20_obj.runSteadyScheduleWork();
			DCT_Result_21_obj.runSteadyScheduleWork();
			Sink_22_obj.runSteadyScheduleWork();
		}
		for(int index=0; index>= 1; --index)
			stage[index] = stage[index-1];
		if(_stageNum == (MAX_ITER - 1 + 1))
		{
			stage[0]=0;
		}
	
		masterSync(1);
	}
}
