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
	BeamFirFilter_4 BeamFirFilter_4_obj(BeamFirFilter_4_join_38,BeamFirFilter_3_BeamFirFilter_4);
	BeamFirFilter_13 BeamFirFilter_13_obj(BeamFirFilter_13_join_38,BeamFirFilter_12_BeamFirFilter_13);
	BeamFirFilter_15 BeamFirFilter_15_obj(BeamFirFilter_15_BeamFirFilter_16,InputGenerate_14_BeamFirFilter_15);
	BeamFirFilter_16 BeamFirFilter_16_obj(BeamFirFilter_16_join_38,BeamFirFilter_15_BeamFirFilter_16);
	BeamFirFilter_18 BeamFirFilter_18_obj(BeamFirFilter_18_BeamFirFilter_19,InputGenerate_17_BeamFirFilter_18);
	BeamFirFilter_19 BeamFirFilter_19_obj(BeamFirFilter_19_join_38,BeamFirFilter_18_BeamFirFilter_19);
	InputGenerate_20 InputGenerate_20_obj(InputGenerate_20_BeamFirFilter_21,Duplicate_1_InputGenerate_20);
	BeamFirFilter_21 BeamFirFilter_21_obj(BeamFirFilter_21_BeamFirFilter_22,InputGenerate_20_BeamFirFilter_21);
	BeamFirFilter_22 BeamFirFilter_22_obj(BeamFirFilter_22_join_38,BeamFirFilter_21_BeamFirFilter_22);
	char stage[4]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<4;_stageNum++)
	{
		if(1==_stageNum)
		{
			BeamFirFilter_4_obj.runInitScheduleWork();
			BeamFirFilter_13_obj.runInitScheduleWork();
			BeamFirFilter_15_obj.runInitScheduleWork();
			BeamFirFilter_16_obj.runInitScheduleWork();
			BeamFirFilter_18_obj.runInitScheduleWork();
			BeamFirFilter_19_obj.runInitScheduleWork();
			InputGenerate_20_obj.runInitScheduleWork();
			BeamFirFilter_21_obj.runInitScheduleWork();
			BeamFirFilter_22_obj.runInitScheduleWork();
		}
	
		masterSync(4);
	}
	for(int _stageNum=4;_stageNum<2*4+MAX_ITER-1;_stageNum++)
	{
		if(stage[1])
		{
			BeamFirFilter_4_obj.runSteadyScheduleWork();
			BeamFirFilter_13_obj.runSteadyScheduleWork();
			BeamFirFilter_15_obj.runSteadyScheduleWork();
			BeamFirFilter_16_obj.runSteadyScheduleWork();
			BeamFirFilter_18_obj.runSteadyScheduleWork();
			BeamFirFilter_19_obj.runSteadyScheduleWork();
			InputGenerate_20_obj.runSteadyScheduleWork();
			BeamFirFilter_21_obj.runSteadyScheduleWork();
			BeamFirFilter_22_obj.runSteadyScheduleWork();
		}
		for(int index=3; index>= 1; --index)
			stage[index] = stage[index-1];
		if(_stageNum == (MAX_ITER - 1 + 4))
		{
			stage[0]=0;
		}
	
		masterSync(4);
	}
}
