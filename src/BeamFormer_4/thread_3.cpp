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
	InputGenerate_23 InputGenerate_23_obj(InputGenerate_23_BeamFirFilter_24,Duplicate_1_InputGenerate_23);
	BeamFirFilter_24 BeamFirFilter_24_obj(BeamFirFilter_24_BeamFirFilter_25,InputGenerate_23_BeamFirFilter_24);
	InputGenerate_26 InputGenerate_26_obj(InputGenerate_26_BeamFirFilter_27,Duplicate_1_InputGenerate_26);
	BeamFirFilter_27 BeamFirFilter_27_obj(BeamFirFilter_27_BeamFirFilter_28,InputGenerate_26_BeamFirFilter_27);
	BeamFirFilter_28 BeamFirFilter_28_obj(BeamFirFilter_28_join_38,BeamFirFilter_27_BeamFirFilter_28);
	InputGenerate_29 InputGenerate_29_obj(InputGenerate_29_BeamFirFilter_30,Duplicate_1_InputGenerate_29);
	BeamFirFilter_30 BeamFirFilter_30_obj(BeamFirFilter_30_BeamFirFilter_31,InputGenerate_29_BeamFirFilter_30);
	BeamFirFilter_31 BeamFirFilter_31_obj(BeamFirFilter_31_join_38,BeamFirFilter_30_BeamFirFilter_31);
	InputGenerate_32 InputGenerate_32_obj(InputGenerate_32_BeamFirFilter_33,Duplicate_1_InputGenerate_32);
	BeamFirFilter_33 BeamFirFilter_33_obj(BeamFirFilter_33_BeamFirFilter_34,InputGenerate_32_BeamFirFilter_33);
	char stage[4]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<4;_stageNum++)
	{
		if(1==_stageNum)
		{
			InputGenerate_23_obj.runInitScheduleWork();
			BeamFirFilter_24_obj.runInitScheduleWork();
			InputGenerate_26_obj.runInitScheduleWork();
			BeamFirFilter_27_obj.runInitScheduleWork();
			BeamFirFilter_28_obj.runInitScheduleWork();
			InputGenerate_29_obj.runInitScheduleWork();
			BeamFirFilter_30_obj.runInitScheduleWork();
			BeamFirFilter_31_obj.runInitScheduleWork();
			InputGenerate_32_obj.runInitScheduleWork();
			BeamFirFilter_33_obj.runInitScheduleWork();
		}
	
		workerSync(3);
	}
	for(int _stageNum=4;_stageNum<2*4+MAX_ITER-1;_stageNum++)
	{
		if(stage[1])
		{
			InputGenerate_23_obj.runSteadyScheduleWork();
			BeamFirFilter_24_obj.runSteadyScheduleWork();
			InputGenerate_26_obj.runSteadyScheduleWork();
			BeamFirFilter_27_obj.runSteadyScheduleWork();
			BeamFirFilter_28_obj.runSteadyScheduleWork();
			InputGenerate_29_obj.runSteadyScheduleWork();
			BeamFirFilter_30_obj.runSteadyScheduleWork();
			BeamFirFilter_31_obj.runSteadyScheduleWork();
			InputGenerate_32_obj.runSteadyScheduleWork();
			BeamFirFilter_33_obj.runSteadyScheduleWork();
		}
		for(int index=3; index>= 1; --index)
			stage[index] = stage[index-1];
		if(_stageNum == (MAX_ITER - 1 + 4))
		{
			stage[0]=0;
		}
	
		workerSync(3);
	}
}
