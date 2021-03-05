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
	masterSync(4);
	FFTReorderSimpleX_2 FFTReorderSimpleX_2_obj(FFTReorderSimpleX_2_CombineDFTX_3,AssignmentX_1_FFTReorderSimpleX_2);
	char stage[5]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<5;_stageNum++)
	{
		if(1==_stageNum)
		{
			FFTReorderSimpleX_2_obj.runInitScheduleWork();
		}
	
		masterSync(4);
	}
	for(int _stageNum=5;_stageNum<2*5+MAX_ITER-1;_stageNum++)
	{
		if(stage[1])
		{
			FFTReorderSimpleX_2_obj.runSteadyScheduleWork();
		}
		for(int index=4; index>= 1; --index)
			stage[index] = stage[index-1];
		if(_stageNum == (MAX_ITER - 1 + 5))
		{
			stage[0]=0;
		}
	
		masterSync(4);
	}
}