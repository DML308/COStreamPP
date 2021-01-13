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
	join_13 join_13_obj(join_13_Subtracter_14,LowPassFilterX_11_join_13,LowPassFilterX_12_join_13);
	Subtracter_14 Subtracter_14_obj(Subtracter_14_AmplifyX_15,join_13_Subtracter_14);
	AmplifyX_15 AmplifyX_15_obj(AmplifyX_15_join_64,Subtracter_14_AmplifyX_15);
	Duplicate_16 Duplicate_16_obj(Duplicate_16_LowPassFilterX_17,Duplicate_16_LowPassFilterX_18,Duplicate_3_Duplicate_16);
	LowPassFilterX_17 LowPassFilterX_17_obj(LowPassFilterX_17_join_19,Duplicate_16_LowPassFilterX_17);
	LowPassFilterX_18 LowPassFilterX_18_obj(LowPassFilterX_18_join_19,Duplicate_16_LowPassFilterX_18);
	join_19 join_19_obj(join_19_Subtracter_20,LowPassFilterX_17_join_19,LowPassFilterX_18_join_19);
	Subtracter_20 Subtracter_20_obj(Subtracter_20_AmplifyX_21,join_19_Subtracter_20);
	AmplifyX_21 AmplifyX_21_obj(AmplifyX_21_join_64,Subtracter_20_AmplifyX_21);
	AmplifyX_27 AmplifyX_27_obj(AmplifyX_27_join_64,Subtracter_26_AmplifyX_27);
	AmplifyX_33 AmplifyX_33_obj(AmplifyX_33_join_64,Subtracter_32_AmplifyX_33);
	AmplifyX_39 AmplifyX_39_obj(AmplifyX_39_join_64,Subtracter_38_AmplifyX_39);
	AmplifyX_45 AmplifyX_45_obj(AmplifyX_45_join_64,Subtracter_44_AmplifyX_45);
	AmplifyX_51 AmplifyX_51_obj(AmplifyX_51_join_64,Subtracter_50_AmplifyX_51);
	AmplifyX_57 AmplifyX_57_obj(AmplifyX_57_join_64,Subtracter_56_AmplifyX_57);
	AmplifyX_63 AmplifyX_63_obj(AmplifyX_63_join_64,Subtracter_62_AmplifyX_63);
	join_64 join_64_obj(join_64_AnnoyX_65,AmplifyX_9_join_64,AmplifyX_15_join_64,AmplifyX_21_join_64,AmplifyX_27_join_64,AmplifyX_33_join_64,AmplifyX_39_join_64,AmplifyX_45_join_64,AmplifyX_51_join_64,AmplifyX_57_join_64,AmplifyX_63_join_64);
	AnnoyX_65 AnnoyX_65_obj(AnnoyX_65_Sink_66,join_64_AnnoyX_65);
	Sink_66 Sink_66_obj(AnnoyX_65_Sink_66);
	char stage[7]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<7;_stageNum++)
	{
		if(6==_stageNum)
		{
			join_64_obj.runInitScheduleWork();
			AnnoyX_65_obj.runInitScheduleWork();
			Sink_66_obj.runInitScheduleWork();
		}
		if(5==_stageNum)
		{
			AmplifyX_45_obj.runInitScheduleWork();
		}
		if(3==_stageNum)
		{
			AmplifyX_27_obj.runInitScheduleWork();
			AmplifyX_33_obj.runInitScheduleWork();
			AmplifyX_39_obj.runInitScheduleWork();
			AmplifyX_51_obj.runInitScheduleWork();
			AmplifyX_57_obj.runInitScheduleWork();
			AmplifyX_63_obj.runInitScheduleWork();
		}
		if(2==_stageNum)
		{
			join_13_obj.runInitScheduleWork();
			Subtracter_14_obj.runInitScheduleWork();
			AmplifyX_15_obj.runInitScheduleWork();
			Duplicate_16_obj.runInitScheduleWork();
			LowPassFilterX_17_obj.runInitScheduleWork();
			LowPassFilterX_18_obj.runInitScheduleWork();
			join_19_obj.runInitScheduleWork();
			Subtracter_20_obj.runInitScheduleWork();
			AmplifyX_21_obj.runInitScheduleWork();
		}
	
		workerSync(1);
	}
	for(int _stageNum=7;_stageNum<2*7+MAX_ITER-1;_stageNum++)
	{
		if(stage[6])
		{
			join_64_obj.runSteadyScheduleWork();
			AnnoyX_65_obj.runSteadyScheduleWork();
			Sink_66_obj.runSteadyScheduleWork();
		}
		if(stage[5])
		{
			AmplifyX_45_obj.runSteadyScheduleWork();
		}
		if(stage[3])
		{
			AmplifyX_27_obj.runSteadyScheduleWork();
			AmplifyX_33_obj.runSteadyScheduleWork();
			AmplifyX_39_obj.runSteadyScheduleWork();
			AmplifyX_51_obj.runSteadyScheduleWork();
			AmplifyX_57_obj.runSteadyScheduleWork();
			AmplifyX_63_obj.runSteadyScheduleWork();
		}
		if(stage[2])
		{
			join_13_obj.runSteadyScheduleWork();
			Subtracter_14_obj.runSteadyScheduleWork();
			AmplifyX_15_obj.runSteadyScheduleWork();
			Duplicate_16_obj.runSteadyScheduleWork();
			LowPassFilterX_17_obj.runSteadyScheduleWork();
			LowPassFilterX_18_obj.runSteadyScheduleWork();
			join_19_obj.runSteadyScheduleWork();
			Subtracter_20_obj.runSteadyScheduleWork();
			AmplifyX_21_obj.runSteadyScheduleWork();
		}
		for(int index=6; index>= 1; --index)
			stage[index] = stage[index-1];
		if(_stageNum == (MAX_ITER - 1 + 7))
		{
			stage[0]=0;
		}
	
		workerSync(1);
	}
}
