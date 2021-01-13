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
	FloatSource_0 FloatSource_0_obj(FloatSource_0_AssignmentX_1);
	AssignmentX_1 AssignmentX_1_obj(AssignmentX_1_FFTReorderSimpleX_2,FloatSource_0_AssignmentX_1);
	FFTReorderSimpleX_2 FFTReorderSimpleX_2_obj(FFTReorderSimpleX_2_CombineDFTX_3,AssignmentX_1_FFTReorderSimpleX_2);
	CombineDFTX_3 CombineDFTX_3_obj(CombineDFTX_3_CombineDFTX_4,FFTReorderSimpleX_2_CombineDFTX_3);
	CombineDFTX_4 CombineDFTX_4_obj(CombineDFTX_4_CombineDFTX_5,CombineDFTX_3_CombineDFTX_4);
	CombineDFTX_5 CombineDFTX_5_obj(CombineDFTX_5_FloatSink_6,CombineDFTX_4_CombineDFTX_5);
	FloatSink_6 FloatSink_6_obj(CombineDFTX_5_FloatSink_6);
	char stage[1]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<1;_stageNum++)
	{
		if(0==_stageNum)
		{
			FloatSource_0_obj.runInitScheduleWork();
			AssignmentX_1_obj.runInitScheduleWork();
			FFTReorderSimpleX_2_obj.runInitScheduleWork();
			CombineDFTX_3_obj.runInitScheduleWork();
			CombineDFTX_4_obj.runInitScheduleWork();
			CombineDFTX_5_obj.runInitScheduleWork();
			FloatSink_6_obj.runInitScheduleWork();
		}
	
		masterSync(1);
	}
	for(int _stageNum=1;_stageNum<2*1+MAX_ITER-1;_stageNum++)
	{
		if(stage[0])
		{
			FloatSource_0_obj.runSteadyScheduleWork();
			AssignmentX_1_obj.runSteadyScheduleWork();
			FFTReorderSimpleX_2_obj.runSteadyScheduleWork();
			CombineDFTX_3_obj.runSteadyScheduleWork();
			CombineDFTX_4_obj.runSteadyScheduleWork();
			CombineDFTX_5_obj.runSteadyScheduleWork();
			FloatSink_6_obj.runSteadyScheduleWork();
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
