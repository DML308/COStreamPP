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
	Source_0 Source_0_obj(Source_0_Roundrobin_1);
	ZigZagUnordering_2 ZigZagUnordering_2_obj(ZigZagUnordering_2_Duplicate_3,Roundrobin_1_ZigZagUnordering_2);
	Duplicate_3 Duplicate_3_obj(Duplicate_3_Roundrobin_4,Duplicate_3_InverseQuantization_AC_Coeff_8,ZigZagUnordering_2_Duplicate_3);
	Roundrobin_4 Roundrobin_4_obj(Roundrobin_4_InverseQuantization_DC_Intra_Coeff_5,Roundrobin_4_InverseQuantization_AC_Coeff_6,Duplicate_3_Roundrobin_4);
	InverseQuantization_DC_Intra_Coeff_5 InverseQuantization_DC_Intra_Coeff_5_obj(InverseQuantization_DC_Intra_Coeff_5_join_7,Roundrobin_4_InverseQuantization_DC_Intra_Coeff_5);
	InverseQuantization_AC_Coeff_6 InverseQuantization_AC_Coeff_6_obj(InverseQuantization_AC_Coeff_6_join_7,Roundrobin_4_InverseQuantization_AC_Coeff_6);
	join_7 join_7_obj(join_7_join_9,InverseQuantization_DC_Intra_Coeff_5_join_7,InverseQuantization_AC_Coeff_6_join_7);
	InverseQuantization_AC_Coeff_8 InverseQuantization_AC_Coeff_8_obj(InverseQuantization_AC_Coeff_8_join_9,Duplicate_3_InverseQuantization_AC_Coeff_8);
	join_9 join_9_obj(join_9_InverseQuantizationJoinerSubstitute_10,join_7_join_9,InverseQuantization_AC_Coeff_8_join_9);
	InverseQuantizationJoinerSubstitute_10 InverseQuantizationJoinerSubstitute_10_obj(InverseQuantizationJoinerSubstitute_10_Saturation_11,join_9_InverseQuantizationJoinerSubstitute_10);
	Saturation_11 Saturation_11_obj(Saturation_11_MismatchControl_12,InverseQuantizationJoinerSubstitute_10_Saturation_11);
	MotionVectorDecode_34 MotionVectorDecode_34_obj(MotionVectorDecode_34_Repeat_35,Roundrobin_1_MotionVectorDecode_34);
	char stage[4]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<4;_stageNum++)
	{
		if(2==_stageNum)
		{
			ZigZagUnordering_2_obj.runInitScheduleWork();
			Duplicate_3_obj.runInitScheduleWork();
			Roundrobin_4_obj.runInitScheduleWork();
			InverseQuantization_DC_Intra_Coeff_5_obj.runInitScheduleWork();
			InverseQuantization_AC_Coeff_6_obj.runInitScheduleWork();
			join_7_obj.runInitScheduleWork();
			InverseQuantization_AC_Coeff_8_obj.runInitScheduleWork();
			join_9_obj.runInitScheduleWork();
			InverseQuantizationJoinerSubstitute_10_obj.runInitScheduleWork();
			Saturation_11_obj.runInitScheduleWork();
			MotionVectorDecode_34_obj.runInitScheduleWork();
		}
		if(0==_stageNum)
		{
			Source_0_obj.runInitScheduleWork();
		}
	
		workerSync(1);
	}
	for(int _stageNum=4;_stageNum<2*4+MAX_ITER-1;_stageNum++)
	{
		if(stage[2])
		{
			ZigZagUnordering_2_obj.runSteadyScheduleWork();
			Duplicate_3_obj.runSteadyScheduleWork();
			Roundrobin_4_obj.runSteadyScheduleWork();
			InverseQuantization_DC_Intra_Coeff_5_obj.runSteadyScheduleWork();
			InverseQuantization_AC_Coeff_6_obj.runSteadyScheduleWork();
			join_7_obj.runSteadyScheduleWork();
			InverseQuantization_AC_Coeff_8_obj.runSteadyScheduleWork();
			join_9_obj.runSteadyScheduleWork();
			InverseQuantizationJoinerSubstitute_10_obj.runSteadyScheduleWork();
			Saturation_11_obj.runSteadyScheduleWork();
			MotionVectorDecode_34_obj.runSteadyScheduleWork();
		}
		if(stage[0])
		{
			Source_0_obj.runSteadyScheduleWork();
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
