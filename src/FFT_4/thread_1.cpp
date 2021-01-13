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
	CombineDFTX_3 CombineDFTX_3_obj(CombineDFTX_3_CombineDFTX_4,FFTReorderSimpleX_2_CombineDFTX_3);
	CombineDFTX_4 CombineDFTX_4_obj(CombineDFTX_4_CombineDFTX_5,CombineDFTX_3_CombineDFTX_4);
	char stage[4]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<4;_stageNum++)
	{
		if(2==_stageNum)
		{
			CombineDFTX_3_obj.runInitScheduleWork();
			CombineDFTX_4_obj.runInitScheduleWork();
		}
	
		workerSync(1);
	}
	for(int _stageNum=4;_stageNum<2*4+MAX_ITER-1;_stageNum++)
	{
		if(stage[2])
		{
			CombineDFTX_3_obj.runSteadyScheduleWork();
			CombineDFTX_4_obj.runSteadyScheduleWork();
		}
		for(int index=3; index>= 1; --index)
			stage[index] = stage[index-1];
		if(_stageNum == (MAX_ITER - 1 + 4))
		{
			stage[0]=0;
		}
	
		workerSync(1);
	}
}
