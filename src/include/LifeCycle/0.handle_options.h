#ifndef _HANDLE_OPTIONS_H
#define _HANDLE_OPTIONS_H

#include <stdio.h>
#include <getopt.h>
#include "defines.h"
#include "global.h"

/*帮助处理命令行的 args 参考 http://blog.zhangjikai.com/2016/03/05/%E3%80%90C%E3%80%91%E8%A7%A3%E6%9E%90%E5%91%BD%E4%BB%A4%E8%A1%8C%E5%8F%82%E6%95%B0--getopt%E5%92%8Cgetopt_long/
*/

int handle_options(int argc, char *argv[]);
FILE * get_preprocessed_input();
void print_version_info();
void print_usage();
#endif
