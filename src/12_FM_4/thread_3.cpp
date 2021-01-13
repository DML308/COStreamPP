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
	Duplicate_22 Duplicate_22_obj(Duplicate_22_LowPassFilterX_23,Duplicate_22_LowPassFilterX_24,Duplicate_3_Duplicate_22);
	LowPassFilterX_23 LowPassFilterX_23_obj(LowPassFilterX_23_join_25,Duplicate_22_LowPassFilterX_23);
	LowPassFilterX_24 LowPassFilterX_24_obj(LowPassFilterX_24_join_25,Duplicate_22_LowPassFilterX_24);
	join_25 join_25_obj(join_25_Subtracter_26,LowPassFilterX_23_join_25,LowPassFilterX_24_join_25);
	Subtracter_26 Subtracter_26_obj(Subtracter_26_AmplifyX_27,join_25_Subtracter_26);
	Duplicate_28 Duplicate_28_obj(Duplicate_28_LowPassFilterX_29,Duplicate_28_LowPassFilterX_30,Duplicate_3_Duplicate_28);
	LowPassFilterX_29 LowPassFilterX_29_obj(LowPassFilterX_29_join_31,Duplicate_28_LowPassFilterX_29);
	LowPassFilterX_30 LowPassFilterX_30_obj(LowPassFilterX_30_join_31,Duplicate_28_LowPassFilterX_30);
	join_31 join_31_obj(join_31_Subtracter_32,LowPassFilterX_29_join_31,LowPassFilterX_30_join_31);
	Subtracter_32 Subtracter_32_obj(Subtracter_32_AmplifyX_33,join_31_Subtracter_32);
	Duplicate_34 Duplicate_34_obj(Duplicate_34_LowPassFilterX_35,Duplicate_34_LowPassFilterX_36,Duplicate_3_Duplicate_34);
	LowPassFilterX_35 LowPassFilterX_35_obj(LowPassFilterX_35_join_37,Duplicate_34_LowPassFilterX_35);
	LowPassFilterX_36 LowPassFilterX_36_obj(LowPassFilterX_36_join_37,Duplicate_34_LowPassFilterX_36);
	join_37 join_37_obj(join_37_Subtracter_38,LowPassFilterX_35_join_37,LowPassFilterX_36_join_37);
	Subtracter_38 Subtracter_38_obj(Subtracter_38_AmplifyX_39,join_37_Subtracter_38);
	Duplicate_40 Duplicate_40_obj(Duplicate_40_LowPassFilterX_41,Duplicate_40_LowPassFilterX_42,Duplicate_3_Duplicate_40);
	LowPassFilterX_42 LowPassFilterX_42_obj(LowPassFilterX_42_join_43,Duplicate_40_LowPassFilterX_42);
	char stage[7]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<7;_stageNum++)
	{
		if(2==_stageNum)
		{
			Duplicate_22_obj.runInitScheduleWork();
			LowPassFilterX_23_obj.runInitScheduleWork();
			LowPassFilterX_24_obj.runInitScheduleWork();
			join_25_obj.runInitScheduleWork();
			Subtracter_26_obj.runInitScheduleWork();
			Duplicate_28_obj.runInitScheduleWork();
			LowPassFilterX_29_obj.runInitScheduleWork();
			LowPassFilterX_30_obj.runInitScheduleWork();
			join_31_obj.runInitScheduleWork();
			Subtracter_32_obj.runInitScheduleWork();
			Duplicate_34_obj.runInitScheduleWork();
			LowPassFilterX_35_obj.runInitScheduleWork();
			LowPassFilterX_36_obj.runInitScheduleWork();
			join_37_obj.runInitScheduleWork();
			Subtracter_38_obj.runInitScheduleWork();
			Duplicate_40_obj.runInitScheduleWork();
			LowPassFilterX_42_obj.runInitScheduleWork();
		}
	
		workerSync(3);
	}
	for(int _stageNum=7;_stageNum<2*7+MAX_ITER-1;_stageNum++)
	{
		if(stage[2])
		{
			Duplicate_22_obj.runSteadyScheduleWork();
			LowPassFilterX_23_obj.runSteadyScheduleWork();
			LowPassFilterX_24_obj.runSteadyScheduleWork();
			join_25_obj.runSteadyScheduleWork();
			Subtracter_26_obj.runSteadyScheduleWork();
			Duplicate_28_obj.runSteadyScheduleWork();
			LowPassFilterX_29_obj.runSteadyScheduleWork();
			LowPassFilterX_30_obj.runSteadyScheduleWork();
			join_31_obj.runSteadyScheduleWork();
			Subtracter_32_obj.runSteadyScheduleWork();
			Duplicate_34_obj.runSteadyScheduleWork();
			LowPassFilterX_35_obj.runSteadyScheduleWork();
			LowPassFilterX_36_obj.runSteadyScheduleWork();
			join_37_obj.runSteadyScheduleWork();
			Subtracter_38_obj.runSteadyScheduleWork();
			Duplicate_40_obj.runSteadyScheduleWork();
			LowPassFilterX_42_obj.runSteadyScheduleWork();
		}
		for(int index=6; index>= 1; --index)
			stage[index] = stage[index-1];
		if(_stageNum == (MAX_ITER - 1 + 7))
		{
			stage[0]=0;
		}
	
		workerSync(3);
	}
}
