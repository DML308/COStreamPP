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
	DCT_1D_7 DCT_1D_7_obj(DCT_1D_7_join_10,Roundrobin_1_DCT_1D_7);
	DCT_1D_8 DCT_1D_8_obj(DCT_1D_8_join_10,Roundrobin_1_DCT_1D_8);
	DCT_1D_9 DCT_1D_9_obj(DCT_1D_9_join_10,Roundrobin_1_DCT_1D_9);
	join_10 join_10_obj(join_10_Roundrobin_11,DCT_1D_2_join_10,DCT_1D_3_join_10,DCT_1D_4_join_10,DCT_1D_5_join_10,DCT_1D_6_join_10,DCT_1D_7_join_10,DCT_1D_8_join_10,DCT_1D_9_join_10);
	Roundrobin_11 Roundrobin_11_obj(Roundrobin_11_DCT_1D_12,Roundrobin_11_DCT_1D_13,Roundrobin_11_DCT_1D_14,Roundrobin_11_DCT_1D_15,Roundrobin_11_DCT_1D_16,Roundrobin_11_DCT_1D_17,Roundrobin_11_DCT_1D_18,Roundrobin_11_DCT_1D_19,join_10_Roundrobin_11);
	DCT_1D_12 DCT_1D_12_obj(DCT_1D_12_join_20,Roundrobin_11_DCT_1D_12);
	DCT_1D_13 DCT_1D_13_obj(DCT_1D_13_join_20,Roundrobin_11_DCT_1D_13);
	char stage[7]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<7;_stageNum++)
	{
		if(2==_stageNum)
		{
			join_10_obj.runInitScheduleWork();
			Roundrobin_11_obj.runInitScheduleWork();
			DCT_1D_12_obj.runInitScheduleWork();
			DCT_1D_13_obj.runInitScheduleWork();
		}
		if(1==_stageNum)
		{
			DCT_1D_7_obj.runInitScheduleWork();
			DCT_1D_8_obj.runInitScheduleWork();
			DCT_1D_9_obj.runInitScheduleWork();
		}
	
		workerSync(2);
	}
	for(int _stageNum=7;_stageNum<2*7+MAX_ITER-1;_stageNum++)
	{
		if(stage[2])
		{
			join_10_obj.runSteadyScheduleWork();
			Roundrobin_11_obj.runSteadyScheduleWork();
			DCT_1D_12_obj.runSteadyScheduleWork();
			DCT_1D_13_obj.runSteadyScheduleWork();
		}
		if(stage[1])
		{
			DCT_1D_7_obj.runSteadyScheduleWork();
			DCT_1D_8_obj.runSteadyScheduleWork();
			DCT_1D_9_obj.runSteadyScheduleWork();
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
