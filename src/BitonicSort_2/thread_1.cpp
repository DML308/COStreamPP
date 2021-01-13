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
	CompareExchange_25 CompareExchange_25_obj(CompareExchange_25_join_29,Roundrobin_24_CompareExchange_25);
	CompareExchange_27 CompareExchange_27_obj(CompareExchange_27_join_29,Roundrobin_24_CompareExchange_27);
	CompareExchange_28 CompareExchange_28_obj(CompareExchange_28_join_29,Roundrobin_24_CompareExchange_28);
	join_29 join_29_obj(join_29_join_30,CompareExchange_25_join_29,CompareExchange_26_join_29,CompareExchange_27_join_29,CompareExchange_28_join_29);
	join_30 join_30_obj(join_30_Roundrobin_31,join_29_join_30);
	Roundrobin_31 Roundrobin_31_obj(Roundrobin_31_Roundrobin_32,Roundrobin_31_Roundrobin_36,join_30_Roundrobin_31);
	Roundrobin_32 Roundrobin_32_obj(Roundrobin_32_CompareExchange_33,Roundrobin_32_CompareExchange_34,Roundrobin_31_Roundrobin_32);
	CompareExchange_33 CompareExchange_33_obj(CompareExchange_33_join_35,Roundrobin_32_CompareExchange_33);
	CompareExchange_34 CompareExchange_34_obj(CompareExchange_34_join_35,Roundrobin_32_CompareExchange_34);
	join_35 join_35_obj(join_35_join_40,CompareExchange_33_join_35,CompareExchange_34_join_35);
	Roundrobin_36 Roundrobin_36_obj(Roundrobin_36_CompareExchange_37,Roundrobin_36_CompareExchange_38,Roundrobin_31_Roundrobin_36);
	CompareExchange_37 CompareExchange_37_obj(CompareExchange_37_join_39,Roundrobin_36_CompareExchange_37);
	CompareExchange_38 CompareExchange_38_obj(CompareExchange_38_join_39,Roundrobin_36_CompareExchange_38);
	join_39 join_39_obj(join_39_join_40,CompareExchange_37_join_39,CompareExchange_38_join_39);
	join_40 join_40_obj(join_40_Roundrobin_41,join_35_join_40,join_39_join_40);
	Roundrobin_41 Roundrobin_41_obj(Roundrobin_41_CompareExchange_42,Roundrobin_41_CompareExchange_43,Roundrobin_41_CompareExchange_44,Roundrobin_41_CompareExchange_45,join_40_Roundrobin_41);
	CompareExchange_42 CompareExchange_42_obj(CompareExchange_42_join_46,Roundrobin_41_CompareExchange_42);
	CompareExchange_43 CompareExchange_43_obj(CompareExchange_43_join_46,Roundrobin_41_CompareExchange_43);
	CompareExchange_44 CompareExchange_44_obj(CompareExchange_44_join_46,Roundrobin_41_CompareExchange_44);
	CompareExchange_45 CompareExchange_45_obj(CompareExchange_45_join_46,Roundrobin_41_CompareExchange_45);
	join_46 join_46_obj(join_46_Sink_47,CompareExchange_42_join_46,CompareExchange_43_join_46,CompareExchange_44_join_46,CompareExchange_45_join_46);
	Sink_47 Sink_47_obj(join_46_Sink_47);
	char stage[3]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<3;_stageNum++)
	{
		if(2==_stageNum)
		{
			join_29_obj.runInitScheduleWork();
			join_30_obj.runInitScheduleWork();
			Roundrobin_31_obj.runInitScheduleWork();
			Roundrobin_32_obj.runInitScheduleWork();
			CompareExchange_33_obj.runInitScheduleWork();
			CompareExchange_34_obj.runInitScheduleWork();
			join_35_obj.runInitScheduleWork();
			Roundrobin_36_obj.runInitScheduleWork();
			CompareExchange_37_obj.runInitScheduleWork();
			CompareExchange_38_obj.runInitScheduleWork();
			join_39_obj.runInitScheduleWork();
			join_40_obj.runInitScheduleWork();
			Roundrobin_41_obj.runInitScheduleWork();
			CompareExchange_42_obj.runInitScheduleWork();
			CompareExchange_43_obj.runInitScheduleWork();
			CompareExchange_44_obj.runInitScheduleWork();
			CompareExchange_45_obj.runInitScheduleWork();
			join_46_obj.runInitScheduleWork();
			Sink_47_obj.runInitScheduleWork();
		}
		if(1==_stageNum)
		{
			CompareExchange_25_obj.runInitScheduleWork();
			CompareExchange_27_obj.runInitScheduleWork();
			CompareExchange_28_obj.runInitScheduleWork();
		}
	
		workerSync(1);
	}
	for(int _stageNum=3;_stageNum<2*3+MAX_ITER-1;_stageNum++)
	{
		if(stage[2])
		{
			join_29_obj.runSteadyScheduleWork();
			join_30_obj.runSteadyScheduleWork();
			Roundrobin_31_obj.runSteadyScheduleWork();
			Roundrobin_32_obj.runSteadyScheduleWork();
			CompareExchange_33_obj.runSteadyScheduleWork();
			CompareExchange_34_obj.runSteadyScheduleWork();
			join_35_obj.runSteadyScheduleWork();
			Roundrobin_36_obj.runSteadyScheduleWork();
			CompareExchange_37_obj.runSteadyScheduleWork();
			CompareExchange_38_obj.runSteadyScheduleWork();
			join_39_obj.runSteadyScheduleWork();
			join_40_obj.runSteadyScheduleWork();
			Roundrobin_41_obj.runSteadyScheduleWork();
			CompareExchange_42_obj.runSteadyScheduleWork();
			CompareExchange_43_obj.runSteadyScheduleWork();
			CompareExchange_44_obj.runSteadyScheduleWork();
			CompareExchange_45_obj.runSteadyScheduleWork();
			join_46_obj.runSteadyScheduleWork();
			Sink_47_obj.runSteadyScheduleWork();
		}
		if(stage[1])
		{
			CompareExchange_25_obj.runSteadyScheduleWork();
			CompareExchange_27_obj.runSteadyScheduleWork();
			CompareExchange_28_obj.runSteadyScheduleWork();
		}
		for(int index=2; index>= 1; --index)
			stage[index] = stage[index-1];
		if(_stageNum == (MAX_ITER - 1 + 3))
		{
			stage[0]=0;
		}
	
		workerSync(1);
	}
}
