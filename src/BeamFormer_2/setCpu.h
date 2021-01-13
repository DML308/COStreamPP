#ifndef _SET_CPU_H_
#define _SET_CPU_H_
#include <sched.h>
#include <sys/types.h>
#include <iostream>
#include <unistd.h> //getpid()
#include <sys/syscall.h>
using namespace std;
#define gettid() syscall(__NR_gettid)
int set_cpu(int i);
#endif