#ifndef _GLOBAL_H
#define _GLOBAL_H
#include "Buffer.h"
#include <math.h>
#include <string>
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
#endif
