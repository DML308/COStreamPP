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
	Duplicate_217 Duplicate_217_obj(Duplicate_217_LowPassFilterX_218,Duplicate_217_HighPassFilterX_219,Expander_216_Duplicate_217);
	LowPassFilterX_218 LowPassFilterX_218_obj(LowPassFilterX_218_join_220,Duplicate_217_LowPassFilterX_218);
	HighPassFilterX_219 HighPassFilterX_219_obj(HighPassFilterX_219_join_220,Duplicate_217_HighPassFilterX_219);
	LowPassFilterX_228 LowPassFilterX_228_obj(LowPassFilterX_228_join_230,Duplicate_227_LowPassFilterX_228);
	HighPassFilterX_229 HighPassFilterX_229_obj(HighPassFilterX_229_join_230,Duplicate_227_HighPassFilterX_229);
	Duplicate_237 Duplicate_237_obj(Duplicate_237_LowPassFilterX_238,Duplicate_237_HighPassFilterX_239,Expander_236_Duplicate_237);
	LowPassFilterX_238 LowPassFilterX_238_obj(LowPassFilterX_238_join_240,Duplicate_237_LowPassFilterX_238);
	HighPassFilterX_239 HighPassFilterX_239_obj(HighPassFilterX_239_join_240,Duplicate_237_HighPassFilterX_239);
	HighPassFilterX_243 HighPassFilterX_243_obj(HighPassFilterX_243_Compressor_244,LowPassFilterX_242_HighPassFilterX_243);
	Compressor_244 Compressor_244_obj(Compressor_244_ProcessFilter_245,HighPassFilterX_243_Compressor_244);
	ProcessFilter_245 ProcessFilter_245_obj(ProcessFilter_245_Expander_246,Compressor_244_ProcessFilter_245);
	Expander_246 Expander_246_obj(Expander_246_Duplicate_247,ProcessFilter_245_Expander_246);
	Duplicate_247 Duplicate_247_obj(Duplicate_247_LowPassFilterX_248,Duplicate_247_HighPassFilterX_249,Expander_246_Duplicate_247);
	LowPassFilterX_248 LowPassFilterX_248_obj(LowPassFilterX_248_join_250,Duplicate_247_LowPassFilterX_248);
	HighPassFilterX_249 HighPassFilterX_249_obj(HighPassFilterX_249_join_250,Duplicate_247_HighPassFilterX_249);
	LowPassFilterX_258 LowPassFilterX_258_obj(LowPassFilterX_258_join_260,Duplicate_257_LowPassFilterX_258);
	HighPassFilterX_259 HighPassFilterX_259_obj(HighPassFilterX_259_join_260,Duplicate_257_HighPassFilterX_259);
	Expander_266 Expander_266_obj(Expander_266_Duplicate_267,ProcessFilter_265_Expander_266);
	Duplicate_267 Duplicate_267_obj(Duplicate_267_LowPassFilterX_268,Duplicate_267_HighPassFilterX_269,Expander_266_Duplicate_267);
	LowPassFilterX_268 LowPassFilterX_268_obj(LowPassFilterX_268_join_270,Duplicate_267_LowPassFilterX_268);
	HighPassFilterX_269 HighPassFilterX_269_obj(HighPassFilterX_269_join_270,Duplicate_267_HighPassFilterX_269);
	LowPassFilterX_272 LowPassFilterX_272_obj(LowPassFilterX_272_HighPassFilterX_273,Duplicate_1_LowPassFilterX_272);
	HighPassFilterX_273 HighPassFilterX_273_obj(HighPassFilterX_273_Compressor_274,LowPassFilterX_272_HighPassFilterX_273);
	Compressor_274 Compressor_274_obj(Compressor_274_ProcessFilter_275,HighPassFilterX_273_Compressor_274);
	ProcessFilter_275 ProcessFilter_275_obj(ProcessFilter_275_Expander_276,Compressor_274_ProcessFilter_275);
	Expander_276 Expander_276_obj(Expander_276_Duplicate_277,ProcessFilter_275_Expander_276);
	Duplicate_277 Duplicate_277_obj(Duplicate_277_LowPassFilterX_278,Duplicate_277_HighPassFilterX_279,Expander_276_Duplicate_277);
	LowPassFilterX_278 LowPassFilterX_278_obj(LowPassFilterX_278_join_280,Duplicate_277_LowPassFilterX_278);
	HighPassFilterX_279 HighPassFilterX_279_obj(HighPassFilterX_279_join_280,Duplicate_277_HighPassFilterX_279);
	LowPassFilterX_282 LowPassFilterX_282_obj(LowPassFilterX_282_HighPassFilterX_283,Duplicate_1_LowPassFilterX_282);
	HighPassFilterX_283 HighPassFilterX_283_obj(HighPassFilterX_283_Compressor_284,LowPassFilterX_282_HighPassFilterX_283);
	Compressor_284 Compressor_284_obj(Compressor_284_ProcessFilter_285,HighPassFilterX_283_Compressor_284);
	ProcessFilter_285 ProcessFilter_285_obj(ProcessFilter_285_Expander_286,Compressor_284_ProcessFilter_285);
	Expander_286 Expander_286_obj(Expander_286_Duplicate_287,ProcessFilter_285_Expander_286);
	Duplicate_287 Duplicate_287_obj(Duplicate_287_LowPassFilterX_288,Duplicate_287_HighPassFilterX_289,Expander_286_Duplicate_287);
	LowPassFilterX_288 LowPassFilterX_288_obj(LowPassFilterX_288_join_290,Duplicate_287_LowPassFilterX_288);
	HighPassFilterX_289 HighPassFilterX_289_obj(HighPassFilterX_289_join_290,Duplicate_287_HighPassFilterX_289);
	LowPassFilterX_292 LowPassFilterX_292_obj(LowPassFilterX_292_HighPassFilterX_293,Duplicate_1_LowPassFilterX_292);
	HighPassFilterX_293 HighPassFilterX_293_obj(HighPassFilterX_293_Compressor_294,LowPassFilterX_292_HighPassFilterX_293);
	Compressor_294 Compressor_294_obj(Compressor_294_ProcessFilter_295,HighPassFilterX_293_Compressor_294);
	ProcessFilter_295 ProcessFilter_295_obj(ProcessFilter_295_Expander_296,Compressor_294_ProcessFilter_295);
	Expander_296 Expander_296_obj(Expander_296_Duplicate_297,ProcessFilter_295_Expander_296);
	Duplicate_297 Duplicate_297_obj(Duplicate_297_LowPassFilterX_298,Duplicate_297_HighPassFilterX_299,Expander_296_Duplicate_297);
	LowPassFilterX_298 LowPassFilterX_298_obj(LowPassFilterX_298_join_300,Duplicate_297_LowPassFilterX_298);
	HighPassFilterX_299 HighPassFilterX_299_obj(HighPassFilterX_299_join_300,Duplicate_297_HighPassFilterX_299);
	LowPassFilterX_302 LowPassFilterX_302_obj(LowPassFilterX_302_HighPassFilterX_303,Duplicate_1_LowPassFilterX_302);
	HighPassFilterX_303 HighPassFilterX_303_obj(HighPassFilterX_303_Compressor_304,LowPassFilterX_302_HighPassFilterX_303);
	Compressor_304 Compressor_304_obj(Compressor_304_ProcessFilter_305,HighPassFilterX_303_Compressor_304);
	ProcessFilter_305 ProcessFilter_305_obj(ProcessFilter_305_Expander_306,Compressor_304_ProcessFilter_305);
	Expander_306 Expander_306_obj(Expander_306_Duplicate_307,ProcessFilter_305_Expander_306);
	Duplicate_307 Duplicate_307_obj(Duplicate_307_LowPassFilterX_308,Duplicate_307_HighPassFilterX_309,Expander_306_Duplicate_307);
	LowPassFilterX_308 LowPassFilterX_308_obj(LowPassFilterX_308_join_310,Duplicate_307_LowPassFilterX_308);
	HighPassFilterX_309 HighPassFilterX_309_obj(HighPassFilterX_309_join_310,Duplicate_307_HighPassFilterX_309);
	LowPassFilterX_312 LowPassFilterX_312_obj(LowPassFilterX_312_HighPassFilterX_313,Duplicate_1_LowPassFilterX_312);
	HighPassFilterX_313 HighPassFilterX_313_obj(HighPassFilterX_313_Compressor_314,LowPassFilterX_312_HighPassFilterX_313);
	Compressor_314 Compressor_314_obj(Compressor_314_ProcessFilter_315,HighPassFilterX_313_Compressor_314);
	ProcessFilter_315 ProcessFilter_315_obj(ProcessFilter_315_Expander_316,Compressor_314_ProcessFilter_315);
	Expander_316 Expander_316_obj(Expander_316_Duplicate_317,ProcessFilter_315_Expander_316);
	Duplicate_317 Duplicate_317_obj(Duplicate_317_LowPassFilterX_318,Duplicate_317_HighPassFilterX_319,Expander_316_Duplicate_317);
	LowPassFilterX_318 LowPassFilterX_318_obj(LowPassFilterX_318_join_320,Duplicate_317_LowPassFilterX_318);
	HighPassFilterX_319 HighPassFilterX_319_obj(HighPassFilterX_319_join_320,Duplicate_317_HighPassFilterX_319);
	char stage[5]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<5;_stageNum++)
	{
		if(1==_stageNum)
		{
			Duplicate_217_obj.runInitScheduleWork();
			LowPassFilterX_218_obj.runInitScheduleWork();
			HighPassFilterX_219_obj.runInitScheduleWork();
			LowPassFilterX_228_obj.runInitScheduleWork();
			HighPassFilterX_229_obj.runInitScheduleWork();
			Duplicate_237_obj.runInitScheduleWork();
			LowPassFilterX_238_obj.runInitScheduleWork();
			HighPassFilterX_239_obj.runInitScheduleWork();
			HighPassFilterX_243_obj.runInitScheduleWork();
			Compressor_244_obj.runInitScheduleWork();
			ProcessFilter_245_obj.runInitScheduleWork();
			Expander_246_obj.runInitScheduleWork();
			Duplicate_247_obj.runInitScheduleWork();
			LowPassFilterX_248_obj.runInitScheduleWork();
			HighPassFilterX_249_obj.runInitScheduleWork();
			LowPassFilterX_258_obj.runInitScheduleWork();
			HighPassFilterX_259_obj.runInitScheduleWork();
			Expander_266_obj.runInitScheduleWork();
			Duplicate_267_obj.runInitScheduleWork();
			LowPassFilterX_268_obj.runInitScheduleWork();
			HighPassFilterX_269_obj.runInitScheduleWork();
			LowPassFilterX_272_obj.runInitScheduleWork();
			HighPassFilterX_273_obj.runInitScheduleWork();
			Compressor_274_obj.runInitScheduleWork();
			ProcessFilter_275_obj.runInitScheduleWork();
			Expander_276_obj.runInitScheduleWork();
			Duplicate_277_obj.runInitScheduleWork();
			LowPassFilterX_278_obj.runInitScheduleWork();
			HighPassFilterX_279_obj.runInitScheduleWork();
			LowPassFilterX_282_obj.runInitScheduleWork();
			HighPassFilterX_283_obj.runInitScheduleWork();
			Compressor_284_obj.runInitScheduleWork();
			ProcessFilter_285_obj.runInitScheduleWork();
			Expander_286_obj.runInitScheduleWork();
			Duplicate_287_obj.runInitScheduleWork();
			LowPassFilterX_288_obj.runInitScheduleWork();
			HighPassFilterX_289_obj.runInitScheduleWork();
			LowPassFilterX_292_obj.runInitScheduleWork();
			HighPassFilterX_293_obj.runInitScheduleWork();
			Compressor_294_obj.runInitScheduleWork();
			ProcessFilter_295_obj.runInitScheduleWork();
			Expander_296_obj.runInitScheduleWork();
			Duplicate_297_obj.runInitScheduleWork();
			LowPassFilterX_298_obj.runInitScheduleWork();
			HighPassFilterX_299_obj.runInitScheduleWork();
			LowPassFilterX_302_obj.runInitScheduleWork();
			HighPassFilterX_303_obj.runInitScheduleWork();
			Compressor_304_obj.runInitScheduleWork();
			ProcessFilter_305_obj.runInitScheduleWork();
			Expander_306_obj.runInitScheduleWork();
			Duplicate_307_obj.runInitScheduleWork();
			LowPassFilterX_308_obj.runInitScheduleWork();
			HighPassFilterX_309_obj.runInitScheduleWork();
			LowPassFilterX_312_obj.runInitScheduleWork();
			HighPassFilterX_313_obj.runInitScheduleWork();
			Compressor_314_obj.runInitScheduleWork();
			ProcessFilter_315_obj.runInitScheduleWork();
			Expander_316_obj.runInitScheduleWork();
			Duplicate_317_obj.runInitScheduleWork();
			LowPassFilterX_318_obj.runInitScheduleWork();
			HighPassFilterX_319_obj.runInitScheduleWork();
		}
	
		masterSync(4);
	}
	for(int _stageNum=5;_stageNum<2*5+MAX_ITER-1;_stageNum++)
	{
		if(stage[1])
		{
			Duplicate_217_obj.runSteadyScheduleWork();
			LowPassFilterX_218_obj.runSteadyScheduleWork();
			HighPassFilterX_219_obj.runSteadyScheduleWork();
			LowPassFilterX_228_obj.runSteadyScheduleWork();
			HighPassFilterX_229_obj.runSteadyScheduleWork();
			Duplicate_237_obj.runSteadyScheduleWork();
			LowPassFilterX_238_obj.runSteadyScheduleWork();
			HighPassFilterX_239_obj.runSteadyScheduleWork();
			HighPassFilterX_243_obj.runSteadyScheduleWork();
			Compressor_244_obj.runSteadyScheduleWork();
			ProcessFilter_245_obj.runSteadyScheduleWork();
			Expander_246_obj.runSteadyScheduleWork();
			Duplicate_247_obj.runSteadyScheduleWork();
			LowPassFilterX_248_obj.runSteadyScheduleWork();
			HighPassFilterX_249_obj.runSteadyScheduleWork();
			LowPassFilterX_258_obj.runSteadyScheduleWork();
			HighPassFilterX_259_obj.runSteadyScheduleWork();
			Expander_266_obj.runSteadyScheduleWork();
			Duplicate_267_obj.runSteadyScheduleWork();
			LowPassFilterX_268_obj.runSteadyScheduleWork();
			HighPassFilterX_269_obj.runSteadyScheduleWork();
			LowPassFilterX_272_obj.runSteadyScheduleWork();
			HighPassFilterX_273_obj.runSteadyScheduleWork();
			Compressor_274_obj.runSteadyScheduleWork();
			ProcessFilter_275_obj.runSteadyScheduleWork();
			Expander_276_obj.runSteadyScheduleWork();
			Duplicate_277_obj.runSteadyScheduleWork();
			LowPassFilterX_278_obj.runSteadyScheduleWork();
			HighPassFilterX_279_obj.runSteadyScheduleWork();
			LowPassFilterX_282_obj.runSteadyScheduleWork();
			HighPassFilterX_283_obj.runSteadyScheduleWork();
			Compressor_284_obj.runSteadyScheduleWork();
			ProcessFilter_285_obj.runSteadyScheduleWork();
			Expander_286_obj.runSteadyScheduleWork();
			Duplicate_287_obj.runSteadyScheduleWork();
			LowPassFilterX_288_obj.runSteadyScheduleWork();
			HighPassFilterX_289_obj.runSteadyScheduleWork();
			LowPassFilterX_292_obj.runSteadyScheduleWork();
			HighPassFilterX_293_obj.runSteadyScheduleWork();
			Compressor_294_obj.runSteadyScheduleWork();
			ProcessFilter_295_obj.runSteadyScheduleWork();
			Expander_296_obj.runSteadyScheduleWork();
			Duplicate_297_obj.runSteadyScheduleWork();
			LowPassFilterX_298_obj.runSteadyScheduleWork();
			HighPassFilterX_299_obj.runSteadyScheduleWork();
			LowPassFilterX_302_obj.runSteadyScheduleWork();
			HighPassFilterX_303_obj.runSteadyScheduleWork();
			Compressor_304_obj.runSteadyScheduleWork();
			ProcessFilter_305_obj.runSteadyScheduleWork();
			Expander_306_obj.runSteadyScheduleWork();
			Duplicate_307_obj.runSteadyScheduleWork();
			LowPassFilterX_308_obj.runSteadyScheduleWork();
			HighPassFilterX_309_obj.runSteadyScheduleWork();
			LowPassFilterX_312_obj.runSteadyScheduleWork();
			HighPassFilterX_313_obj.runSteadyScheduleWork();
			Compressor_314_obj.runSteadyScheduleWork();
			ProcessFilter_315_obj.runSteadyScheduleWork();
			Expander_316_obj.runSteadyScheduleWork();
			Duplicate_317_obj.runSteadyScheduleWork();
			LowPassFilterX_318_obj.runSteadyScheduleWork();
			HighPassFilterX_319_obj.runSteadyScheduleWork();
		}
		for(int index=4; index>= 1; --index)
			stage[index] = stage[index-1];
		if(_stageNum == (MAX_ITER - 1 + 5))
		{
			stage[0]=0;
		}
	
		masterSync(4);
	}
}
