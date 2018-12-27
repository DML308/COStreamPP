#ifndef _SPEED_UP_H
#define _SPEED_UP_H
#include "Partition.h"
#include <limits>
#include <time.h>
#include <iomanip>
#include <algorithm>
string do_fraction(double v, int decplaces = 3);
void ComputeSpeedup(SchedulerSSG *sssg, Partition *mp, string sourceFileName, const char *fileName, string pSelected);
#endif