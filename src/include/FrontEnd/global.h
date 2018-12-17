#ifndef _GLOBAL_HH_
#define _GLOBAL_HH_
#include <stdio.h>
#include "defines.h"

#define MAX_SCOPE_DEPTH 100 //定义最大嵌套深度为100
extern char *infile_name;
extern char *outfile_name;
extern const char *temp_name;
extern FILE *infp;
extern FILE *outfp;

extern float VersionNumber;
extern const char *const CompiledDate;
extern int Level;          //symbol.h
extern int current_version[MAX_SCOPE_DEPTH];
extern int WarningLevel;
//全局函数

//报错+输出源文件的一行
void Error(const char * msg, int line, int column = 0);
void Warning(const char * msg, int line, int column = 0);

#endif
