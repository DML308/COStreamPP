#ifndef _WORK_ESTIMATE_H
#define _WORK_ESTIMATE_H
#include "node.h"
#define INIT 0
#define STEADY 1
/*定义不同类型node工作量大小*/
#define PRINT  3
#define PEEK  3
#define POP  3
#define PUSH  3
#define INT_ARITH_OP  1
#define FLOAT_ARITH_OP  2
#define LOOP_COUNT  5
#define SWITCH  1
#define IF  1  
#define CONTINUE  1
#define BREAK  1
#define MEMORY_OP  2
#define METHOD_CALL_OVERHEAD  10
#define UNKNOWN_METHOD_CALL  60
#define STREAM_OP 20
#define PRINTLN_OP 4000 
#define FRTA_OP 60 
#define CAST 2


  


const int updateBuffer = 20;
int work=0;
int state; /*标识init和steady*/
int workEstimate_init(operBodyNode *body, int w_init);
int workEstimate_steady(operBodyNode *body, int w_steady);
void workCompute(Node *node);
void WEST_astwalk(Node *node);
void WorkFunc_withoutarg(funcBodyNode *func_body);
void WorkFunc_witharg(funcBodyNode *func_body);
#endif