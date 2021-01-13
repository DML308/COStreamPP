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
	join_43 join_43_obj(join_43_Subtracter_44,LowPassFilterX_41_join_43,LowPassFilterX_42_join_43);
	Subtracter_44 Subtracter_44_obj(Subtracter_44_AmplifyX_45,join_43_Subtracter_44);
	LowPassFilterX_47 LowPassFilterX_47_obj(LowPassFilterX_47_join_49,Duplicate_46_LowPassFilterX_47);
	LowPassFilterX_48 LowPassFilterX_48_obj(LowPassFilterX_48_join_49,Duplicate_46_LowPassFilterX_48);
	LowPassFilterX_53 LowPassFilterX_53_obj(LowPassFilterX_53_join_55,Duplicate_52_LowPassFilterX_53);
	LowPassFilterX_54 LowPassFilterX_54_obj(LowPassFilterX_54_join_55,Duplicate_52_LowPassFilterX_54);
	join_55 join_55_obj(join_55_Subtracter_56,LowPassFilterX_53_join_55,LowPassFilterX_54_join_55);
	Subtracter_56 Subtracter_56_obj(Subtracter_56_AmplifyX_57,join_55_Subtracter_56);
	LowPassFilterX_59 LowPassFilterX_59_obj(LowPassFilterX_59_join_61,Duplicate_58_LowPassFilterX_59);
	LowPassFilterX_60 LowPassFilterX_60_obj(LowPassFilterX_60_join_61,Duplicate_58_LowPassFilterX_60);
	char stage[4]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<4;_stageNum++)
	{
		if(2==_stageNum)
		{
			join_43_obj.runInitScheduleWork();
			Subtracter_44_obj.runInitScheduleWork();
		}
		if(1==_stageNum)
		{
			LowPassFilterX_47_obj.runInitScheduleWork();
			LowPassFilterX_48_obj.runInitScheduleWork();
			LowPassFilterX_53_obj.runInitScheduleWork();
			LowPassFilterX_54_obj.runInitScheduleWork();
			join_55_obj.runInitScheduleWork();
			Subtracter_56_obj.runInitScheduleWork();
			LowPassFilterX_59_obj.runInitScheduleWork();
			LowPassFilterX_60_obj.runInitScheduleWork();
		}
	
		workerSync(1);
	}
	for(int _stageNum=4;_stageNum<2*4+MAX_ITER-1;_stageNum++)
	{
		if(stage[2])
		{
			join_43_obj.runSteadyScheduleWork();
			Subtracter_44_obj.runSteadyScheduleWork();
		}
		if(stage[1])
		{
			LowPassFilterX_47_obj.runSteadyScheduleWork();
			LowPassFilterX_48_obj.runSteadyScheduleWork();
			LowPassFilterX_53_obj.runSteadyScheduleWork();
			LowPassFilterX_54_obj.runSteadyScheduleWork();
			join_55_obj.runSteadyScheduleWork();
			Subtracter_56_obj.runSteadyScheduleWork();
			LowPassFilterX_59_obj.runSteadyScheduleWork();
			LowPassFilterX_60_obj.runSteadyScheduleWork();
		}
		for(int index=3; index>= 1; --index)
			stage[index] = stage[index-1];
		if(_stageNum == (MAX_ITER - 1 + 4))
		{
			stage[0]=0;
		}
	
		workerSync(1);
	}
}
