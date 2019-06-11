#ifndef _CODEGENERATION_H
#define _CODEGENERATION_H
#include "ActorStageAssignment.h"
#include "Partition.h"
#include "GPULBPartition.h"

void CodeGeneration(int cpuCoreNum,SchedulerSSG *sssg, string filePath, StageAssignment *psa, Partition *mp);
void GPUCodeGeneration(int cpuCoreNum,SchedulerSSG *sssg, string filePath, StageAssignment *psa, Partition *mp);
#endif