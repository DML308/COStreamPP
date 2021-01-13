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
	DCT_1D_12 DCT_1D_12_obj(DCT_1D_12_join_20,Roundrobin_11_DCT_1D_12);
	DCT_1D_13 DCT_1D_13_obj(DCT_1D_13_join_20,Roundrobin_11_DCT_1D_13);
	DCT_1D_14 DCT_1D_14_obj(DCT_1D_14_join_20,Roundrobin_11_DCT_1D_14);
	DCT_1D_15 DCT_1D_15_obj(DCT_1D_15_join_20,Roundrobin_11_DCT_1D_15);
	DCT_1D_16 DCT_1D_16_obj(DCT_1D_16_join_20,Roundrobin_11_DCT_1D_16);
	DCT_1D_17 DCT_1D_17_obj(DCT_1D_17_join_20,Roundrobin_11_DCT_1D_17);
	DCT_1D_19 DCT_1D_19_obj(DCT_1D_19_join_20,Roundrobin_11_DCT_1D_19);
	join_20 join_20_obj(join_20_DCT_Result_21,DCT_1D_12_join_20,DCT_1D_13_join_20,DCT_1D_14_join_20,DCT_1D_15_join_20,DCT_1D_16_join_20,DCT_1D_17_join_20,DCT_1D_18_join_20,DCT_1D_19_join_20);
	DCT_Result_21 DCT_Result_21_obj(DCT_Result_21_Sink_22,join_20_DCT_Result_21);
	Sink_22 Sink_22_obj(DCT_Result_21_Sink_22);
	char stage[3]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<3;_stageNum++)
	{
		if(2==_stageNum)
		{
			join_20_obj.runInitScheduleWork();
			DCT_Result_21_obj.runInitScheduleWork();
			Sink_22_obj.runInitScheduleWork();
		}
		if(1==_stageNum)
		{
			DCT_1D_12_obj.runInitScheduleWork();
			DCT_1D_13_obj.runInitScheduleWork();
			DCT_1D_14_obj.runInitScheduleWork();
			DCT_1D_15_obj.runInitScheduleWork();
			DCT_1D_16_obj.runInitScheduleWork();
			DCT_1D_17_obj.runInitScheduleWork();
			DCT_1D_19_obj.runInitScheduleWork();
		}
	
		workerSync(1);
	}
	for(int _stageNum=3;_stageNum<2*3+MAX_ITER-1;_stageNum++)
	{
		if(stage[2])
		{
			join_20_obj.runSteadyScheduleWork();
			DCT_Result_21_obj.runSteadyScheduleWork();
			Sink_22_obj.runSteadyScheduleWork();
		}
		if(stage[1])
		{
			DCT_1D_12_obj.runSteadyScheduleWork();
			DCT_1D_13_obj.runSteadyScheduleWork();
			DCT_1D_14_obj.runSteadyScheduleWork();
			DCT_1D_15_obj.runSteadyScheduleWork();
			DCT_1D_16_obj.runSteadyScheduleWork();
			DCT_1D_17_obj.runSteadyScheduleWork();
			DCT_1D_19_obj.runSteadyScheduleWork();
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
