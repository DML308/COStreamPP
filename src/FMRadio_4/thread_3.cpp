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
void thread_3_fun()
{
	workerSync(3);
	AmplifyX_9 AmplifyX_9_obj(AmplifyX_9_join_64,Subtracter_8_AmplifyX_9);
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
	join_31 join_31_obj(join_31_Subtracter_32,LowPassFilterX_29_join_31,LowPassFilterX_30_join_31);
	Subtracter_32 Subtracter_32_obj(Subtracter_32_AmplifyX_33,join_31_Subtracter_32);
	AmplifyX_33 AmplifyX_33_obj(AmplifyX_33_join_64,Subtracter_32_AmplifyX_33);
	join_37 join_37_obj(join_37_Subtracter_38,LowPassFilterX_35_join_37,LowPassFilterX_36_join_37);
	Subtracter_38 Subtracter_38_obj(Subtracter_38_AmplifyX_39,join_37_Subtracter_38);
	AmplifyX_39 AmplifyX_39_obj(AmplifyX_39_join_64,Subtracter_38_AmplifyX_39);
	AmplifyX_45 AmplifyX_45_obj(AmplifyX_45_join_64,Subtracter_44_AmplifyX_45);
	join_49 join_49_obj(join_49_Subtracter_50,LowPassFilterX_47_join_49,LowPassFilterX_48_join_49);
	Subtracter_50 Subtracter_50_obj(Subtracter_50_AmplifyX_51,join_49_Subtracter_50);
	AmplifyX_51 AmplifyX_51_obj(AmplifyX_51_join_64,Subtracter_50_AmplifyX_51);
	AmplifyX_57 AmplifyX_57_obj(AmplifyX_57_join_64,Subtracter_56_AmplifyX_57);
	join_61 join_61_obj(join_61_Subtracter_62,LowPassFilterX_59_join_61,LowPassFilterX_60_join_61);
	Subtracter_62 Subtracter_62_obj(Subtracter_62_AmplifyX_63,join_61_Subtracter_62);
	AmplifyX_63 AmplifyX_63_obj(AmplifyX_63_join_64,Subtracter_62_AmplifyX_63);
	join_64 join_64_obj(join_64_AnnoyX_65,AmplifyX_9_join_64,AmplifyX_15_join_64,AmplifyX_21_join_64,AmplifyX_27_join_64,AmplifyX_33_join_64,AmplifyX_39_join_64,AmplifyX_45_join_64,AmplifyX_51_join_64,AmplifyX_57_join_64,AmplifyX_63_join_64);
	AnnoyX_65 AnnoyX_65_obj(AnnoyX_65_Sink_66,join_64_AnnoyX_65);
	Sink_66 Sink_66_obj(AnnoyX_65_Sink_66);
	char stage[4]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<4;_stageNum++)
	{
		if(3==_stageNum)
		{
			AmplifyX_45_obj.runInitScheduleWork();
			join_64_obj.runInitScheduleWork();
			AnnoyX_65_obj.runInitScheduleWork();
			Sink_66_obj.runInitScheduleWork();
		}
		if(2==_stageNum)
		{
			join_31_obj.runInitScheduleWork();
			Subtracter_32_obj.runInitScheduleWork();
			AmplifyX_33_obj.runInitScheduleWork();
			join_37_obj.runInitScheduleWork();
			Subtracter_38_obj.runInitScheduleWork();
			AmplifyX_39_obj.runInitScheduleWork();
			join_49_obj.runInitScheduleWork();
			Subtracter_50_obj.runInitScheduleWork();
			AmplifyX_51_obj.runInitScheduleWork();
			AmplifyX_57_obj.runInitScheduleWork();
			join_61_obj.runInitScheduleWork();
			Subtracter_62_obj.runInitScheduleWork();
			AmplifyX_63_obj.runInitScheduleWork();
		}
		if(1==_stageNum)
		{
			AmplifyX_9_obj.runInitScheduleWork();
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
		}
	
		workerSync(3);
	}
	for(int _stageNum=4;_stageNum<2*4+MAX_ITER-1;_stageNum++)
	{
		if(stage[3])
		{
			AmplifyX_45_obj.runSteadyScheduleWork();
			join_64_obj.runSteadyScheduleWork();
			AnnoyX_65_obj.runSteadyScheduleWork();
			Sink_66_obj.runSteadyScheduleWork();
		}
		if(stage[2])
		{
			join_31_obj.runSteadyScheduleWork();
			Subtracter_32_obj.runSteadyScheduleWork();
			AmplifyX_33_obj.runSteadyScheduleWork();
			join_37_obj.runSteadyScheduleWork();
			Subtracter_38_obj.runSteadyScheduleWork();
			AmplifyX_39_obj.runSteadyScheduleWork();
			join_49_obj.runSteadyScheduleWork();
			Subtracter_50_obj.runSteadyScheduleWork();
			AmplifyX_51_obj.runSteadyScheduleWork();
			AmplifyX_57_obj.runSteadyScheduleWork();
			join_61_obj.runSteadyScheduleWork();
			Subtracter_62_obj.runSteadyScheduleWork();
			AmplifyX_63_obj.runSteadyScheduleWork();
		}
		if(stage[1])
		{
			AmplifyX_9_obj.runSteadyScheduleWork();
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
		}
		for(int index=3; index>= 1; --index)
			stage[index] = stage[index-1];
		if(_stageNum == (MAX_ITER - 1 + 4))
		{
			stage[0]=0;
		}
	
		workerSync(3);
	}
}
