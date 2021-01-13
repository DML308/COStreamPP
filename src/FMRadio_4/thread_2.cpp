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
void thread_2_fun()
{
	workerSync(2);
	Source_0 Source_0_obj(Source_0_LowPassFilterX_1);
	LowPassFilterX_1 LowPassFilterX_1_obj(LowPassFilterX_1_FMDemodulatorX_2,Source_0_LowPassFilterX_1);
	FMDemodulatorX_2 FMDemodulatorX_2_obj(FMDemodulatorX_2_Duplicate_3,LowPassFilterX_1_FMDemodulatorX_2);
	Duplicate_3 Duplicate_3_obj(Duplicate_3_Duplicate_4,Duplicate_3_Duplicate_10,Duplicate_3_Duplicate_16,Duplicate_3_Duplicate_22,Duplicate_3_Duplicate_28,Duplicate_3_Duplicate_34,Duplicate_3_Duplicate_40,Duplicate_3_Duplicate_46,Duplicate_3_Duplicate_52,Duplicate_3_Duplicate_58,FMDemodulatorX_2_Duplicate_3);
	Duplicate_4 Duplicate_4_obj(Duplicate_4_LowPassFilterX_5,Duplicate_4_LowPassFilterX_6,Duplicate_3_Duplicate_4);
	LowPassFilterX_5 LowPassFilterX_5_obj(LowPassFilterX_5_join_7,Duplicate_4_LowPassFilterX_5);
	LowPassFilterX_6 LowPassFilterX_6_obj(LowPassFilterX_6_join_7,Duplicate_4_LowPassFilterX_6);
	join_7 join_7_obj(join_7_Subtracter_8,LowPassFilterX_5_join_7,LowPassFilterX_6_join_7);
	Subtracter_8 Subtracter_8_obj(Subtracter_8_AmplifyX_9,join_7_Subtracter_8);
	Duplicate_10 Duplicate_10_obj(Duplicate_10_LowPassFilterX_11,Duplicate_10_LowPassFilterX_12,Duplicate_3_Duplicate_10);
	LowPassFilterX_11 LowPassFilterX_11_obj(LowPassFilterX_11_join_13,Duplicate_10_LowPassFilterX_11);
	LowPassFilterX_12 LowPassFilterX_12_obj(LowPassFilterX_12_join_13,Duplicate_10_LowPassFilterX_12);
	Duplicate_46 Duplicate_46_obj(Duplicate_46_LowPassFilterX_47,Duplicate_46_LowPassFilterX_48,Duplicate_3_Duplicate_46);
	Duplicate_52 Duplicate_52_obj(Duplicate_52_LowPassFilterX_53,Duplicate_52_LowPassFilterX_54,Duplicate_3_Duplicate_52);
	Duplicate_58 Duplicate_58_obj(Duplicate_58_LowPassFilterX_59,Duplicate_58_LowPassFilterX_60,Duplicate_3_Duplicate_58);
	char stage[4]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<4;_stageNum++)
	{
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
			Duplicate_10_obj.runInitScheduleWork();
			LowPassFilterX_11_obj.runInitScheduleWork();
			LowPassFilterX_12_obj.runInitScheduleWork();
			Duplicate_46_obj.runInitScheduleWork();
			Duplicate_52_obj.runInitScheduleWork();
			Duplicate_58_obj.runInitScheduleWork();
		}
	
		workerSync(2);
	}
	for(int _stageNum=4;_stageNum<2*4+MAX_ITER-1;_stageNum++)
	{
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
			Duplicate_10_obj.runSteadyScheduleWork();
			LowPassFilterX_11_obj.runSteadyScheduleWork();
			LowPassFilterX_12_obj.runSteadyScheduleWork();
			Duplicate_46_obj.runSteadyScheduleWork();
			Duplicate_52_obj.runSteadyScheduleWork();
			Duplicate_58_obj.runSteadyScheduleWork();
		}
		for(int index=3; index>= 1; --index)
			stage[index] = stage[index-1];
		if(_stageNum == (MAX_ITER - 1 + 4))
		{
			stage[0]=0;
		}
	
		workerSync(2);
	}
}
