#ifndef _GLOBAL_H
#define _GLOBAL_H
#include "Buffer.h"
#include <math.h>
#include <string>
#include <time.h>
using namespace std;
struct streamData{
	double  x;
};
extern Buffer<streamData> FloatSource_0_AssignmentX_1;
extern Buffer<streamData> AssignmentX_1_FFTReorderSimpleX_2;
extern Buffer<streamData> FFTReorderSimpleX_2_CombineDFTX_3;
extern Buffer<streamData> CombineDFTX_3_CombineDFTX_4;
extern Buffer<streamData> CombineDFTX_4_CombineDFTX_5;
extern Buffer<streamData> CombineDFTX_5_FloatSink_6;
extern timespec workTime0;
extern timespec workTime1;
extern timespec workTime2;
extern timespec workTime3;
extern timespec workTime4;
extern timespec workTime5;
extern timespec workTime6;
extern timespec worktime0;
extern timespec worktime1;
extern timespec worktime2;
extern timespec worktime3;
extern timespec worktime4;
extern timespec worktime5;
extern timespec worktime6;
extern int clks1;
#endif
