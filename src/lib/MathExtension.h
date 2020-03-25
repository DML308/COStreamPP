#ifndef MATH_EXTENSION_H_
#define MATH_EXTENSION_H_
#include <math.h>
#include <stdlib.h>
// 期望为E，方差为V
inline double gauss_random(double E = 0, double V = 1)
{
    static double V1, V2, S;
    static int phase = 0;
    double X;
    if ( phase == 0 ) {
        do {
            double U1 = (double)rand() / RAND_MAX;
            double U2 = (double)rand() / RAND_MAX;
             
            V1 = 2 * U1 - 1;
            V2 = 2 * U2 - 1;
            S = V1 * V1 + V2 * V2;
        } while(S >= 1 || S == 0);
         
        X = V1 * sqrt(-2 * log(S) / S);
    } else
        X = V2 * sqrt(-2 * log(S) / S);
         
    phase = 1 - phase;
 
    return X * V + E;
}
inline double uniform_rand(double a = -1, double b = 1) {
	if (a < b)
		return a + ((double)rand() / RAND_MAX) * (b - a);
	else
		return b + ((double)rand() / RAND_MAX) * (a - b);
}
#endif