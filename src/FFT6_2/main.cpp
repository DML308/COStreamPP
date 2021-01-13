#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include "setCpu.h"
#include "lock_free_barrier.h"	//包含barrier函数
#include "Global.h"
using namespace std;
int MAX_ITER=1;//默认的执行次数是1
extern void thread_0_fun();
extern void thread_1_fun();
void* thread_1_fun_start(void *)
{
	set_cpu(1);
	thread_1_fun();
	return 0;
}
int main(int argc,char **argv)
{
	void setRunIterCount(int,char**);
	setRunIterCount(argc,argv);
	set_cpu(0);
	allocBarrier(2);
	pthread_t tid[1];
	pthread_create (&tid[0], NULL, thread_1_fun_start, (void*)NULL);
	thread_0_fun();
	cout<<"the clocks of FloatSource_0:"<<clks1<<endl;
	cout<<"workTime of FloatSource_0:"<<(workTime0.tv_sec*1000+workTime0.tv_nsec/1000000)<<"ms"<<endl;
	cout<<"worktime of Float's reset tail and head:"<<(worktime0.tv_sec*1000+worktime0.tv_nsec/1000000)<<"ms"<<endl;
	cout<<"workTime of AssignmentX_1:"<<(workTime1.tv_sec*1000+workTime1.tv_nsec/1000000)<<"ms"<<endl;
	cout<<"worktime of Assign's reset tail and head:"<<(worktime1.tv_sec*1000+worktime1.tv_nsec/1000000)<<"ms"<<endl;
	cout<<"workTime of FFTReorderSimpleX_2:"<<(workTime2.tv_sec*1000+workTime2.tv_nsec/1000000)<<"ms"<<endl;
	cout<<"worktime of FFTRe's reset tail and head:"<<(worktime2.tv_sec*1000+worktime2.tv_nsec/1000000)<<"ms"<<endl;
	cout<<"workTime of CombineDFTX_3:"<<(workTime3.tv_sec*1000+workTime3.tv_nsec/1000000)<<"ms"<<endl;
	cout<<"worktime of DFT3's reset tail and head:"<<(worktime3.tv_sec*1000+worktime3.tv_nsec/1000000)<<"ms"<<endl;
	cout<<"workTime of CombineDFTX_4:"<<(workTime4.tv_sec*1000+workTime4.tv_nsec/1000000)<<"ms"<<endl;
	cout<<"worktime of DFT4's reset tail and head:"<<(worktime4.tv_sec*1000+worktime4.tv_nsec/1000000)<<"ms"<<endl;
	cout<<"workTime of CombineDFTX_5:"<<(workTime5.tv_sec*1000+workTime5.tv_nsec/1000000)<<"ms"<<endl;
	cout<<"worktime of DFT5's reset tail and head:"<<(worktime5.tv_sec*1000+worktime5.tv_nsec/1000000)<<"ms"<<endl;
	cout<<"workTime of FloatSink_6:"<<(workTime6.tv_sec*1000+workTime6.tv_nsec/1000000)<<"ms"<<endl;
	cout<<"worktime of DFT6's reset tail and head:"<<(worktime6.tv_sec*1000+worktime6.tv_nsec/1000000)<<"ms"<<endl;
	
	return 0;
}
//设置运行次数
void setRunIterCount(int argc,char **argv)
{
	int oc;
	char *b_opt_arg;
	while((oc=getopt(argc,argv,"i:"))!=-1)
	{
		switch(oc)
		{
			case 'i':
				MAX_ITER=atoi(optarg);
				break;
		}
	}
}
