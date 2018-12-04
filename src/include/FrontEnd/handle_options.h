#ifndef _HANDLE_OPTIONS_H
#define _HANDLE_OPTIONS_H

#include <stdio.h>
#include <getopt.h> //帮助处理命令行的 args 参考 https://blog.csdn.net/fjt19900921/article/details/8081015

int handle_options(int argc, char *argv[]);
FILE * get_preprocessed_input();
void print_version_info();
#endif
