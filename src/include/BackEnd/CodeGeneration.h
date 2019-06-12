#ifndef _CODEGENERATION_H
#define _CODEGENERATION_H
#include "ActorStageAssignment.h"
#include "Partition.h"
#include "GPULBPartition.h"

void CodeGeneration(int cpuCoreNum,SchedulerSSG *sssg, string filePath, StageAssignment *psa, Partition *mp);
void GPUCodeGeneration(int CpuCoreNum, SchedulerSSG *sssg, string path, StageAssignment *psa, GPULBPartition *hlbp);
#endif