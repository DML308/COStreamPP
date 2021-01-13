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
	DCT_1D_14 DCT_1D_14_obj(DCT_1D_14_join_20,Roundrobin_11_DCT_1D_14);
	DCT_1D_15 DCT_1D_15_obj(DCT_1D_15_join_20,Roundrobin_11_DCT_1D_15);
	DCT_1D_16 DCT_1D_16_obj(DCT_1D_16_join_20,Roundrobin_11_DCT_1D_16);
	DCT_1D_17 DCT_1D_17_obj(DCT_1D_17_join_20,Roundrobin_11_DCT_1D_17);
	DCT_1D_18 DCT_1D_18_obj(DCT_1D_18_join_20,Roundrobin_11_DCT_1D_18);
	DCT_1D_19 DCT_1D_19_obj(DCT_1D_19_join_20,Roundrobin_11_DCT_1D_19);
	join_20 join_20_obj(join_20_Quantification_21,DCT_1D_12_join_20,DCT_1D_13_join_20,DCT_1D_14_join_20,DCT_1D_15_join_20,DCT_1D_16_join_20,DCT_1D_17_join_20,DCT_1D_18_join_20,DCT_1D_19_join_20);
	Sink_22 Sink_22_obj(Quantification_21_Sink_22);
	char stage[7]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<7;_stageNum++)
	{
		if(6==_stageNum)
		{
			Sink_22_obj.runInitScheduleWork();
		}
		if(4==_stageNum)
		{
			join_20_obj.runInitScheduleWork();
		}
		if(3==_stageNum)
		{
			DCT_1D_14_obj.runInitScheduleWork();
			DCT_1D_15_obj.runInitScheduleWork();
			DCT_1D_16_obj.runInitScheduleWork();
			DCT_1D_17_obj.runInitScheduleWork();
			DCT_1D_18_obj.runInitScheduleWork();
			DCT_1D_19_obj.runInitScheduleWork();
		}
	
		workerSync(3);
	}
	for(int _stageNum=7;_stageNum<2*7+MAX_ITER-1;_stageNum++)
	{
		if(stage[6])
		{
			Sink_22_obj.runSteadyScheduleWork();
		}
		if(stage[4])
		{
			join_20_obj.runSteadyScheduleWork();
		}
		if(stage[3])
		{
			DCT_1D_14_obj.runSteadyScheduleWork();
			DCT_1D_15_obj.runSteadyScheduleWork();
			DCT_1D_16_obj.runSteadyScheduleWork();
			DCT_1D_17_obj.runSteadyScheduleWork();
			DCT_1D_18_obj.runSteadyScheduleWork();
			DCT_1D_19_obj.runSteadyScheduleWork();
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
