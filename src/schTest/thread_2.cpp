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
	Init_0 Init_0_obj(Init_0_A_1);
	C_3 C_3_obj(C_3_D_4,B_2_C_3);
	D_4 D_4_obj(C_3_D_4);
	char stage[4]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<4;_stageNum++)
	{
		if(3==_stageNum)
		{
			C_3_obj.runInitScheduleWork();
			D_4_obj.runInitScheduleWork();
		}
		if(0==_stageNum)
		{
			Init_0_obj.runInitScheduleWork();
		}
	
		workerSync(2);
	}
	for(int _stageNum=4;_stageNum<2*4+MAX_ITER-1;_stageNum++)
	{
		if(stage[3])
		{
			C_3_obj.runSteadyScheduleWork();
			D_4_obj.runSteadyScheduleWork();
		}
		if(stage[0])
		{
			Init_0_obj.runSteadyScheduleWork();
		}
		for(int index=3; index>= 1; --index)
			stage[index] = stage[index-1];
		if(_stageNum == (MAX_ITER - 1 + 4))
		{
			stage[0]=0;
		}
	
		workerSync(2);
	}
}
