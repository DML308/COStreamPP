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
	LowPassFilterX_41 LowPassFilterX_41_obj(LowPassFilterX_41_join_43,Duplicate_40_LowPassFilterX_41);
	join_43 join_43_obj(join_43_Subtracter_44,LowPassFilterX_41_join_43,LowPassFilterX_42_join_43);
	Subtracter_44 Subtracter_44_obj(Subtracter_44_AmplifyX_45,join_43_Subtracter_44);
	LowPassFilterX_47 LowPassFilterX_47_obj(LowPassFilterX_47_join_49,Duplicate_46_LowPassFilterX_47);
	LowPassFilterX_48 LowPassFilterX_48_obj(LowPassFilterX_48_join_49,Duplicate_46_LowPassFilterX_48);
	join_49 join_49_obj(join_49_Subtracter_50,LowPassFilterX_47_join_49,LowPassFilterX_48_join_49);
	Subtracter_50 Subtracter_50_obj(Subtracter_50_AmplifyX_51,join_49_Subtracter_50);
	Duplicate_52 Duplicate_52_obj(Duplicate_52_LowPassFilterX_53,Duplicate_52_LowPassFilterX_54,Duplicate_3_Duplicate_52);
	LowPassFilterX_53 LowPassFilterX_53_obj(LowPassFilterX_53_join_55,Duplicate_52_LowPassFilterX_53);
	LowPassFilterX_54 LowPassFilterX_54_obj(LowPassFilterX_54_join_55,Duplicate_52_LowPassFilterX_54);
	join_55 join_55_obj(join_55_Subtracter_56,LowPassFilterX_53_join_55,LowPassFilterX_54_join_55);
	Subtracter_56 Subtracter_56_obj(Subtracter_56_AmplifyX_57,join_55_Subtracter_56);
	Duplicate_58 Duplicate_58_obj(Duplicate_58_LowPassFilterX_59,Duplicate_58_LowPassFilterX_60,Duplicate_3_Duplicate_58);
	LowPassFilterX_59 LowPassFilterX_59_obj(LowPassFilterX_59_join_61,Duplicate_58_LowPassFilterX_59);
	LowPassFilterX_60 LowPassFilterX_60_obj(LowPassFilterX_60_join_61,Duplicate_58_LowPassFilterX_60);
	join_61 join_61_obj(join_61_Subtracter_62,LowPassFilterX_59_join_61,LowPassFilterX_60_join_61);
	Subtracter_62 Subtracter_62_obj(Subtracter_62_AmplifyX_63,join_61_Subtracter_62);
	char stage[7]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<7;_stageNum++)
	{
		if(4==_stageNum)
		{
			join_43_obj.runInitScheduleWork();
			Subtracter_44_obj.runInitScheduleWork();
		}
		if(3==_stageNum)
		{
			LowPassFilterX_41_obj.runInitScheduleWork();
		}
		if(2==_stageNum)
		{
			LowPassFilterX_47_obj.runInitScheduleWork();
			LowPassFilterX_48_obj.runInitScheduleWork();
			join_49_obj.runInitScheduleWork();
			Subtracter_50_obj.runInitScheduleWork();
			Duplicate_52_obj.runInitScheduleWork();
			LowPassFilterX_53_obj.runInitScheduleWork();
			LowPassFilterX_54_obj.runInitScheduleWork();
			join_55_obj.runInitScheduleWork();
			Subtracter_56_obj.runInitScheduleWork();
			Duplicate_58_obj.runInitScheduleWork();
			LowPassFilterX_59_obj.runInitScheduleWork();
			LowPassFilterX_60_obj.runInitScheduleWork();
			join_61_obj.runInitScheduleWork();
			Subtracter_62_obj.runInitScheduleWork();
		}
		if(0==_stageNum)
		{
			Source_0_obj.runInitScheduleWork();
		}
	
		workerSync(2);
	}
	for(int _stageNum=7;_stageNum<2*7+MAX_ITER-1;_stageNum++)
	{
		if(stage[4])
		{
			join_43_obj.runSteadyScheduleWork();
			Subtracter_44_obj.runSteadyScheduleWork();
		}
		if(stage[3])
		{
			LowPassFilterX_41_obj.runSteadyScheduleWork();
		}
		if(stage[2])
		{
			LowPassFilterX_47_obj.runSteadyScheduleWork();
			LowPassFilterX_48_obj.runSteadyScheduleWork();
			join_49_obj.runSteadyScheduleWork();
			Subtracter_50_obj.runSteadyScheduleWork();
			Duplicate_52_obj.runSteadyScheduleWork();
			LowPassFilterX_53_obj.runSteadyScheduleWork();
			LowPassFilterX_54_obj.runSteadyScheduleWork();
			join_55_obj.runSteadyScheduleWork();
			Subtracter_56_obj.runSteadyScheduleWork();
			Duplicate_58_obj.runSteadyScheduleWork();
			LowPassFilterX_59_obj.runSteadyScheduleWork();
			LowPassFilterX_60_obj.runSteadyScheduleWork();
			join_61_obj.runSteadyScheduleWork();
			Subtracter_62_obj.runSteadyScheduleWork();
		}
		if(stage[0])
		{
			Source_0_obj.runSteadyScheduleWork();
		}
		for(int index=6; index>= 1; --index)
			stage[index] = stage[index-1];
		if(_stageNum == (MAX_ITER - 1 + 7))
		{
			stage[0]=0;
		}
	
		workerSync(2);
	}
}
