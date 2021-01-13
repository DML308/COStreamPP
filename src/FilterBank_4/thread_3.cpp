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
	DataSource_0 DataSource_0_obj(DataSource_0_Duplicate_1);
	Duplicate_1 Duplicate_1_obj(Duplicate_1_LowPassFilterX_2,Duplicate_1_LowPassFilterX_12,Duplicate_1_LowPassFilterX_22,Duplicate_1_LowPassFilterX_32,Duplicate_1_LowPassFilterX_42,Duplicate_1_LowPassFilterX_52,Duplicate_1_LowPassFilterX_62,Duplicate_1_LowPassFilterX_72,Duplicate_1_LowPassFilterX_82,Duplicate_1_LowPassFilterX_92,Duplicate_1_LowPassFilterX_102,Duplicate_1_LowPassFilterX_112,Duplicate_1_LowPassFilterX_122,Duplicate_1_LowPassFilterX_132,Duplicate_1_LowPassFilterX_142,Duplicate_1_LowPassFilterX_152,Duplicate_1_LowPassFilterX_162,Duplicate_1_LowPassFilterX_172,Duplicate_1_LowPassFilterX_182,Duplicate_1_LowPassFilterX_192,Duplicate_1_LowPassFilterX_202,Duplicate_1_LowPassFilterX_212,Duplicate_1_LowPassFilterX_222,Duplicate_1_LowPassFilterX_232,Duplicate_1_LowPassFilterX_242,Duplicate_1_LowPassFilterX_252,Duplicate_1_LowPassFilterX_262,Duplicate_1_LowPassFilterX_272,Duplicate_1_LowPassFilterX_282,Duplicate_1_LowPassFilterX_292,Duplicate_1_LowPassFilterX_302,Duplicate_1_LowPassFilterX_312,DataSource_0_Duplicate_1);
	LowPassFilterX_2 LowPassFilterX_2_obj(LowPassFilterX_2_HighPassFilterX_3,Duplicate_1_LowPassFilterX_2);
	HighPassFilterX_3 HighPassFilterX_3_obj(HighPassFilterX_3_Compressor_4,LowPassFilterX_2_HighPassFilterX_3);
	Compressor_4 Compressor_4_obj(Compressor_4_ProcessFilter_5,HighPassFilterX_3_Compressor_4);
	ProcessFilter_5 ProcessFilter_5_obj(ProcessFilter_5_Expander_6,Compressor_4_ProcessFilter_5);
	Expander_6 Expander_6_obj(Expander_6_Duplicate_7,ProcessFilter_5_Expander_6);
	Duplicate_7 Duplicate_7_obj(Duplicate_7_LowPassFilterX_8,Duplicate_7_HighPassFilterX_9,Expander_6_Duplicate_7);
	LowPassFilterX_8 LowPassFilterX_8_obj(LowPassFilterX_8_join_10,Duplicate_7_LowPassFilterX_8);
	HighPassFilterX_9 HighPassFilterX_9_obj(HighPassFilterX_9_join_10,Duplicate_7_HighPassFilterX_9);
	join_10 join_10_obj(join_10_Adder_11,LowPassFilterX_8_join_10,HighPassFilterX_9_join_10);
	Adder_11 Adder_11_obj(Adder_11_join_322,join_10_Adder_11);
	LowPassFilterX_22 LowPassFilterX_22_obj(LowPassFilterX_22_HighPassFilterX_23,Duplicate_1_LowPassFilterX_22);
	LowPassFilterX_32 LowPassFilterX_32_obj(LowPassFilterX_32_HighPassFilterX_33,Duplicate_1_LowPassFilterX_32);
	LowPassFilterX_42 LowPassFilterX_42_obj(LowPassFilterX_42_HighPassFilterX_43,Duplicate_1_LowPassFilterX_42);
	LowPassFilterX_52 LowPassFilterX_52_obj(LowPassFilterX_52_HighPassFilterX_53,Duplicate_1_LowPassFilterX_52);
	LowPassFilterX_62 LowPassFilterX_62_obj(LowPassFilterX_62_HighPassFilterX_63,Duplicate_1_LowPassFilterX_62);
	LowPassFilterX_72 LowPassFilterX_72_obj(LowPassFilterX_72_HighPassFilterX_73,Duplicate_1_LowPassFilterX_72);
	LowPassFilterX_82 LowPassFilterX_82_obj(LowPassFilterX_82_HighPassFilterX_83,Duplicate_1_LowPassFilterX_82);
	LowPassFilterX_92 LowPassFilterX_92_obj(LowPassFilterX_92_HighPassFilterX_93,Duplicate_1_LowPassFilterX_92);
	LowPassFilterX_102 LowPassFilterX_102_obj(LowPassFilterX_102_HighPassFilterX_103,Duplicate_1_LowPassFilterX_102);
	LowPassFilterX_122 LowPassFilterX_122_obj(LowPassFilterX_122_HighPassFilterX_123,Duplicate_1_LowPassFilterX_122);
	LowPassFilterX_132 LowPassFilterX_132_obj(LowPassFilterX_132_HighPassFilterX_133,Duplicate_1_LowPassFilterX_132);
	LowPassFilterX_142 LowPassFilterX_142_obj(LowPassFilterX_142_HighPassFilterX_143,Duplicate_1_LowPassFilterX_142);
	LowPassFilterX_162 LowPassFilterX_162_obj(LowPassFilterX_162_HighPassFilterX_163,Duplicate_1_LowPassFilterX_162);
	LowPassFilterX_172 LowPassFilterX_172_obj(LowPassFilterX_172_HighPassFilterX_173,Duplicate_1_LowPassFilterX_172);
	LowPassFilterX_182 LowPassFilterX_182_obj(LowPassFilterX_182_HighPassFilterX_183,Duplicate_1_LowPassFilterX_182);
	LowPassFilterX_192 LowPassFilterX_192_obj(LowPassFilterX_192_HighPassFilterX_193,Duplicate_1_LowPassFilterX_192);
	LowPassFilterX_202 LowPassFilterX_202_obj(LowPassFilterX_202_HighPassFilterX_203,Duplicate_1_LowPassFilterX_202);
	LowPassFilterX_212 LowPassFilterX_212_obj(LowPassFilterX_212_HighPassFilterX_213,Duplicate_1_LowPassFilterX_212);
	HighPassFilterX_213 HighPassFilterX_213_obj(HighPassFilterX_213_Compressor_214,LowPassFilterX_212_HighPassFilterX_213);
	Compressor_214 Compressor_214_obj(Compressor_214_ProcessFilter_215,HighPassFilterX_213_Compressor_214);
	ProcessFilter_215 ProcessFilter_215_obj(ProcessFilter_215_Expander_216,Compressor_214_ProcessFilter_215);
	Expander_216 Expander_216_obj(Expander_216_Duplicate_217,ProcessFilter_215_Expander_216);
	LowPassFilterX_222 LowPassFilterX_222_obj(LowPassFilterX_222_HighPassFilterX_223,Duplicate_1_LowPassFilterX_222);
	HighPassFilterX_223 HighPassFilterX_223_obj(HighPassFilterX_223_Compressor_224,LowPassFilterX_222_HighPassFilterX_223);
	Compressor_224 Compressor_224_obj(Compressor_224_ProcessFilter_225,HighPassFilterX_223_Compressor_224);
	ProcessFilter_225 ProcessFilter_225_obj(ProcessFilter_225_Expander_226,Compressor_224_ProcessFilter_225);
	Expander_226 Expander_226_obj(Expander_226_Duplicate_227,ProcessFilter_225_Expander_226);
	Duplicate_227 Duplicate_227_obj(Duplicate_227_LowPassFilterX_228,Duplicate_227_HighPassFilterX_229,Expander_226_Duplicate_227);
	LowPassFilterX_232 LowPassFilterX_232_obj(LowPassFilterX_232_HighPassFilterX_233,Duplicate_1_LowPassFilterX_232);
	HighPassFilterX_233 HighPassFilterX_233_obj(HighPassFilterX_233_Compressor_234,LowPassFilterX_232_HighPassFilterX_233);
	Compressor_234 Compressor_234_obj(Compressor_234_ProcessFilter_235,HighPassFilterX_233_Compressor_234);
	ProcessFilter_235 ProcessFilter_235_obj(ProcessFilter_235_Expander_236,Compressor_234_ProcessFilter_235);
	Expander_236 Expander_236_obj(Expander_236_Duplicate_237,ProcessFilter_235_Expander_236);
	LowPassFilterX_242 LowPassFilterX_242_obj(LowPassFilterX_242_HighPassFilterX_243,Duplicate_1_LowPassFilterX_242);
	LowPassFilterX_252 LowPassFilterX_252_obj(LowPassFilterX_252_HighPassFilterX_253,Duplicate_1_LowPassFilterX_252);
	HighPassFilterX_253 HighPassFilterX_253_obj(HighPassFilterX_253_Compressor_254,LowPassFilterX_252_HighPassFilterX_253);
	Compressor_254 Compressor_254_obj(Compressor_254_ProcessFilter_255,HighPassFilterX_253_Compressor_254);
	ProcessFilter_255 ProcessFilter_255_obj(ProcessFilter_255_Expander_256,Compressor_254_ProcessFilter_255);
	Expander_256 Expander_256_obj(Expander_256_Duplicate_257,ProcessFilter_255_Expander_256);
	Duplicate_257 Duplicate_257_obj(Duplicate_257_LowPassFilterX_258,Duplicate_257_HighPassFilterX_259,Expander_256_Duplicate_257);
	LowPassFilterX_262 LowPassFilterX_262_obj(LowPassFilterX_262_HighPassFilterX_263,Duplicate_1_LowPassFilterX_262);
	HighPassFilterX_263 HighPassFilterX_263_obj(HighPassFilterX_263_Compressor_264,LowPassFilterX_262_HighPassFilterX_263);
	Compressor_264 Compressor_264_obj(Compressor_264_ProcessFilter_265,HighPassFilterX_263_Compressor_264);
	ProcessFilter_265 ProcessFilter_265_obj(ProcessFilter_265_Expander_266,Compressor_264_ProcessFilter_265);
	char stage[5]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<5;_stageNum++)
	{
		if(0==_stageNum)
		{
			DataSource_0_obj.runInitScheduleWork();
			Duplicate_1_obj.runInitScheduleWork();
			LowPassFilterX_2_obj.runInitScheduleWork();
			HighPassFilterX_3_obj.runInitScheduleWork();
			Compressor_4_obj.runInitScheduleWork();
			ProcessFilter_5_obj.runInitScheduleWork();
			Expander_6_obj.runInitScheduleWork();
			Duplicate_7_obj.runInitScheduleWork();
			LowPassFilterX_8_obj.runInitScheduleWork();
			HighPassFilterX_9_obj.runInitScheduleWork();
			join_10_obj.runInitScheduleWork();
			Adder_11_obj.runInitScheduleWork();
			LowPassFilterX_22_obj.runInitScheduleWork();
			LowPassFilterX_32_obj.runInitScheduleWork();
			LowPassFilterX_42_obj.runInitScheduleWork();
			LowPassFilterX_52_obj.runInitScheduleWork();
			LowPassFilterX_62_obj.runInitScheduleWork();
			LowPassFilterX_72_obj.runInitScheduleWork();
			LowPassFilterX_82_obj.runInitScheduleWork();
			LowPassFilterX_92_obj.runInitScheduleWork();
			LowPassFilterX_102_obj.runInitScheduleWork();
			LowPassFilterX_122_obj.runInitScheduleWork();
			LowPassFilterX_132_obj.runInitScheduleWork();
			LowPassFilterX_142_obj.runInitScheduleWork();
			LowPassFilterX_162_obj.runInitScheduleWork();
			LowPassFilterX_172_obj.runInitScheduleWork();
			LowPassFilterX_182_obj.runInitScheduleWork();
			LowPassFilterX_192_obj.runInitScheduleWork();
			LowPassFilterX_202_obj.runInitScheduleWork();
			LowPassFilterX_212_obj.runInitScheduleWork();
			HighPassFilterX_213_obj.runInitScheduleWork();
			Compressor_214_obj.runInitScheduleWork();
			ProcessFilter_215_obj.runInitScheduleWork();
			Expander_216_obj.runInitScheduleWork();
			LowPassFilterX_222_obj.runInitScheduleWork();
			HighPassFilterX_223_obj.runInitScheduleWork();
			Compressor_224_obj.runInitScheduleWork();
			ProcessFilter_225_obj.runInitScheduleWork();
			Expander_226_obj.runInitScheduleWork();
			Duplicate_227_obj.runInitScheduleWork();
			LowPassFilterX_232_obj.runInitScheduleWork();
			HighPassFilterX_233_obj.runInitScheduleWork();
			Compressor_234_obj.runInitScheduleWork();
			ProcessFilter_235_obj.runInitScheduleWork();
			Expander_236_obj.runInitScheduleWork();
			LowPassFilterX_242_obj.runInitScheduleWork();
			LowPassFilterX_252_obj.runInitScheduleWork();
			HighPassFilterX_253_obj.runInitScheduleWork();
			Compressor_254_obj.runInitScheduleWork();
			ProcessFilter_255_obj.runInitScheduleWork();
			Expander_256_obj.runInitScheduleWork();
			Duplicate_257_obj.runInitScheduleWork();
			LowPassFilterX_262_obj.runInitScheduleWork();
			HighPassFilterX_263_obj.runInitScheduleWork();
			Compressor_264_obj.runInitScheduleWork();
			ProcessFilter_265_obj.runInitScheduleWork();
		}
	
		workerSync(3);
	}
	for(int _stageNum=5;_stageNum<2*5+MAX_ITER-1;_stageNum++)
	{
		if(stage[0])
		{
			DataSource_0_obj.runSteadyScheduleWork();
			Duplicate_1_obj.runSteadyScheduleWork();
			LowPassFilterX_2_obj.runSteadyScheduleWork();
			HighPassFilterX_3_obj.runSteadyScheduleWork();
			Compressor_4_obj.runSteadyScheduleWork();
			ProcessFilter_5_obj.runSteadyScheduleWork();
			Expander_6_obj.runSteadyScheduleWork();
			Duplicate_7_obj.runSteadyScheduleWork();
			LowPassFilterX_8_obj.runSteadyScheduleWork();
			HighPassFilterX_9_obj.runSteadyScheduleWork();
			join_10_obj.runSteadyScheduleWork();
			Adder_11_obj.runSteadyScheduleWork();
			LowPassFilterX_22_obj.runSteadyScheduleWork();
			LowPassFilterX_32_obj.runSteadyScheduleWork();
			LowPassFilterX_42_obj.runSteadyScheduleWork();
			LowPassFilterX_52_obj.runSteadyScheduleWork();
			LowPassFilterX_62_obj.runSteadyScheduleWork();
			LowPassFilterX_72_obj.runSteadyScheduleWork();
			LowPassFilterX_82_obj.runSteadyScheduleWork();
			LowPassFilterX_92_obj.runSteadyScheduleWork();
			LowPassFilterX_102_obj.runSteadyScheduleWork();
			LowPassFilterX_122_obj.runSteadyScheduleWork();
			LowPassFilterX_132_obj.runSteadyScheduleWork();
			LowPassFilterX_142_obj.runSteadyScheduleWork();
			LowPassFilterX_162_obj.runSteadyScheduleWork();
			LowPassFilterX_172_obj.runSteadyScheduleWork();
			LowPassFilterX_182_obj.runSteadyScheduleWork();
			LowPassFilterX_192_obj.runSteadyScheduleWork();
			LowPassFilterX_202_obj.runSteadyScheduleWork();
			LowPassFilterX_212_obj.runSteadyScheduleWork();
			HighPassFilterX_213_obj.runSteadyScheduleWork();
			Compressor_214_obj.runSteadyScheduleWork();
			ProcessFilter_215_obj.runSteadyScheduleWork();
			Expander_216_obj.runSteadyScheduleWork();
			LowPassFilterX_222_obj.runSteadyScheduleWork();
			HighPassFilterX_223_obj.runSteadyScheduleWork();
			Compressor_224_obj.runSteadyScheduleWork();
			ProcessFilter_225_obj.runSteadyScheduleWork();
			Expander_226_obj.runSteadyScheduleWork();
			Duplicate_227_obj.runSteadyScheduleWork();
			LowPassFilterX_232_obj.runSteadyScheduleWork();
			HighPassFilterX_233_obj.runSteadyScheduleWork();
			Compressor_234_obj.runSteadyScheduleWork();
			ProcessFilter_235_obj.runSteadyScheduleWork();
			Expander_236_obj.runSteadyScheduleWork();
			LowPassFilterX_242_obj.runSteadyScheduleWork();
			LowPassFilterX_252_obj.runSteadyScheduleWork();
			HighPassFilterX_253_obj.runSteadyScheduleWork();
			Compressor_254_obj.runSteadyScheduleWork();
			ProcessFilter_255_obj.runSteadyScheduleWork();
			Expander_256_obj.runSteadyScheduleWork();
			Duplicate_257_obj.runSteadyScheduleWork();
			LowPassFilterX_262_obj.runSteadyScheduleWork();
			HighPassFilterX_263_obj.runSteadyScheduleWork();
			Compressor_264_obj.runSteadyScheduleWork();
			ProcessFilter_265_obj.runSteadyScheduleWork();
		}
		for(int index=4; index>= 1; --index)
			stage[index] = stage[index-1];
		if(_stageNum == (MAX_ITER - 1 + 5))
		{
			stage[0]=0;
		}
	
		workerSync(3);
	}
}
