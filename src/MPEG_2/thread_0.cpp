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
	masterSync(2);
	Roundrobin_1 Roundrobin_1_obj(Roundrobin_1_ZigZagUnordering_2,Roundrobin_1_MotionVectorDecode_34,Roundrobin_1_Repeat_36,Source_0_Roundrobin_1);
	MismatchControl_12 MismatchControl_12_obj(MismatchControl_12_Roundrobin_13,Saturation_11_MismatchControl_12);
	Roundrobin_13 Roundrobin_13_obj(Roundrobin_13_iDCT8x8_1D_row_fast_14,Roundrobin_13_iDCT8x8_1D_row_fast_15,Roundrobin_13_iDCT8x8_1D_row_fast_16,Roundrobin_13_iDCT8x8_1D_row_fast_17,Roundrobin_13_iDCT8x8_1D_row_fast_18,Roundrobin_13_iDCT8x8_1D_row_fast_19,Roundrobin_13_iDCT8x8_1D_row_fast_20,Roundrobin_13_iDCT8x8_1D_row_fast_21,MismatchControl_12_Roundrobin_13);
	iDCT8x8_1D_row_fast_14 iDCT8x8_1D_row_fast_14_obj(iDCT8x8_1D_row_fast_14_join_22,Roundrobin_13_iDCT8x8_1D_row_fast_14);
	iDCT8x8_1D_row_fast_15 iDCT8x8_1D_row_fast_15_obj(iDCT8x8_1D_row_fast_15_join_22,Roundrobin_13_iDCT8x8_1D_row_fast_15);
	iDCT8x8_1D_row_fast_16 iDCT8x8_1D_row_fast_16_obj(iDCT8x8_1D_row_fast_16_join_22,Roundrobin_13_iDCT8x8_1D_row_fast_16);
	iDCT8x8_1D_row_fast_17 iDCT8x8_1D_row_fast_17_obj(iDCT8x8_1D_row_fast_17_join_22,Roundrobin_13_iDCT8x8_1D_row_fast_17);
	iDCT8x8_1D_row_fast_18 iDCT8x8_1D_row_fast_18_obj(iDCT8x8_1D_row_fast_18_join_22,Roundrobin_13_iDCT8x8_1D_row_fast_18);
	iDCT8x8_1D_row_fast_19 iDCT8x8_1D_row_fast_19_obj(iDCT8x8_1D_row_fast_19_join_22,Roundrobin_13_iDCT8x8_1D_row_fast_19);
	iDCT8x8_1D_row_fast_20 iDCT8x8_1D_row_fast_20_obj(iDCT8x8_1D_row_fast_20_join_22,Roundrobin_13_iDCT8x8_1D_row_fast_20);
	iDCT8x8_1D_row_fast_21 iDCT8x8_1D_row_fast_21_obj(iDCT8x8_1D_row_fast_21_join_22,Roundrobin_13_iDCT8x8_1D_row_fast_21);
	join_22 join_22_obj(join_22_Roundrobin_23,iDCT8x8_1D_row_fast_14_join_22,iDCT8x8_1D_row_fast_15_join_22,iDCT8x8_1D_row_fast_16_join_22,iDCT8x8_1D_row_fast_17_join_22,iDCT8x8_1D_row_fast_18_join_22,iDCT8x8_1D_row_fast_19_join_22,iDCT8x8_1D_row_fast_20_join_22,iDCT8x8_1D_row_fast_21_join_22);
	Roundrobin_23 Roundrobin_23_obj(Roundrobin_23_iDCT8x8_1D_col_fast_fine_24,Roundrobin_23_iDCT8x8_1D_col_fast_fine_25,Roundrobin_23_iDCT8x8_1D_col_fast_fine_26,Roundrobin_23_iDCT8x8_1D_col_fast_fine_27,Roundrobin_23_iDCT8x8_1D_col_fast_fine_28,Roundrobin_23_iDCT8x8_1D_col_fast_fine_29,Roundrobin_23_iDCT8x8_1D_col_fast_fine_30,Roundrobin_23_iDCT8x8_1D_col_fast_fine_31,join_22_Roundrobin_23);
	iDCT8x8_1D_col_fast_fine_24 iDCT8x8_1D_col_fast_fine_24_obj(iDCT8x8_1D_col_fast_fine_24_join_32,Roundrobin_23_iDCT8x8_1D_col_fast_fine_24);
	iDCT8x8_1D_col_fast_fine_25 iDCT8x8_1D_col_fast_fine_25_obj(iDCT8x8_1D_col_fast_fine_25_join_32,Roundrobin_23_iDCT8x8_1D_col_fast_fine_25);
	iDCT8x8_1D_col_fast_fine_26 iDCT8x8_1D_col_fast_fine_26_obj(iDCT8x8_1D_col_fast_fine_26_join_32,Roundrobin_23_iDCT8x8_1D_col_fast_fine_26);
	iDCT8x8_1D_col_fast_fine_27 iDCT8x8_1D_col_fast_fine_27_obj(iDCT8x8_1D_col_fast_fine_27_join_32,Roundrobin_23_iDCT8x8_1D_col_fast_fine_27);
	iDCT8x8_1D_col_fast_fine_28 iDCT8x8_1D_col_fast_fine_28_obj(iDCT8x8_1D_col_fast_fine_28_join_32,Roundrobin_23_iDCT8x8_1D_col_fast_fine_28);
	iDCT8x8_1D_col_fast_fine_29 iDCT8x8_1D_col_fast_fine_29_obj(iDCT8x8_1D_col_fast_fine_29_join_32,Roundrobin_23_iDCT8x8_1D_col_fast_fine_29);
	iDCT8x8_1D_col_fast_fine_30 iDCT8x8_1D_col_fast_fine_30_obj(iDCT8x8_1D_col_fast_fine_30_join_32,Roundrobin_23_iDCT8x8_1D_col_fast_fine_30);
	iDCT8x8_1D_col_fast_fine_31 iDCT8x8_1D_col_fast_fine_31_obj(iDCT8x8_1D_col_fast_fine_31_join_32,Roundrobin_23_iDCT8x8_1D_col_fast_fine_31);
	join_32 join_32_obj(join_32_BoundedSaturation_33,iDCT8x8_1D_col_fast_fine_24_join_32,iDCT8x8_1D_col_fast_fine_25_join_32,iDCT8x8_1D_col_fast_fine_26_join_32,iDCT8x8_1D_col_fast_fine_27_join_32,iDCT8x8_1D_col_fast_fine_28_join_32,iDCT8x8_1D_col_fast_fine_29_join_32,iDCT8x8_1D_col_fast_fine_30_join_32,iDCT8x8_1D_col_fast_fine_31_join_32);
	BoundedSaturation_33 BoundedSaturation_33_obj(BoundedSaturation_33_join_37,join_32_BoundedSaturation_33);
	Repeat_35 Repeat_35_obj(Repeat_35_join_37,MotionVectorDecode_34_Repeat_35);
	Repeat_36 Repeat_36_obj(Repeat_36_join_37,Roundrobin_1_Repeat_36);
	join_37 join_37_obj(join_37_Sink_38,BoundedSaturation_33_join_37,Repeat_35_join_37,Repeat_36_join_37);
	Sink_38 Sink_38_obj(join_37_Sink_38);
	char stage[4]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<4;_stageNum++)
	{
		if(3==_stageNum)
		{
			MismatchControl_12_obj.runInitScheduleWork();
			Roundrobin_13_obj.runInitScheduleWork();
			iDCT8x8_1D_row_fast_14_obj.runInitScheduleWork();
			iDCT8x8_1D_row_fast_15_obj.runInitScheduleWork();
			iDCT8x8_1D_row_fast_16_obj.runInitScheduleWork();
			iDCT8x8_1D_row_fast_17_obj.runInitScheduleWork();
			iDCT8x8_1D_row_fast_18_obj.runInitScheduleWork();
			iDCT8x8_1D_row_fast_19_obj.runInitScheduleWork();
			iDCT8x8_1D_row_fast_20_obj.runInitScheduleWork();
			iDCT8x8_1D_row_fast_21_obj.runInitScheduleWork();
			join_22_obj.runInitScheduleWork();
			Roundrobin_23_obj.runInitScheduleWork();
			iDCT8x8_1D_col_fast_fine_24_obj.runInitScheduleWork();
			iDCT8x8_1D_col_fast_fine_25_obj.runInitScheduleWork();
			iDCT8x8_1D_col_fast_fine_26_obj.runInitScheduleWork();
			iDCT8x8_1D_col_fast_fine_27_obj.runInitScheduleWork();
			iDCT8x8_1D_col_fast_fine_28_obj.runInitScheduleWork();
			iDCT8x8_1D_col_fast_fine_29_obj.runInitScheduleWork();
			iDCT8x8_1D_col_fast_fine_30_obj.runInitScheduleWork();
			iDCT8x8_1D_col_fast_fine_31_obj.runInitScheduleWork();
			join_32_obj.runInitScheduleWork();
			BoundedSaturation_33_obj.runInitScheduleWork();
			Repeat_35_obj.runInitScheduleWork();
			join_37_obj.runInitScheduleWork();
			Sink_38_obj.runInitScheduleWork();
		}
		if(1==_stageNum)
		{
			Roundrobin_1_obj.runInitScheduleWork();
			Repeat_36_obj.runInitScheduleWork();
		}
	
		masterSync(2);
	}
	for(int _stageNum=4;_stageNum<2*4+MAX_ITER-1;_stageNum++)
	{
		if(stage[3])
		{
			MismatchControl_12_obj.runSteadyScheduleWork();
			Roundrobin_13_obj.runSteadyScheduleWork();
			iDCT8x8_1D_row_fast_14_obj.runSteadyScheduleWork();
			iDCT8x8_1D_row_fast_15_obj.runSteadyScheduleWork();
			iDCT8x8_1D_row_fast_16_obj.runSteadyScheduleWork();
			iDCT8x8_1D_row_fast_17_obj.runSteadyScheduleWork();
			iDCT8x8_1D_row_fast_18_obj.runSteadyScheduleWork();
			iDCT8x8_1D_row_fast_19_obj.runSteadyScheduleWork();
			iDCT8x8_1D_row_fast_20_obj.runSteadyScheduleWork();
			iDCT8x8_1D_row_fast_21_obj.runSteadyScheduleWork();
			join_22_obj.runSteadyScheduleWork();
			Roundrobin_23_obj.runSteadyScheduleWork();
			iDCT8x8_1D_col_fast_fine_24_obj.runSteadyScheduleWork();
			iDCT8x8_1D_col_fast_fine_25_obj.runSteadyScheduleWork();
			iDCT8x8_1D_col_fast_fine_26_obj.runSteadyScheduleWork();
			iDCT8x8_1D_col_fast_fine_27_obj.runSteadyScheduleWork();
			iDCT8x8_1D_col_fast_fine_28_obj.runSteadyScheduleWork();
			iDCT8x8_1D_col_fast_fine_29_obj.runSteadyScheduleWork();
			iDCT8x8_1D_col_fast_fine_30_obj.runSteadyScheduleWork();
			iDCT8x8_1D_col_fast_fine_31_obj.runSteadyScheduleWork();
			join_32_obj.runSteadyScheduleWork();
			BoundedSaturation_33_obj.runSteadyScheduleWork();
			Repeat_35_obj.runSteadyScheduleWork();
			join_37_obj.runSteadyScheduleWork();
			Sink_38_obj.runSteadyScheduleWork();
		}
		if(stage[1])
		{
			Roundrobin_1_obj.runSteadyScheduleWork();
			Repeat_36_obj.runSteadyScheduleWork();
		}
		for(int index=3; index>= 1; --index)
			stage[index] = stage[index-1];
		if(_stageNum == (MAX_ITER - 1 + 4))
		{
			stage[0]=0;
		}
	
		masterSync(2);
	}
}
