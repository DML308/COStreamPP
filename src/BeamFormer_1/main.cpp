#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include "setCpu.h"
#include "lock_free_barrier.h"	//包含barrier函数
#include "Global.h"
using namespace std;
int MAX_ITER=1;//默认的执行次数是1
extern void thread_0_fun();
int main(int argc,char **argv)
{
	void setRunIterCount(int,char**);
	setRunIterCount(argc,argv);
	set_cpu(0);
	allocBarrier(1);
	pthread_t tid[0];
	thread_0_fun();
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
