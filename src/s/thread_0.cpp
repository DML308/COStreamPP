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
	masterSync(1);
	Init_0 Init_0_obj(Init_0_A_1);
	A_1 A_1_obj(A_1_B_2,Init_0_A_1);
	B_2 B_2_obj(B_2_C_3,A_1_B_2);
	C_3 C_3_obj(C_3_D_4,B_2_C_3);
	D_4 D_4_obj(C_3_D_4);
	char stage[1]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<1;_stageNum++)
	{
		if(0==_stageNum)
		{
			Init_0_obj.runInitScheduleWork();
			A_1_obj.runInitScheduleWork();
			B_2_obj.runInitScheduleWork();
			C_3_obj.runInitScheduleWork();
			D_4_obj.runInitScheduleWork();
		}
	
		masterSync(1);
	}
	for(int _stageNum=1;_stageNum<2*1+MAX_ITER-1;_stageNum++)
	{
		if(stage[0])
		{
			Init_0_obj.runSteadyScheduleWork();
			A_1_obj.runSteadyScheduleWork();
			B_2_obj.runSteadyScheduleWork();
			C_3_obj.runSteadyScheduleWork();
			D_4_obj.runSteadyScheduleWork();
		}
		for(int index=0; index>= 1; --index)
			stage[index] = stage[index-1];
		if(_stageNum == (MAX_ITER - 1 + 1))
		{
			stage[0]=0;
		}
	
		masterSync(1);
	}
}
