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
	LowPassFilterX_30 LowPassFilterX_30_obj(LowPassFilterX_30_join_31,Duplicate_28_LowPassFilterX_30);
	join_31 join_31_obj(join_31_Subtracter_32,LowPassFilterX_29_join_31,LowPassFilterX_30_join_31);
	Subtracter_32 Subtracter_32_obj(Subtracter_32_AmplifyX_33,join_31_Subtracter_32);
	LowPassFilterX_35 LowPassFilterX_35_obj(LowPassFilterX_35_join_37,Duplicate_34_LowPassFilterX_35);
	LowPassFilterX_36 LowPassFilterX_36_obj(LowPassFilterX_36_join_37,Duplicate_34_LowPassFilterX_36);
	join_37 join_37_obj(join_37_Subtracter_38,LowPassFilterX_35_join_37,LowPassFilterX_36_join_37);
	Subtracter_38 Subtracter_38_obj(Subtracter_38_AmplifyX_39,join_37_Subtracter_38);
	AmplifyX_39 AmplifyX_39_obj(AmplifyX_39_join_64,Subtracter_38_AmplifyX_39);
	Duplicate_40 Duplicate_40_obj(Duplicate_40_LowPassFilterX_41,Duplicate_40_LowPassFilterX_42,Duplicate_3_Duplicate_40);
	LowPassFilterX_41 LowPassFilterX_41_obj(LowPassFilterX_41_join_43,Duplicate_40_LowPassFilterX_41);
	LowPassFilterX_42 LowPassFilterX_42_obj(LowPassFilterX_42_join_43,Duplicate_40_LowPassFilterX_42);
	join_43 join_43_obj(join_43_Subtracter_44,LowPassFilterX_41_join_43,LowPassFilterX_42_join_43);
	Subtracter_44 Subtracter_44_obj(Subtracter_44_AmplifyX_45,join_43_Subtracter_44);
	AmplifyX_45 AmplifyX_45_obj(AmplifyX_45_join_64,Subtracter_44_AmplifyX_45);
	Duplicate_46 Duplicate_46_obj(Duplicate_46_LowPassFilterX_47,Duplicate_46_LowPassFilterX_48,Duplicate_3_Duplicate_46);
	LowPassFilterX_47 LowPassFilterX_47_obj(LowPassFilterX_47_join_49,Duplicate_46_LowPassFilterX_47);
	LowPassFilterX_48 LowPassFilterX_48_obj(LowPassFilterX_48_join_49,Duplicate_46_LowPassFilterX_48);
	join_49 join_49_obj(join_49_Subtracter_50,LowPassFilterX_47_join_49,LowPassFilterX_48_join_49);
	Subtracter_50 Subtracter_50_obj(Subtracter_50_AmplifyX_51,join_49_Subtracter_50);
	AmplifyX_51 AmplifyX_51_obj(AmplifyX_51_join_64,Subtracter_50_AmplifyX_51);
	Duplicate_52 Duplicate_52_obj(Duplicate_52_LowPassFilterX_53,Duplicate_52_LowPassFilterX_54,Duplicate_3_Duplicate_52);
	LowPassFilterX_53 LowPassFilterX_53_obj(LowPassFilterX_53_join_55,Duplicate_52_LowPassFilterX_53);
	LowPassFilterX_54 LowPassFilterX_54_obj(LowPassFilterX_54_join_55,Duplicate_52_LowPassFilterX_54);
	join_55 join_55_obj(join_55_Subtracter_56,LowPassFilterX_53_join_55,LowPassFilterX_54_join_55);
	Subtracter_56 Subtracter_56_obj(Subtracter_56_AmplifyX_57,join_55_Subtracter_56);
	AmplifyX_57 AmplifyX_57_obj(AmplifyX_57_join_64,Subtracter_56_AmplifyX_57);
	Duplicate_58 Duplicate_58_obj(Duplicate_58_LowPassFilterX_59,Duplicate_58_LowPassFilterX_60,Duplicate_3_Duplicate_58);
	LowPassFilterX_59 LowPassFilterX_59_obj(LowPassFilterX_59_join_61,Duplicate_58_LowPassFilterX_59);
	LowPassFilterX_60 LowPassFilterX_60_obj(LowPassFilterX_60_join_61,Duplicate_58_LowPassFilterX_60);
	join_61 join_61_obj(join_61_Subtracter_62,LowPassFilterX_59_join_61,LowPassFilterX_60_join_61);
	Subtracter_62 Subtracter_62_obj(Subtracter_62_AmplifyX_63,join_61_Subtracter_62);
	AmplifyX_63 AmplifyX_63_obj(AmplifyX_63_join_64,Subtracter_62_AmplifyX_63);
	char stage[5]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<5;_stageNum++)
	{
		if(2==_stageNum)
		{
			join_31_obj.runInitScheduleWork();
			Subtracter_32_obj.runInitScheduleWork();
		}
		if(1==_stageNum)
		{
			LowPassFilterX_30_obj.runInitScheduleWork();
			LowPassFilterX_35_obj.runInitScheduleWork();
			LowPassFilterX_36_obj.runInitScheduleWork();
			join_37_obj.runInitScheduleWork();
			Subtracter_38_obj.runInitScheduleWork();
			AmplifyX_39_obj.runInitScheduleWork();
			Duplicate_40_obj.runInitScheduleWork();
			LowPassFilterX_41_obj.runInitScheduleWork();
			LowPassFilterX_42_obj.runInitScheduleWork();
			join_43_obj.runInitScheduleWork();
			Subtracter_44_obj.runInitScheduleWork();
			AmplifyX_45_obj.runInitScheduleWork();
			Duplicate_46_obj.runInitScheduleWork();
			LowPassFilterX_47_obj.runInitScheduleWork();
			LowPassFilterX_48_obj.runInitScheduleWork();
			join_49_obj.runInitScheduleWork();
			Subtracter_50_obj.runInitScheduleWork();
			AmplifyX_51_obj.runInitScheduleWork();
			Duplicate_52_obj.runInitScheduleWork();
			LowPassFilterX_53_obj.runInitScheduleWork();
			LowPassFilterX_54_obj.runInitScheduleWork();
			join_55_obj.runInitScheduleWork();
			Subtracter_56_obj.runInitScheduleWork();
			AmplifyX_57_obj.runInitScheduleWork();
			Duplicate_58_obj.runInitScheduleWork();
			LowPassFilterX_59_obj.runInitScheduleWork();
			LowPassFilterX_60_obj.runInitScheduleWork();
			join_61_obj.runInitScheduleWork();
			Subtracter_62_obj.runInitScheduleWork();
			AmplifyX_63_obj.runInitScheduleWork();
		}
	
		masterSync(2);
	}
	for(int _stageNum=5;_stageNum<2*5+MAX_ITER-1;_stageNum++)
	{
		if(stage[2])
		{
			join_31_obj.runSteadyScheduleWork();
			Subtracter_32_obj.runSteadyScheduleWork();
		}
		if(stage[1])
		{
			LowPassFilterX_30_obj.runSteadyScheduleWork();
			LowPassFilterX_35_obj.runSteadyScheduleWork();
			LowPassFilterX_36_obj.runSteadyScheduleWork();
			join_37_obj.runSteadyScheduleWork();
			Subtracter_38_obj.runSteadyScheduleWork();
			AmplifyX_39_obj.runSteadyScheduleWork();
			Duplicate_40_obj.runSteadyScheduleWork();
			LowPassFilterX_41_obj.runSteadyScheduleWork();
			LowPassFilterX_42_obj.runSteadyScheduleWork();
			join_43_obj.runSteadyScheduleWork();
			Subtracter_44_obj.runSteadyScheduleWork();
			AmplifyX_45_obj.runSteadyScheduleWork();
			Duplicate_46_obj.runSteadyScheduleWork();
			LowPassFilterX_47_obj.runSteadyScheduleWork();
			LowPassFilterX_48_obj.runSteadyScheduleWork();
			join_49_obj.runSteadyScheduleWork();
			Subtracter_50_obj.runSteadyScheduleWork();
			AmplifyX_51_obj.runSteadyScheduleWork();
			Duplicate_52_obj.runSteadyScheduleWork();
			LowPassFilterX_53_obj.runSteadyScheduleWork();
			LowPassFilterX_54_obj.runSteadyScheduleWork();
			join_55_obj.runSteadyScheduleWork();
			Subtracter_56_obj.runSteadyScheduleWork();
			AmplifyX_57_obj.runSteadyScheduleWork();
			Duplicate_58_obj.runSteadyScheduleWork();
			LowPassFilterX_59_obj.runSteadyScheduleWork();
			LowPassFilterX_60_obj.runSteadyScheduleWork();
			join_61_obj.runSteadyScheduleWork();
			Subtracter_62_obj.runSteadyScheduleWork();
			AmplifyX_63_obj.runSteadyScheduleWork();
		}
		for(int index=4; index>= 1; --index)
			stage[index] = stage[index-1];
		if(_stageNum == (MAX_ITER - 1 + 5))
		{
			stage[0]=0;
		}
	
		masterSync(2);
	}
}
