#ifndef _GLOBAL_H
#define _GLOBAL_H
#include "Buffer.h"
#include <math.h>
#include <string>
using namespace std;
struct streamData{
	int  x;
};
extern Buffer<streamData> Source_0_Roundrobin_1;
extern Buffer<streamData> Roundrobin_1_ZigZagUnordering_2;
extern Buffer<streamData> Roundrobin_1_MotionVectorDecode_34;
extern Buffer<streamData> Roundrobin_1_Repeat_36;
extern Buffer<streamData> ZigZagUnordering_2_Duplicate_3;
extern Buffer<streamData> Duplicate_3_Roundrobin_4;
extern Buffer<streamData> Duplicate_3_InverseQuantization_AC_Coeff_8;
extern Buffer<streamData> Roundrobin_4_InverseQuantization_DC_Intra_Coeff_5;
extern Buffer<streamData> Roundrobin_4_InverseQuantization_AC_Coeff_6;
extern Buffer<streamData> InverseQuantization_DC_Intra_Coeff_5_join_7;
extern Buffer<streamData> InverseQuantization_AC_Coeff_6_join_7;
extern Buffer<streamData> join_7_join_9;
extern Buffer<streamData> InverseQuantization_AC_Coeff_8_join_9;
extern Buffer<streamData> join_9_InverseQuantizationJoinerSubstitute_10;
extern Buffer<streamData> InverseQuantizationJoinerSubstitute_10_Saturation_11;
extern Buffer<streamData> Saturation_11_MismatchControl_12;
extern Buffer<streamData> MismatchControl_12_Roundrobin_13;
extern Buffer<streamData> Roundrobin_13_iDCT8x8_1D_row_fast_14;
extern Buffer<streamData> Roundrobin_13_iDCT8x8_1D_row_fast_15;
extern Buffer<streamData> Roundrobin_13_iDCT8x8_1D_row_fast_16;
extern Buffer<streamData> Roundrobin_13_iDCT8x8_1D_row_fast_17;
extern Buffer<streamData> Roundrobin_13_iDCT8x8_1D_row_fast_18;
extern Buffer<streamData> Roundrobin_13_iDCT8x8_1D_row_fast_19;
extern Buffer<streamData> Roundrobin_13_iDCT8x8_1D_row_fast_20;
extern Buffer<streamData> Roundrobin_13_iDCT8x8_1D_row_fast_21;
extern Buffer<streamData> iDCT8x8_1D_row_fast_14_join_22;
extern Buffer<streamData> iDCT8x8_1D_row_fast_15_join_22;
extern Buffer<streamData> iDCT8x8_1D_row_fast_16_join_22;
extern Buffer<streamData> iDCT8x8_1D_row_fast_17_join_22;
extern Buffer<streamData> iDCT8x8_1D_row_fast_18_join_22;
extern Buffer<streamData> iDCT8x8_1D_row_fast_19_join_22;
extern Buffer<streamData> iDCT8x8_1D_row_fast_20_join_22;
extern Buffer<streamData> iDCT8x8_1D_row_fast_21_join_22;
extern Buffer<streamData> join_22_Roundrobin_23;
extern Buffer<streamData> Roundrobin_23_iDCT8x8_1D_col_fast_fine_24;
extern Buffer<streamData> Roundrobin_23_iDCT8x8_1D_col_fast_fine_25;
extern Buffer<streamData> Roundrobin_23_iDCT8x8_1D_col_fast_fine_26;
extern Buffer<streamData> Roundrobin_23_iDCT8x8_1D_col_fast_fine_27;
extern Buffer<streamData> Roundrobin_23_iDCT8x8_1D_col_fast_fine_28;
extern Buffer<streamData> Roundrobin_23_iDCT8x8_1D_col_fast_fine_29;
extern Buffer<streamData> Roundrobin_23_iDCT8x8_1D_col_fast_fine_30;
extern Buffer<streamData> Roundrobin_23_iDCT8x8_1D_col_fast_fine_31;
extern Buffer<streamData> iDCT8x8_1D_col_fast_fine_24_join_32;
extern Buffer<streamData> iDCT8x8_1D_col_fast_fine_25_join_32;
extern Buffer<streamData> iDCT8x8_1D_col_fast_fine_26_join_32;
extern Buffer<streamData> iDCT8x8_1D_col_fast_fine_27_join_32;
extern Buffer<streamData> iDCT8x8_1D_col_fast_fine_28_join_32;
extern Buffer<streamData> iDCT8x8_1D_col_fast_fine_29_join_32;
extern Buffer<streamData> iDCT8x8_1D_col_fast_fine_30_join_32;
extern Buffer<streamData> iDCT8x8_1D_col_fast_fine_31_join_32;
extern Buffer<streamData> join_32_BoundedSaturation_33;
extern Buffer<streamData> BoundedSaturation_33_join_37;
extern Buffer<streamData> MotionVectorDecode_34_Repeat_35;
extern Buffer<streamData> Repeat_35_join_37;
extern Buffer<streamData> Repeat_36_join_37;
extern Buffer<streamData> join_37_Sink_38;
#endif
