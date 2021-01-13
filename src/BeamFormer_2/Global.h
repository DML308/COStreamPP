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
extern Buffer<streamData> FloatSource_0_Duplicate_1;
extern Buffer<streamData> Duplicate_1_InputGenerate_2;
extern Buffer<streamData> Duplicate_1_InputGenerate_5;
extern Buffer<streamData> Duplicate_1_InputGenerate_8;
extern Buffer<streamData> Duplicate_1_InputGenerate_11;
extern Buffer<streamData> Duplicate_1_InputGenerate_14;
extern Buffer<streamData> Duplicate_1_InputGenerate_17;
extern Buffer<streamData> Duplicate_1_InputGenerate_20;
extern Buffer<streamData> Duplicate_1_InputGenerate_23;
extern Buffer<streamData> Duplicate_1_InputGenerate_26;
extern Buffer<streamData> Duplicate_1_InputGenerate_29;
extern Buffer<streamData> Duplicate_1_InputGenerate_32;
extern Buffer<streamData> Duplicate_1_InputGenerate_35;
extern Buffer<streamData> InputGenerate_2_BeamFirFilter_3;
extern Buffer<streamData> BeamFirFilter_3_BeamFirFilter_4;
extern Buffer<streamData> BeamFirFilter_4_join_38;
extern Buffer<streamData> InputGenerate_5_BeamFirFilter_6;
extern Buffer<streamData> BeamFirFilter_6_BeamFirFilter_7;
extern Buffer<streamData> BeamFirFilter_7_join_38;
extern Buffer<streamData> InputGenerate_8_BeamFirFilter_9;
extern Buffer<streamData> BeamFirFilter_9_BeamFirFilter_10;
extern Buffer<streamData> BeamFirFilter_10_join_38;
extern Buffer<streamData> InputGenerate_11_BeamFirFilter_12;
extern Buffer<streamData> BeamFirFilter_12_BeamFirFilter_13;
extern Buffer<streamData> BeamFirFilter_13_join_38;
extern Buffer<streamData> InputGenerate_14_BeamFirFilter_15;
extern Buffer<streamData> BeamFirFilter_15_BeamFirFilter_16;
extern Buffer<streamData> BeamFirFilter_16_join_38;
extern Buffer<streamData> InputGenerate_17_BeamFirFilter_18;
extern Buffer<streamData> BeamFirFilter_18_BeamFirFilter_19;
extern Buffer<streamData> BeamFirFilter_19_join_38;
extern Buffer<streamData> InputGenerate_20_BeamFirFilter_21;
extern Buffer<streamData> BeamFirFilter_21_BeamFirFilter_22;
extern Buffer<streamData> BeamFirFilter_22_join_38;
extern Buffer<streamData> InputGenerate_23_BeamFirFilter_24;
extern Buffer<streamData> BeamFirFilter_24_BeamFirFilter_25;
extern Buffer<streamData> BeamFirFilter_25_join_38;
extern Buffer<streamData> InputGenerate_26_BeamFirFilter_27;
extern Buffer<streamData> BeamFirFilter_27_BeamFirFilter_28;
extern Buffer<streamData> BeamFirFilter_28_join_38;
extern Buffer<streamData> InputGenerate_29_BeamFirFilter_30;
extern Buffer<streamData> BeamFirFilter_30_BeamFirFilter_31;
extern Buffer<streamData> BeamFirFilter_31_join_38;
extern Buffer<streamData> InputGenerate_32_BeamFirFilter_33;
extern Buffer<streamData> BeamFirFilter_33_BeamFirFilter_34;
extern Buffer<streamData> BeamFirFilter_34_join_38;
extern Buffer<streamData> InputGenerate_35_BeamFirFilter_36;
extern Buffer<streamData> BeamFirFilter_36_BeamFirFilter_37;
extern Buffer<streamData> BeamFirFilter_37_join_38;
extern Buffer<streamData> join_38_Duplicate_39;
extern Buffer<streamData> Duplicate_39_BeamForm_40;
extern Buffer<streamData> Duplicate_39_BeamForm_44;
extern Buffer<streamData> Duplicate_39_BeamForm_48;
extern Buffer<streamData> Duplicate_39_BeamForm_52;
extern Buffer<streamData> BeamForm_40_BeamFirFilter_41;
extern Buffer<streamData> BeamFirFilter_41_Magnitude_42;
extern Buffer<streamData> Magnitude_42_Detector_43;
extern Buffer<streamData> Detector_43_join_56;
extern Buffer<streamData> BeamForm_44_BeamFirFilter_45;
extern Buffer<streamData> BeamFirFilter_45_Magnitude_46;
extern Buffer<streamData> Magnitude_46_Detector_47;
extern Buffer<streamData> Detector_47_join_56;
extern Buffer<streamData> BeamForm_48_BeamFirFilter_49;
extern Buffer<streamData> BeamFirFilter_49_Magnitude_50;
extern Buffer<streamData> Magnitude_50_Detector_51;
extern Buffer<streamData> Detector_51_join_56;
extern Buffer<streamData> BeamForm_52_BeamFirFilter_53;
extern Buffer<streamData> BeamFirFilter_53_Magnitude_54;
extern Buffer<streamData> Magnitude_54_Detector_55;
extern Buffer<streamData> Detector_55_join_56;
extern Buffer<streamData> join_56_FloatSink_57;
extern timespec worktime1;
extern timespec worktime2;
#endif
