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
	FloatSource_0 FloatSource_0_obj(FloatSource_0_Duplicate_1);
	Duplicate_1 Duplicate_1_obj(Duplicate_1_InputGenerate_2,Duplicate_1_InputGenerate_5,Duplicate_1_InputGenerate_8,Duplicate_1_InputGenerate_11,Duplicate_1_InputGenerate_14,Duplicate_1_InputGenerate_17,Duplicate_1_InputGenerate_20,Duplicate_1_InputGenerate_23,Duplicate_1_InputGenerate_26,Duplicate_1_InputGenerate_29,Duplicate_1_InputGenerate_32,Duplicate_1_InputGenerate_35,FloatSource_0_Duplicate_1);
	InputGenerate_2 InputGenerate_2_obj(InputGenerate_2_BeamFirFilter_3,Duplicate_1_InputGenerate_2);
	BeamFirFilter_3 BeamFirFilter_3_obj(BeamFirFilter_3_BeamFirFilter_4,InputGenerate_2_BeamFirFilter_3);
	InputGenerate_5 InputGenerate_5_obj(InputGenerate_5_BeamFirFilter_6,Duplicate_1_InputGenerate_5);
	BeamFirFilter_6 BeamFirFilter_6_obj(BeamFirFilter_6_BeamFirFilter_7,InputGenerate_5_BeamFirFilter_6);
	BeamFirFilter_7 BeamFirFilter_7_obj(BeamFirFilter_7_join_38,BeamFirFilter_6_BeamFirFilter_7);
	InputGenerate_8 InputGenerate_8_obj(InputGenerate_8_BeamFirFilter_9,Duplicate_1_InputGenerate_8);
	BeamFirFilter_9 BeamFirFilter_9_obj(BeamFirFilter_9_BeamFirFilter_10,InputGenerate_8_BeamFirFilter_9);
	BeamFirFilter_10 BeamFirFilter_10_obj(BeamFirFilter_10_join_38,BeamFirFilter_9_BeamFirFilter_10);
	InputGenerate_11 InputGenerate_11_obj(InputGenerate_11_BeamFirFilter_12,Duplicate_1_InputGenerate_11);
	BeamFirFilter_12 BeamFirFilter_12_obj(BeamFirFilter_12_BeamFirFilter_13,InputGenerate_11_BeamFirFilter_12);
	InputGenerate_14 InputGenerate_14_obj(InputGenerate_14_BeamFirFilter_15,Duplicate_1_InputGenerate_14);
	InputGenerate_17 InputGenerate_17_obj(InputGenerate_17_BeamFirFilter_18,Duplicate_1_InputGenerate_17);
	char stage[4]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<4;_stageNum++)
	{
		if(0==_stageNum)
		{
			FloatSource_0_obj.runInitScheduleWork();
			Duplicate_1_obj.runInitScheduleWork();
			InputGenerate_2_obj.runInitScheduleWork();
			BeamFirFilter_3_obj.runInitScheduleWork();
			InputGenerate_5_obj.runInitScheduleWork();
			BeamFirFilter_6_obj.runInitScheduleWork();
			BeamFirFilter_7_obj.runInitScheduleWork();
			InputGenerate_8_obj.runInitScheduleWork();
			BeamFirFilter_9_obj.runInitScheduleWork();
			BeamFirFilter_10_obj.runInitScheduleWork();
			InputGenerate_11_obj.runInitScheduleWork();
			BeamFirFilter_12_obj.runInitScheduleWork();
			InputGenerate_14_obj.runInitScheduleWork();
			InputGenerate_17_obj.runInitScheduleWork();
		}
	
		workerSync(2);
	}
	for(int _stageNum=4;_stageNum<2*4+MAX_ITER-1;_stageNum++)
	{
		if(stage[0])
		{
			FloatSource_0_obj.runSteadyScheduleWork();
			Duplicate_1_obj.runSteadyScheduleWork();
			InputGenerate_2_obj.runSteadyScheduleWork();
			BeamFirFilter_3_obj.runSteadyScheduleWork();
			InputGenerate_5_obj.runSteadyScheduleWork();
			BeamFirFilter_6_obj.runSteadyScheduleWork();
			BeamFirFilter_7_obj.runSteadyScheduleWork();
			InputGenerate_8_obj.runSteadyScheduleWork();
			BeamFirFilter_9_obj.runSteadyScheduleWork();
			BeamFirFilter_10_obj.runSteadyScheduleWork();
			InputGenerate_11_obj.runSteadyScheduleWork();
			BeamFirFilter_12_obj.runSteadyScheduleWork();
			InputGenerate_14_obj.runSteadyScheduleWork();
			InputGenerate_17_obj.runSteadyScheduleWork();
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
