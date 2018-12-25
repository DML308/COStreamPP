#ifndef _WORK_ESTIMATE_H
#define _WORK_ESTIMATE_H
#include "node.h"
#define INIT 0
#define STEADY 1
const int updateBuffer = 20;
int work;
int state; /*标识init和steady*/
int workEstimate_init(operBodyNode *body, int w_init);
int workEstimate_steady(operBodyNode *body, int w_steady);
void workCompute(Node *node);
#endif