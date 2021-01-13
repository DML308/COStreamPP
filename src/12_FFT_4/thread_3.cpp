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
	FloatSource_0 FloatSource_0_obj(FloatSource_0_AssignmentX_1);
	AssignmentX_1 AssignmentX_1_obj(AssignmentX_1_FFTReorderSimpleX_2,FloatSource_0_AssignmentX_1);
	CombineDFTX_4 CombineDFTX_4_obj(CombineDFTX_4_CombineDFTX_5,CombineDFTX_3_CombineDFTX_4);
	char stage[5]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<5;_stageNum++)
	{
		if(3==_stageNum)
		{
			CombineDFTX_4_obj.runInitScheduleWork();
		}
		if(0==_stageNum)
		{
			FloatSource_0_obj.runInitScheduleWork();
			AssignmentX_1_obj.runInitScheduleWork();
		}
	
		workerSync(3);
	}
	for(int _stageNum=5;_stageNum<2*5+MAX_ITER-1;_stageNum++)
	{
		if(stage[3])
		{
			CombineDFTX_4_obj.runSteadyScheduleWork();
		}
		if(stage[0])
		{
			FloatSource_0_obj.runSteadyScheduleWork();
			AssignmentX_1_obj.runSteadyScheduleWork();
		}
		for(int index=4; index>= 1; --index)
			stage[index] = stage[index-1];
		if(_stageNum == (MAX_ITER - 1 + 5))
		{
			stage[0]=0;
		}
	
		workerSync(3);
	}
}
