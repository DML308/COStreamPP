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
	C_3 C_3_obj(C_3_D_4,B_2_C_3);
	char stage[3]={0};
	stage[0]=1;
	for(int _stageNum=0;_stageNum<3;_stageNum++)
	{
		if(1==_stageNum)
		{
			C_3_obj.runInitScheduleWork();
		}
	
		masterSync(2);
	}
	for(int _stageNum=3;_stageNum<2*3+MAX_ITER-1;_stageNum++)
	{
		if(stage[1])
		{
			C_3_obj.runSteadyScheduleWork();
		}
		for(int index=2; index>= 1; --index)
			stage[index] = stage[index-1];
		if(_stageNum == (MAX_ITER - 1 + 3))
		{
			stage[0]=0;
		}
	
		masterSync(2);
	}
}
