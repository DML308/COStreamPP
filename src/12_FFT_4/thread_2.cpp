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
	CombineDFTX_5 CombineDFTX_5_obj(CombineDFTX_5_FloatSink_6,CombineDFTX_4_CombineDFTX_5);
	FloatSink_6 FloatSink_6_obj(CombineDFTX_5_FloatSink_6);
	char stage[5]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<5;_stageNum++)
	{
		if(4==_stageNum)
		{
			CombineDFTX_5_obj.runInitScheduleWork();
			FloatSink_6_obj.runInitScheduleWork();
		}
	
		workerSync(2);
	}
	for(int _stageNum=5;_stageNum<2*5+MAX_ITER-1;_stageNum++)
	{
		if(stage[4])
		{
			CombineDFTX_5_obj.runSteadyScheduleWork();
			FloatSink_6_obj.runSteadyScheduleWork();
		}
		for(int index=4; index>= 1; --index)
			stage[index] = stage[index-1];
		if(_stageNum == (MAX_ITER - 1 + 5))
		{
			stage[0]=0;
		}
	
		workerSync(2);
	}
}
