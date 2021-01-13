#ifndef _LOCK_FREE_BARRIER_H_
#define _LOCK_FREE_BARRIER_H_
extern volatile char *barrierBuffer;
void masterSync(int n, bool yasuo = false, bool updown = false, int core = 0);
void workerSync(const int);
void allocBarrier(int n);
#endif