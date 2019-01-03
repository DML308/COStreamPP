#ifndef _X86CODEGENERATE_H_
#define _X86CODEGENERATE_H_
#include "schedulerSSG.h"
#include "ActorStageAssignment.h"
#include "Partition.h"

class X86CodeGenerate
{
  public:
    /******************构造函数*******************/
    X86CodeGenerate(int, SchedulerSSG *, const char *, StageAssignment *, Partition *);
    /********************************************/
};

#endif