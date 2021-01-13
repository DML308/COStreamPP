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
	FloatSource_0 FloatSource_0_obj(FloatSource_0_Duplicate_1);
	Duplicate_1 Duplicate_1_obj(Duplicate_1_InputGenerate_2,Duplicate_1_InputGenerate_5,Duplicate_1_InputGenerate_8,Duplicate_1_InputGenerate_11,Duplicate_1_InputGenerate_14,Duplicate_1_InputGenerate_17,Duplicate_1_InputGenerate_20,Duplicate_1_InputGenerate_23,Duplicate_1_InputGenerate_26,Duplicate_1_InputGenerate_29,Duplicate_1_InputGenerate_32,Duplicate_1_InputGenerate_35,FloatSource_0_Duplicate_1);
	InputGenerate_2 InputGenerate_2_obj(InputGenerate_2_BeamFirFilter_3,Duplicate_1_InputGenerate_2);
	BeamFirFilter_3 BeamFirFilter_3_obj(BeamFirFilter_3_BeamFirFilter_4,InputGenerate_2_BeamFirFilter_3);
	BeamFirFilter_4 BeamFirFilter_4_obj(BeamFirFilter_4_join_38,BeamFirFilter_3_BeamFirFilter_4);
	InputGenerate_5 InputGenerate_5_obj(InputGenerate_5_BeamFirFilter_6,Duplicate_1_InputGenerate_5);
	BeamFirFilter_6 BeamFirFilter_6_obj(BeamFirFilter_6_BeamFirFilter_7,InputGenerate_5_BeamFirFilter_6);
	BeamFirFilter_7 BeamFirFilter_7_obj(BeamFirFilter_7_join_38,BeamFirFilter_6_BeamFirFilter_7);
	InputGenerate_8 InputGenerate_8_obj(InputGenerate_8_BeamFirFilter_9,Duplicate_1_InputGenerate_8);
	BeamFirFilter_9 BeamFirFilter_9_obj(BeamFirFilter_9_BeamFirFilter_10,InputGenerate_8_BeamFirFilter_9);
	BeamFirFilter_10 BeamFirFilter_10_obj(BeamFirFilter_10_join_38,BeamFirFilter_9_BeamFirFilter_10);
	InputGenerate_11 InputGenerate_11_obj(InputGenerate_11_BeamFirFilter_12,Duplicate_1_InputGenerate_11);
	BeamFirFilter_12 BeamFirFilter_12_obj(BeamFirFilter_12_BeamFirFilter_13,InputGenerate_11_BeamFirFilter_12);
	BeamFirFilter_13 BeamFirFilter_13_obj(BeamFirFilter_13_join_38,BeamFirFilter_12_BeamFirFilter_13);
	InputGenerate_14 InputGenerate_14_obj(InputGenerate_14_BeamFirFilter_15,Duplicate_1_InputGenerate_14);
	BeamFirFilter_15 BeamFirFilter_15_obj(BeamFirFilter_15_BeamFirFilter_16,InputGenerate_14_BeamFirFilter_15);
	BeamFirFilter_16 BeamFirFilter_16_obj(BeamFirFilter_16_join_38,BeamFirFilter_15_BeamFirFilter_16);
	InputGenerate_17 InputGenerate_17_obj(InputGenerate_17_BeamFirFilter_18,Duplicate_1_InputGenerate_17);
	BeamFirFilter_18 BeamFirFilter_18_obj(BeamFirFilter_18_BeamFirFilter_19,InputGenerate_17_BeamFirFilter_18);
	BeamFirFilter_19 BeamFirFilter_19_obj(BeamFirFilter_19_join_38,BeamFirFilter_18_BeamFirFilter_19);
	InputGenerate_20 InputGenerate_20_obj(InputGenerate_20_BeamFirFilter_21,Duplicate_1_InputGenerate_20);
	BeamFirFilter_21 BeamFirFilter_21_obj(BeamFirFilter_21_BeamFirFilter_22,InputGenerate_20_BeamFirFilter_21);
	BeamFirFilter_22 BeamFirFilter_22_obj(BeamFirFilter_22_join_38,BeamFirFilter_21_BeamFirFilter_22);
	InputGenerate_23 InputGenerate_23_obj(InputGenerate_23_BeamFirFilter_24,Duplicate_1_InputGenerate_23);
	BeamFirFilter_24 BeamFirFilter_24_obj(BeamFirFilter_24_BeamFirFilter_25,InputGenerate_23_BeamFirFilter_24);
	BeamFirFilter_25 BeamFirFilter_25_obj(BeamFirFilter_25_join_38,BeamFirFilter_24_BeamFirFilter_25);
	InputGenerate_26 InputGenerate_26_obj(InputGenerate_26_BeamFirFilter_27,Duplicate_1_InputGenerate_26);
	BeamFirFilter_27 BeamFirFilter_27_obj(BeamFirFilter_27_BeamFirFilter_28,InputGenerate_26_BeamFirFilter_27);
	BeamFirFilter_28 BeamFirFilter_28_obj(BeamFirFilter_28_join_38,BeamFirFilter_27_BeamFirFilter_28);
	InputGenerate_29 InputGenerate_29_obj(InputGenerate_29_BeamFirFilter_30,Duplicate_1_InputGenerate_29);
	BeamFirFilter_30 BeamFirFilter_30_obj(BeamFirFilter_30_BeamFirFilter_31,InputGenerate_29_BeamFirFilter_30);
	BeamFirFilter_31 BeamFirFilter_31_obj(BeamFirFilter_31_join_38,BeamFirFilter_30_BeamFirFilter_31);
	InputGenerate_32 InputGenerate_32_obj(InputGenerate_32_BeamFirFilter_33,Duplicate_1_InputGenerate_32);
	BeamFirFilter_33 BeamFirFilter_33_obj(BeamFirFilter_33_BeamFirFilter_34,InputGenerate_32_BeamFirFilter_33);
	BeamFirFilter_34 BeamFirFilter_34_obj(BeamFirFilter_34_join_38,BeamFirFilter_33_BeamFirFilter_34);
	InputGenerate_35 InputGenerate_35_obj(InputGenerate_35_BeamFirFilter_36,Duplicate_1_InputGenerate_35);
	BeamFirFilter_36 BeamFirFilter_36_obj(BeamFirFilter_36_BeamFirFilter_37,InputGenerate_35_BeamFirFilter_36);
	BeamFirFilter_37 BeamFirFilter_37_obj(BeamFirFilter_37_join_38,BeamFirFilter_36_BeamFirFilter_37);
	join_38 join_38_obj(join_38_Duplicate_39,BeamFirFilter_4_join_38,BeamFirFilter_7_join_38,BeamFirFilter_10_join_38,BeamFirFilter_13_join_38,BeamFirFilter_16_join_38,BeamFirFilter_19_join_38,BeamFirFilter_22_join_38,BeamFirFilter_25_join_38,BeamFirFilter_28_join_38,BeamFirFilter_31_join_38,BeamFirFilter_34_join_38,BeamFirFilter_37_join_38);
	Duplicate_39 Duplicate_39_obj(Duplicate_39_BeamForm_40,Duplicate_39_BeamForm_44,Duplicate_39_BeamForm_48,Duplicate_39_BeamForm_52,join_38_Duplicate_39);
	BeamForm_40 BeamForm_40_obj(BeamForm_40_BeamFirFilter_41,Duplicate_39_BeamForm_40);
	BeamFirFilter_41 BeamFirFilter_41_obj(BeamFirFilter_41_Magnitude_42,BeamForm_40_BeamFirFilter_41);
	Magnitude_42 Magnitude_42_obj(Magnitude_42_Detector_43,BeamFirFilter_41_Magnitude_42);
	Detector_43 Detector_43_obj(Detector_43_join_56,Magnitude_42_Detector_43);
	BeamForm_44 BeamForm_44_obj(BeamForm_44_BeamFirFilter_45,Duplicate_39_BeamForm_44);
	BeamFirFilter_45 BeamFirFilter_45_obj(BeamFirFilter_45_Magnitude_46,BeamForm_44_BeamFirFilter_45);
	Magnitude_46 Magnitude_46_obj(Magnitude_46_Detector_47,BeamFirFilter_45_Magnitude_46);
	Detector_47 Detector_47_obj(Detector_47_join_56,Magnitude_46_Detector_47);
	BeamForm_48 BeamForm_48_obj(BeamForm_48_BeamFirFilter_49,Duplicate_39_BeamForm_48);
	BeamFirFilter_49 BeamFirFilter_49_obj(BeamFirFilter_49_Magnitude_50,BeamForm_48_BeamFirFilter_49);
	Magnitude_50 Magnitude_50_obj(Magnitude_50_Detector_51,BeamFirFilter_49_Magnitude_50);
	Detector_51 Detector_51_obj(Detector_51_join_56,Magnitude_50_Detector_51);
	BeamForm_52 BeamForm_52_obj(BeamForm_52_BeamFirFilter_53,Duplicate_39_BeamForm_52);
	BeamFirFilter_53 BeamFirFilter_53_obj(BeamFirFilter_53_Magnitude_54,BeamForm_52_BeamFirFilter_53);
	Magnitude_54 Magnitude_54_obj(Magnitude_54_Detector_55,BeamFirFilter_53_Magnitude_54);
	Detector_55 Detector_55_obj(Detector_55_join_56,Magnitude_54_Detector_55);
	join_56 join_56_obj(join_56_FloatSink_57,Detector_43_join_56,Detector_47_join_56,Detector_51_join_56,Detector_55_join_56);
	FloatSink_57 FloatSink_57_obj(join_56_FloatSink_57);
	char stage[1]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<1;_stageNum++)
	{
		if(0==_stageNum)
		{
			FloatSource_0_obj.runInitScheduleWork();
			Duplicate_1_obj.runInitScheduleWork();
			InputGenerate_2_obj.runInitScheduleWork();
			BeamFirFilter_3_obj.runInitScheduleWork();
			BeamFirFilter_4_obj.runInitScheduleWork();
			InputGenerate_5_obj.runInitScheduleWork();
			BeamFirFilter_6_obj.runInitScheduleWork();
			BeamFirFilter_7_obj.runInitScheduleWork();
			InputGenerate_8_obj.runInitScheduleWork();
			BeamFirFilter_9_obj.runInitScheduleWork();
			BeamFirFilter_10_obj.runInitScheduleWork();
			InputGenerate_11_obj.runInitScheduleWork();
			BeamFirFilter_12_obj.runInitScheduleWork();
			BeamFirFilter_13_obj.runInitScheduleWork();
			InputGenerate_14_obj.runInitScheduleWork();
			BeamFirFilter_15_obj.runInitScheduleWork();
			BeamFirFilter_16_obj.runInitScheduleWork();
			InputGenerate_17_obj.runInitScheduleWork();
			BeamFirFilter_18_obj.runInitScheduleWork();
			BeamFirFilter_19_obj.runInitScheduleWork();
			InputGenerate_20_obj.runInitScheduleWork();
			BeamFirFilter_21_obj.runInitScheduleWork();
			BeamFirFilter_22_obj.runInitScheduleWork();
			InputGenerate_23_obj.runInitScheduleWork();
			BeamFirFilter_24_obj.runInitScheduleWork();
			BeamFirFilter_25_obj.runInitScheduleWork();
			InputGenerate_26_obj.runInitScheduleWork();
			BeamFirFilter_27_obj.runInitScheduleWork();
			BeamFirFilter_28_obj.runInitScheduleWork();
			InputGenerate_29_obj.runInitScheduleWork();
			BeamFirFilter_30_obj.runInitScheduleWork();
			BeamFirFilter_31_obj.runInitScheduleWork();
			InputGenerate_32_obj.runInitScheduleWork();
			BeamFirFilter_33_obj.runInitScheduleWork();
			BeamFirFilter_34_obj.runInitScheduleWork();
			InputGenerate_35_obj.runInitScheduleWork();
			BeamFirFilter_36_obj.runInitScheduleWork();
			BeamFirFilter_37_obj.runInitScheduleWork();
			join_38_obj.runInitScheduleWork();
			Duplicate_39_obj.runInitScheduleWork();
			BeamForm_40_obj.runInitScheduleWork();
			BeamFirFilter_41_obj.runInitScheduleWork();
			Magnitude_42_obj.runInitScheduleWork();
			Detector_43_obj.runInitScheduleWork();
			BeamForm_44_obj.runInitScheduleWork();
			BeamFirFilter_45_obj.runInitScheduleWork();
			Magnitude_46_obj.runInitScheduleWork();
			Detector_47_obj.runInitScheduleWork();
			BeamForm_48_obj.runInitScheduleWork();
			BeamFirFilter_49_obj.runInitScheduleWork();
			Magnitude_50_obj.runInitScheduleWork();
			Detector_51_obj.runInitScheduleWork();
			BeamForm_52_obj.runInitScheduleWork();
			BeamFirFilter_53_obj.runInitScheduleWork();
			Magnitude_54_obj.runInitScheduleWork();
			Detector_55_obj.runInitScheduleWork();
			join_56_obj.runInitScheduleWork();
			FloatSink_57_obj.runInitScheduleWork();
		}
	
		masterSync(1);
	}
	for(int _stageNum=1;_stageNum<2*1+MAX_ITER-1;_stageNum++)
	{
		if(stage[0])
		{
			FloatSource_0_obj.runSteadyScheduleWork();
			Duplicate_1_obj.runSteadyScheduleWork();
			InputGenerate_2_obj.runSteadyScheduleWork();
			BeamFirFilter_3_obj.runSteadyScheduleWork();
			BeamFirFilter_4_obj.runSteadyScheduleWork();
			InputGenerate_5_obj.runSteadyScheduleWork();
			BeamFirFilter_6_obj.runSteadyScheduleWork();
			BeamFirFilter_7_obj.runSteadyScheduleWork();
			InputGenerate_8_obj.runSteadyScheduleWork();
			BeamFirFilter_9_obj.runSteadyScheduleWork();
			BeamFirFilter_10_obj.runSteadyScheduleWork();
			InputGenerate_11_obj.runSteadyScheduleWork();
			BeamFirFilter_12_obj.runSteadyScheduleWork();
			BeamFirFilter_13_obj.runSteadyScheduleWork();
			InputGenerate_14_obj.runSteadyScheduleWork();
			BeamFirFilter_15_obj.runSteadyScheduleWork();
			BeamFirFilter_16_obj.runSteadyScheduleWork();
			InputGenerate_17_obj.runSteadyScheduleWork();
			BeamFirFilter_18_obj.runSteadyScheduleWork();
			BeamFirFilter_19_obj.runSteadyScheduleWork();
			InputGenerate_20_obj.runSteadyScheduleWork();
			BeamFirFilter_21_obj.runSteadyScheduleWork();
			BeamFirFilter_22_obj.runSteadyScheduleWork();
			InputGenerate_23_obj.runSteadyScheduleWork();
			BeamFirFilter_24_obj.runSteadyScheduleWork();
			BeamFirFilter_25_obj.runSteadyScheduleWork();
			InputGenerate_26_obj.runSteadyScheduleWork();
			BeamFirFilter_27_obj.runSteadyScheduleWork();
			BeamFirFilter_28_obj.runSteadyScheduleWork();
			InputGenerate_29_obj.runSteadyScheduleWork();
			BeamFirFilter_30_obj.runSteadyScheduleWork();
			BeamFirFilter_31_obj.runSteadyScheduleWork();
			InputGenerate_32_obj.runSteadyScheduleWork();
			BeamFirFilter_33_obj.runSteadyScheduleWork();
			BeamFirFilter_34_obj.runSteadyScheduleWork();
			InputGenerate_35_obj.runSteadyScheduleWork();
			BeamFirFilter_36_obj.runSteadyScheduleWork();
			BeamFirFilter_37_obj.runSteadyScheduleWork();
			join_38_obj.runSteadyScheduleWork();
			Duplicate_39_obj.runSteadyScheduleWork();
			BeamForm_40_obj.runSteadyScheduleWork();
			BeamFirFilter_41_obj.runSteadyScheduleWork();
			Magnitude_42_obj.runSteadyScheduleWork();
			Detector_43_obj.runSteadyScheduleWork();
			BeamForm_44_obj.runSteadyScheduleWork();
			BeamFirFilter_45_obj.runSteadyScheduleWork();
			Magnitude_46_obj.runSteadyScheduleWork();
			Detector_47_obj.runSteadyScheduleWork();
			BeamForm_48_obj.runSteadyScheduleWork();
			BeamFirFilter_49_obj.runSteadyScheduleWork();
			Magnitude_50_obj.runSteadyScheduleWork();
			Detector_51_obj.runSteadyScheduleWork();
			BeamForm_52_obj.runSteadyScheduleWork();
			BeamFirFilter_53_obj.runSteadyScheduleWork();
			Magnitude_54_obj.runSteadyScheduleWork();
			Detector_55_obj.runSteadyScheduleWork();
			join_56_obj.runSteadyScheduleWork();
			FloatSink_57_obj.runSteadyScheduleWork();
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
