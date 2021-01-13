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
void thread_1_fun()
{
	workerSync(1);
	Source_0 Source_0_obj(Source_0_LowPassFilterX_1);
	LowPassFilterX_1 LowPassFilterX_1_obj(LowPassFilterX_1_FMDemodulatorX_2,Source_0_LowPassFilterX_1);
	FMDemodulatorX_2 FMDemodulatorX_2_obj(FMDemodulatorX_2_Duplicate_3,LowPassFilterX_1_FMDemodulatorX_2);
	Duplicate_3 Duplicate_3_obj(Duplicate_3_Duplicate_4,Duplicate_3_Duplicate_10,Duplicate_3_Duplicate_16,Duplicate_3_Duplicate_22,Duplicate_3_Duplicate_28,Duplicate_3_Duplicate_34,Duplicate_3_Duplicate_40,Duplicate_3_Duplicate_46,Duplicate_3_Duplicate_52,Duplicate_3_Duplicate_58,FMDemodulatorX_2_Duplicate_3);
	Duplicate_4 Duplicate_4_obj(Duplicate_4_LowPassFilterX_5,Duplicate_4_LowPassFilterX_6,Duplicate_3_Duplicate_4);
	LowPassFilterX_5 LowPassFilterX_5_obj(LowPassFilterX_5_join_7,Duplicate_4_LowPassFilterX_5);
	LowPassFilterX_6 LowPassFilterX_6_obj(LowPassFilterX_6_join_7,Duplicate_4_LowPassFilterX_6);
	join_7 join_7_obj(join_7_Subtracter_8,LowPassFilterX_5_join_7,LowPassFilterX_6_join_7);
	Subtracter_8 Subtracter_8_obj(Subtracter_8_AmplifyX_9,join_7_Subtracter_8);
	AmplifyX_9 AmplifyX_9_obj(AmplifyX_9_join_64,Subtracter_8_AmplifyX_9);
	Duplicate_10 Duplicate_10_obj(Duplicate_10_LowPassFilterX_11,Duplicate_10_LowPassFilterX_12,Duplicate_3_Duplicate_10);
	LowPassFilterX_11 LowPassFilterX_11_obj(LowPassFilterX_11_join_13,Duplicate_10_LowPassFilterX_11);
	LowPassFilterX_12 LowPassFilterX_12_obj(LowPassFilterX_12_join_13,Duplicate_10_LowPassFilterX_12);
	join_13 join_13_obj(join_13_Subtracter_14,LowPassFilterX_11_join_13,LowPassFilterX_12_join_13);
	Subtracter_14 Subtracter_14_obj(Subtracter_14_AmplifyX_15,join_13_Subtracter_14);
	AmplifyX_15 AmplifyX_15_obj(AmplifyX_15_join_64,Subtracter_14_AmplifyX_15);
	Duplicate_16 Duplicate_16_obj(Duplicate_16_LowPassFilterX_17,Duplicate_16_LowPassFilterX_18,Duplicate_3_Duplicate_16);
	LowPassFilterX_17 LowPassFilterX_17_obj(LowPassFilterX_17_join_19,Duplicate_16_LowPassFilterX_17);
	LowPassFilterX_18 LowPassFilterX_18_obj(LowPassFilterX_18_join_19,Duplicate_16_LowPassFilterX_18);
	join_19 join_19_obj(join_19_Subtracter_20,LowPassFilterX_17_join_19,LowPassFilterX_18_join_19);
	Subtracter_20 Subtracter_20_obj(Subtracter_20_AmplifyX_21,join_19_Subtracter_20);
	AmplifyX_21 AmplifyX_21_obj(AmplifyX_21_join_64,Subtracter_20_AmplifyX_21);
	Duplicate_22 Duplicate_22_obj(Duplicate_22_LowPassFilterX_23,Duplicate_22_LowPassFilterX_24,Duplicate_3_Duplicate_22);
	LowPassFilterX_23 LowPassFilterX_23_obj(LowPassFilterX_23_join_25,Duplicate_22_LowPassFilterX_23);
	LowPassFilterX_24 LowPassFilterX_24_obj(LowPassFilterX_24_join_25,Duplicate_22_LowPassFilterX_24);
	join_25 join_25_obj(join_25_Subtracter_26,LowPassFilterX_23_join_25,LowPassFilterX_24_join_25);
	Subtracter_26 Subtracter_26_obj(Subtracter_26_AmplifyX_27,join_25_Subtracter_26);
	AmplifyX_27 AmplifyX_27_obj(AmplifyX_27_join_64,Subtracter_26_AmplifyX_27);
	Duplicate_28 Duplicate_28_obj(Duplicate_28_LowPassFilterX_29,Duplicate_28_LowPassFilterX_30,Duplicate_3_Duplicate_28);
	LowPassFilterX_29 LowPassFilterX_29_obj(LowPassFilterX_29_join_31,Duplicate_28_LowPassFilterX_29);
	AmplifyX_33 AmplifyX_33_obj(AmplifyX_33_join_64,Subtracter_32_AmplifyX_33);
	Duplicate_34 Duplicate_34_obj(Duplicate_34_LowPassFilterX_35,Duplicate_34_LowPassFilterX_36,Duplicate_3_Duplicate_34);
	join_64 join_64_obj(join_64_AnnoyX_65,AmplifyX_9_join_64,AmplifyX_15_join_64,AmplifyX_21_join_64,AmplifyX_27_join_64,AmplifyX_33_join_64,AmplifyX_39_join_64,AmplifyX_45_join_64,AmplifyX_51_join_64,AmplifyX_57_join_64,AmplifyX_63_join_64);
	AnnoyX_65 AnnoyX_65_obj(AnnoyX_65_Sink_66,join_64_AnnoyX_65);
	Sink_66 Sink_66_obj(AnnoyX_65_Sink_66);
	char stage[5]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<5;_stageNum++)
	{
		if(4==_stageNum)
		{
			join_64_obj.runInitScheduleWork();
			AnnoyX_65_obj.runInitScheduleWork();
			Sink_66_obj.runInitScheduleWork();
		}
		if(3==_stageNum)
		{
			AmplifyX_33_obj.runInitScheduleWork();
		}
		if(0==_stageNum)
		{
			Source_0_obj.runInitScheduleWork();
			LowPassFilterX_1_obj.runInitScheduleWork();
			FMDemodulatorX_2_obj.runInitScheduleWork();
			Duplicate_3_obj.runInitScheduleWork();
			Duplicate_4_obj.runInitScheduleWork();
			LowPassFilterX_5_obj.runInitScheduleWork();
			LowPassFilterX_6_obj.runInitScheduleWork();
			join_7_obj.runInitScheduleWork();
			Subtracter_8_obj.runInitScheduleWork();
			AmplifyX_9_obj.runInitScheduleWork();
			Duplicate_10_obj.runInitScheduleWork();
			LowPassFilterX_11_obj.runInitScheduleWork();
			LowPassFilterX_12_obj.runInitScheduleWork();
			join_13_obj.runInitScheduleWork();
			Subtracter_14_obj.runInitScheduleWork();
			AmplifyX_15_obj.runInitScheduleWork();
			Duplicate_16_obj.runInitScheduleWork();
			LowPassFilterX_17_obj.runInitScheduleWork();
			LowPassFilterX_18_obj.runInitScheduleWork();
			join_19_obj.runInitScheduleWork();
			Subtracter_20_obj.runInitScheduleWork();
			AmplifyX_21_obj.runInitScheduleWork();
			Duplicate_22_obj.runInitScheduleWork();
			LowPassFilterX_23_obj.runInitScheduleWork();
			LowPassFilterX_24_obj.runInitScheduleWork();
			join_25_obj.runInitScheduleWork();
			Subtracter_26_obj.runInitScheduleWork();
			AmplifyX_27_obj.runInitScheduleWork();
			Duplicate_28_obj.runInitScheduleWork();
			LowPassFilterX_29_obj.runInitScheduleWork();
			Duplicate_34_obj.runInitScheduleWork();
		}
	
		workerSync(1);
	}
	for(int _stageNum=5;_stageNum<2*5+MAX_ITER-1;_stageNum++)
	{
		if(stage[4])
		{
			join_64_obj.runSteadyScheduleWork();
			AnnoyX_65_obj.runSteadyScheduleWork();
			Sink_66_obj.runSteadyScheduleWork();
		}
		if(stage[3])
		{
			AmplifyX_33_obj.runSteadyScheduleWork();
		}
		if(stage[0])
		{
			Source_0_obj.runSteadyScheduleWork();
			LowPassFilterX_1_obj.runSteadyScheduleWork();
			FMDemodulatorX_2_obj.runSteadyScheduleWork();
			Duplicate_3_obj.runSteadyScheduleWork();
			Duplicate_4_obj.runSteadyScheduleWork();
			LowPassFilterX_5_obj.runSteadyScheduleWork();
			LowPassFilterX_6_obj.runSteadyScheduleWork();
			join_7_obj.runSteadyScheduleWork();
			Subtracter_8_obj.runSteadyScheduleWork();
			AmplifyX_9_obj.runSteadyScheduleWork();
			Duplicate_10_obj.runSteadyScheduleWork();
			LowPassFilterX_11_obj.runSteadyScheduleWork();
			LowPassFilterX_12_obj.runSteadyScheduleWork();
			join_13_obj.runSteadyScheduleWork();
			Subtracter_14_obj.runSteadyScheduleWork();
			AmplifyX_15_obj.runSteadyScheduleWork();
			Duplicate_16_obj.runSteadyScheduleWork();
			LowPassFilterX_17_obj.runSteadyScheduleWork();
			LowPassFilterX_18_obj.runSteadyScheduleWork();
			join_19_obj.runSteadyScheduleWork();
			Subtracter_20_obj.runSteadyScheduleWork();
			AmplifyX_21_obj.runSteadyScheduleWork();
			Duplicate_22_obj.runSteadyScheduleWork();
			LowPassFilterX_23_obj.runSteadyScheduleWork();
			LowPassFilterX_24_obj.runSteadyScheduleWork();
			join_25_obj.runSteadyScheduleWork();
			Subtracter_26_obj.runSteadyScheduleWork();
			AmplifyX_27_obj.runSteadyScheduleWork();
			Duplicate_28_obj.runSteadyScheduleWork();
			LowPassFilterX_29_obj.runSteadyScheduleWork();
			Duplicate_34_obj.runSteadyScheduleWork();
		}
		for(int index=4; index>= 1; --index)
			stage[index] = stage[index-1];
		if(_stageNum == (MAX_ITER - 1 + 5))
		{
			stage[0]=0;
		}
	
		workerSync(1);
	}
}
