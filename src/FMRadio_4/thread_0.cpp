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
	masterSync(4);
	Duplicate_28 Duplicate_28_obj(Duplicate_28_LowPassFilterX_29,Duplicate_28_LowPassFilterX_30,Duplicate_3_Duplicate_28);
	LowPassFilterX_29 LowPassFilterX_29_obj(LowPassFilterX_29_join_31,Duplicate_28_LowPassFilterX_29);
	LowPassFilterX_30 LowPassFilterX_30_obj(LowPassFilterX_30_join_31,Duplicate_28_LowPassFilterX_30);
	Duplicate_34 Duplicate_34_obj(Duplicate_34_LowPassFilterX_35,Duplicate_34_LowPassFilterX_36,Duplicate_3_Duplicate_34);
	LowPassFilterX_35 LowPassFilterX_35_obj(LowPassFilterX_35_join_37,Duplicate_34_LowPassFilterX_35);
	LowPassFilterX_36 LowPassFilterX_36_obj(LowPassFilterX_36_join_37,Duplicate_34_LowPassFilterX_36);
	Duplicate_40 Duplicate_40_obj(Duplicate_40_LowPassFilterX_41,Duplicate_40_LowPassFilterX_42,Duplicate_3_Duplicate_40);
	LowPassFilterX_41 LowPassFilterX_41_obj(LowPassFilterX_41_join_43,Duplicate_40_LowPassFilterX_41);
	LowPassFilterX_42 LowPassFilterX_42_obj(LowPassFilterX_42_join_43,Duplicate_40_LowPassFilterX_42);
	char stage[4]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<4;_stageNum++)
	{
		if(1==_stageNum)
		{
			Duplicate_28_obj.runInitScheduleWork();
			LowPassFilterX_29_obj.runInitScheduleWork();
			LowPassFilterX_30_obj.runInitScheduleWork();
			Duplicate_34_obj.runInitScheduleWork();
			LowPassFilterX_35_obj.runInitScheduleWork();
			LowPassFilterX_36_obj.runInitScheduleWork();
			Duplicate_40_obj.runInitScheduleWork();
			LowPassFilterX_41_obj.runInitScheduleWork();
			LowPassFilterX_42_obj.runInitScheduleWork();
		}
	
		masterSync(4);
	}
	for(int _stageNum=4;_stageNum<2*4+MAX_ITER-1;_stageNum++)
	{
		if(stage[1])
		{
			Duplicate_28_obj.runSteadyScheduleWork();
			LowPassFilterX_29_obj.runSteadyScheduleWork();
			LowPassFilterX_30_obj.runSteadyScheduleWork();
			Duplicate_34_obj.runSteadyScheduleWork();
			LowPassFilterX_35_obj.runSteadyScheduleWork();
			LowPassFilterX_36_obj.runSteadyScheduleWork();
			Duplicate_40_obj.runSteadyScheduleWork();
			LowPassFilterX_41_obj.runSteadyScheduleWork();
			LowPassFilterX_42_obj.runSteadyScheduleWork();
		}
		for(int index=3; index>= 1; --index)
			stage[index] = stage[index-1];
		if(_stageNum == (MAX_ITER - 1 + 4))
		{
			stage[0]=0;
		}
	
		masterSync(4);
	}
}
