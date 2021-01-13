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
	BeamFirFilter_25 BeamFirFilter_25_obj(BeamFirFilter_25_join_38,BeamFirFilter_24_BeamFirFilter_25);
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
	char stage[4]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<4;_stageNum++)
	{
		if(3==_stageNum)
		{
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
		if(2==_stageNum)
		{
			BeamFirFilter_25_obj.runInitScheduleWork();
			BeamFirFilter_34_obj.runInitScheduleWork();
		}
		if(1==_stageNum)
		{
			InputGenerate_35_obj.runInitScheduleWork();
			BeamFirFilter_36_obj.runInitScheduleWork();
			BeamFirFilter_37_obj.runInitScheduleWork();
		}
	
		workerSync(1);
	}
	for(int _stageNum=4;_stageNum<2*4+MAX_ITER-1;_stageNum++)
	{
		if(stage[3])
		{
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
		if(stage[2])
		{
			BeamFirFilter_25_obj.runSteadyScheduleWork();
			BeamFirFilter_34_obj.runSteadyScheduleWork();
		}
		if(stage[1])
		{
			InputGenerate_35_obj.runSteadyScheduleWork();
			BeamFirFilter_36_obj.runSteadyScheduleWork();
			BeamFirFilter_37_obj.runSteadyScheduleWork();
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
