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
	IntSource_0 IntSource_0_obj(IntSource_0_Roundrobin_1);
	Roundrobin_1 Roundrobin_1_obj(Roundrobin_1_DCT_1D_2,Roundrobin_1_DCT_1D_3,Roundrobin_1_DCT_1D_4,Roundrobin_1_DCT_1D_5,Roundrobin_1_DCT_1D_6,Roundrobin_1_DCT_1D_7,Roundrobin_1_DCT_1D_8,Roundrobin_1_DCT_1D_9,IntSource_0_Roundrobin_1);
	DCT_1D_2 DCT_1D_2_obj(DCT_1D_2_join_10,Roundrobin_1_DCT_1D_2);
	DCT_1D_3 DCT_1D_3_obj(DCT_1D_3_join_10,Roundrobin_1_DCT_1D_3);
	DCT_1D_4 DCT_1D_4_obj(DCT_1D_4_join_10,Roundrobin_1_DCT_1D_4);
	DCT_1D_5 DCT_1D_5_obj(DCT_1D_5_join_10,Roundrobin_1_DCT_1D_5);
	DCT_1D_6 DCT_1D_6_obj(DCT_1D_6_join_10,Roundrobin_1_DCT_1D_6);
	char stage[7]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<7;_stageNum++)
	{
		if(0==_stageNum)
		{
			IntSource_0_obj.runInitScheduleWork();
			Roundrobin_1_obj.runInitScheduleWork();
			DCT_1D_2_obj.runInitScheduleWork();
			DCT_1D_3_obj.runInitScheduleWork();
			DCT_1D_4_obj.runInitScheduleWork();
			DCT_1D_5_obj.runInitScheduleWork();
			DCT_1D_6_obj.runInitScheduleWork();
		}
	
		workerSync(1);
	}
	for(int _stageNum=7;_stageNum<2*7+MAX_ITER-1;_stageNum++)
	{
		if(stage[0])
		{
			IntSource_0_obj.runSteadyScheduleWork();
			Roundrobin_1_obj.runSteadyScheduleWork();
			DCT_1D_2_obj.runSteadyScheduleWork();
			DCT_1D_3_obj.runSteadyScheduleWork();
			DCT_1D_4_obj.runSteadyScheduleWork();
			DCT_1D_5_obj.runSteadyScheduleWork();
			DCT_1D_6_obj.runSteadyScheduleWork();
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
