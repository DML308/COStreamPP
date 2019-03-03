// 上下文无关的#define
#ifndef _DEFINES_H
#define _DEFINES_H

//===----------------------------------------------------------------------===//
// 1. Pretty-debug
//===----------------------------------------------------------------------===//
// debug info is in blue
//#define DEBUG
#ifdef DEBUG
#define debug(format, args...) fprintf(stdout, "\033[36m" format "\033[0m", ##args)
#else
#define debug(format, args...)
#endif
//line-column info is grey
#ifdef DEBUG
#define line(format, args...) fprintf(stdout, "\033[32m" format " \033[0m", ##args)
#else
#define line(format, args...)
#endif
// error info is in red
#define error(format, args...) fprintf(stderr, "\033[31m" format "\033[0m", ##args)
// warning info is in orange
#define warning(format, args...) fprintf(stderr, "\033[33m" format "\033[30;1m", ##args)

//===----------------------------------------------------------------------===//
// 2. 通用头文件引用
//===----------------------------------------------------------------------===//
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <assert.h>
using namespace std;

//===----------------------------------------------------------------------===//
// 3. 为了阻止两个.h 互相循环引用而做的 定义提升
//===----------------------------------------------------------------------===//
//为了阻止 node.h 和 token.h 的互相循环引用 ,把 YYLTYPE 定义在这里
#if !defined YYLTYPE && !defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
    int first_line;
    int first_column;
    int last_line;
    int last_column;
} YYLTYPE;
#define yyltype YYLTYPE /* obsolescent; will be withdrawn */
#define YYLTYPE_IS_DECLARED 1
#define YYLTYPE_IS_TRIVIAL 1
#endif

#endif
