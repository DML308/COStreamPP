#ifndef _GLOBAL_HH_
#define _GLOBAL_HH_
#include <stdio.h>
#include "defines.h"
#define INT_MIN -2147483648
#define MAX_INF 2147483647
#define MAX_SCOPE_DEPTH 100 //定义最大嵌套深度为100
extern string infile_name;
extern string output_path;
extern string origin_path;
extern string temp_name;
extern FILE *infp;
extern FILE *outfp;

extern float VersionNumber;
extern const char *const CompiledDate;
extern int Level;          //symbol.h
extern int current_version[MAX_SCOPE_DEPTH];
extern int WarningLevel;
//全局函数

//报错+输出源文件的一行
void Error(string msg, int line, int column = 0);
void Warning(string msg, int line, int column = 0);

#endif
