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
	masterSync(2);
	Source_0 Source_0_obj(Source_0_Roundrobin_1);
	Roundrobin_1 Roundrobin_1_obj(Roundrobin_1_CompareExchange_2,Roundrobin_1_CompareExchange_3,Roundrobin_1_CompareExchange_4,Roundrobin_1_CompareExchange_5,Source_0_Roundrobin_1);
	CompareExchange_2 CompareExchange_2_obj(CompareExchange_2_join_6,Roundrobin_1_CompareExchange_2);
	CompareExchange_3 CompareExchange_3_obj(CompareExchange_3_join_6,Roundrobin_1_CompareExchange_3);
	CompareExchange_4 CompareExchange_4_obj(CompareExchange_4_join_6,Roundrobin_1_CompareExchange_4);
	CompareExchange_5 CompareExchange_5_obj(CompareExchange_5_join_6,Roundrobin_1_CompareExchange_5);
	join_6 join_6_obj(join_6_Roundrobin_7,CompareExchange_2_join_6,CompareExchange_3_join_6,CompareExchange_4_join_6,CompareExchange_5_join_6);
	Roundrobin_7 Roundrobin_7_obj(Roundrobin_7_Roundrobin_8,Roundrobin_7_Roundrobin_12,join_6_Roundrobin_7);
	Roundrobin_8 Roundrobin_8_obj(Roundrobin_8_CompareExchange_9,Roundrobin_8_CompareExchange_10,Roundrobin_7_Roundrobin_8);
	CompareExchange_9 CompareExchange_9_obj(CompareExchange_9_join_11,Roundrobin_8_CompareExchange_9);
	CompareExchange_10 CompareExchange_10_obj(CompareExchange_10_join_11,Roundrobin_8_CompareExchange_10);
	join_11 join_11_obj(join_11_join_16,CompareExchange_9_join_11,CompareExchange_10_join_11);
	Roundrobin_12 Roundrobin_12_obj(Roundrobin_12_CompareExchange_13,Roundrobin_12_CompareExchange_14,Roundrobin_7_Roundrobin_12);
	CompareExchange_13 CompareExchange_13_obj(CompareExchange_13_join_15,Roundrobin_12_CompareExchange_13);
	CompareExchange_14 CompareExchange_14_obj(CompareExchange_14_join_15,Roundrobin_12_CompareExchange_14);
	join_15 join_15_obj(join_15_join_16,CompareExchange_13_join_15,CompareExchange_14_join_15);
	join_16 join_16_obj(join_16_Roundrobin_17,join_11_join_16,join_15_join_16);
	Roundrobin_17 Roundrobin_17_obj(Roundrobin_17_CompareExchange_18,Roundrobin_17_CompareExchange_19,Roundrobin_17_CompareExchange_20,Roundrobin_17_CompareExchange_21,join_16_Roundrobin_17);
	CompareExchange_18 CompareExchange_18_obj(CompareExchange_18_join_22,Roundrobin_17_CompareExchange_18);
	CompareExchange_19 CompareExchange_19_obj(CompareExchange_19_join_22,Roundrobin_17_CompareExchange_19);
	CompareExchange_20 CompareExchange_20_obj(CompareExchange_20_join_22,Roundrobin_17_CompareExchange_20);
	CompareExchange_21 CompareExchange_21_obj(CompareExchange_21_join_22,Roundrobin_17_CompareExchange_21);
	join_22 join_22_obj(join_22_Roundrobin_23,CompareExchange_18_join_22,CompareExchange_19_join_22,CompareExchange_20_join_22,CompareExchange_21_join_22);
	Roundrobin_23 Roundrobin_23_obj(Roundrobin_23_Roundrobin_24,join_22_Roundrobin_23);
	Roundrobin_24 Roundrobin_24_obj(Roundrobin_24_CompareExchange_25,Roundrobin_24_CompareExchange_26,Roundrobin_24_CompareExchange_27,Roundrobin_24_CompareExchange_28,Roundrobin_23_Roundrobin_24);
	CompareExchange_26 CompareExchange_26_obj(CompareExchange_26_join_29,Roundrobin_24_CompareExchange_26);
	char stage[3]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<3;_stageNum++)
	{
		if(0==_stageNum)
		{
			Source_0_obj.runInitScheduleWork();
			Roundrobin_1_obj.runInitScheduleWork();
			CompareExchange_2_obj.runInitScheduleWork();
			CompareExchange_3_obj.runInitScheduleWork();
			CompareExchange_4_obj.runInitScheduleWork();
			CompareExchange_5_obj.runInitScheduleWork();
			join_6_obj.runInitScheduleWork();
			Roundrobin_7_obj.runInitScheduleWork();
			Roundrobin_8_obj.runInitScheduleWork();
			CompareExchange_9_obj.runInitScheduleWork();
			CompareExchange_10_obj.runInitScheduleWork();
			join_11_obj.runInitScheduleWork();
			Roundrobin_12_obj.runInitScheduleWork();
			CompareExchange_13_obj.runInitScheduleWork();
			CompareExchange_14_obj.runInitScheduleWork();
			join_15_obj.runInitScheduleWork();
			join_16_obj.runInitScheduleWork();
			Roundrobin_17_obj.runInitScheduleWork();
			CompareExchange_18_obj.runInitScheduleWork();
			CompareExchange_19_obj.runInitScheduleWork();
			CompareExchange_20_obj.runInitScheduleWork();
			CompareExchange_21_obj.runInitScheduleWork();
			join_22_obj.runInitScheduleWork();
			Roundrobin_23_obj.runInitScheduleWork();
			Roundrobin_24_obj.runInitScheduleWork();
			CompareExchange_26_obj.runInitScheduleWork();
		}
	
		masterSync(2);
	}
	for(int _stageNum=3;_stageNum<2*3+MAX_ITER-1;_stageNum++)
	{
		if(stage[0])
		{
			Source_0_obj.runSteadyScheduleWork();
			Roundrobin_1_obj.runSteadyScheduleWork();
			CompareExchange_2_obj.runSteadyScheduleWork();
			CompareExchange_3_obj.runSteadyScheduleWork();
			CompareExchange_4_obj.runSteadyScheduleWork();
			CompareExchange_5_obj.runSteadyScheduleWork();
			join_6_obj.runSteadyScheduleWork();
			Roundrobin_7_obj.runSteadyScheduleWork();
			Roundrobin_8_obj.runSteadyScheduleWork();
			CompareExchange_9_obj.runSteadyScheduleWork();
			CompareExchange_10_obj.runSteadyScheduleWork();
			join_11_obj.runSteadyScheduleWork();
			Roundrobin_12_obj.runSteadyScheduleWork();
			CompareExchange_13_obj.runSteadyScheduleWork();
			CompareExchange_14_obj.runSteadyScheduleWork();
			join_15_obj.runSteadyScheduleWork();
			join_16_obj.runSteadyScheduleWork();
			Roundrobin_17_obj.runSteadyScheduleWork();
			CompareExchange_18_obj.runSteadyScheduleWork();
			CompareExchange_19_obj.runSteadyScheduleWork();
			CompareExchange_20_obj.runSteadyScheduleWork();
			CompareExchange_21_obj.runSteadyScheduleWork();
			join_22_obj.runSteadyScheduleWork();
			Roundrobin_23_obj.runSteadyScheduleWork();
			Roundrobin_24_obj.runSteadyScheduleWork();
			CompareExchange_26_obj.runSteadyScheduleWork();
		}
		for(int index=2; index>= 1; --index)
			stage[index] = stage[index-1];
		if(_stageNum == (MAX_ITER - 1 + 3))
		{
			stage[0]=0;
		}
	
		masterSync(2);
	}
}
