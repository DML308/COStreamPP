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
	FloatSource_0 FloatSource_0_obj(FloatSource_0_AssignmentX_1);
	AssignmentX_1 AssignmentX_1_obj(AssignmentX_1_FFTReorderSimpleX_2,FloatSource_0_AssignmentX_1);
	CombineDFTX_5 CombineDFTX_5_obj(CombineDFTX_5_FloatSink_6,CombineDFTX_4_CombineDFTX_5);
	FloatSink_6 FloatSink_6_obj(CombineDFTX_5_FloatSink_6);
	char stage[4]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<4;_stageNum++)
	{
		if(3==_stageNum)
		{
			CombineDFTX_5_obj.runInitScheduleWork();
			FloatSink_6_obj.runInitScheduleWork();
		}
		if(0==_stageNum)
		{
			FloatSource_0_obj.runInitScheduleWork();
			AssignmentX_1_obj.runInitScheduleWork();
		}
	
		workerSync(2);
	}
	for(int _stageNum=4;_stageNum<2*4+MAX_ITER-1;_stageNum++)
	{
		if(stage[3])
		{
			CombineDFTX_5_obj.runSteadyScheduleWork();
			FloatSink_6_obj.runSteadyScheduleWork();
		}
		if(stage[0])
		{
			FloatSource_0_obj.runSteadyScheduleWork();
			AssignmentX_1_obj.runSteadyScheduleWork();
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
