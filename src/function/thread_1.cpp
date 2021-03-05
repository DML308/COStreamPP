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
	Init_0 Init_0_obj(Init_0_A_1);
	A_1 A_1_obj(A_1_B_2,Init_0_A_1);
	B_2 B_2_obj(B_2_C_3,A_1_B_2);
	D_4 D_4_obj(C_3_D_4);
	char stage[3]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<3;_stageNum++)
	{
		if(2==_stageNum)
		{
			D_4_obj.runInitScheduleWork();
		}
		if(0==_stageNum)
		{
			Init_0_obj.runInitScheduleWork();
			A_1_obj.runInitScheduleWork();
			B_2_obj.runInitScheduleWork();
		}
	
		workerSync(1);
	}
	for(int _stageNum=3;_stageNum<2*3+MAX_ITER-1;_stageNum++)
	{
		if(stage[2])
		{
			D_4_obj.runSteadyScheduleWork();
		}
		if(stage[0])
		{
			Init_0_obj.runSteadyScheduleWork();
			A_1_obj.runSteadyScheduleWork();
			B_2_obj.runSteadyScheduleWork();
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
