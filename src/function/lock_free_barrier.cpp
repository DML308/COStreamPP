#include <stdlib.h>
#include <string.h>
volatile char *barrierBuffer;
void masterSync(int n, bool yasuo, bool updown, int core)
{
	if (yasuo)
	{
		if (updown)
		{
			//上游
			int i, sum;
			do
			{
				for (i = 1, sum = 1; i < core; i++)
					sum += barrierBuffer[i];
			} while (sum < core);
			for (i = 1; i < core; i++)
			{
				barrierBuffer[i] = 0;
			}
		}
		else
		{
			//下游
			int i, sum;
			do
			{
				for (i = (9 - core), sum = 1; i < n; i++)
					sum += barrierBuffer[i];
			} while (sum < core);
			for (i = (9 - core); i < n; i++)
			{
				barrierBuffer[i] = 0;
			}
		}
	}
	else
	{
		int i, sum;
		do
		{
			for (i = 1, sum = 1; i < n; i++)
				sum += barrierBuffer[i];
		} while (sum < n);
		for (i = 1; i < n; i++)
		{
			barrierBuffer[i] = 0;
		}
	}
}
void workerSync(const int tid)
{
	barrierBuffer[tid] = 1;
	while (barrierBuffer[tid])
		; //wait for the masterSync to set it to 0
}
void allocBarrier(int n)
{
	barrierBuffer = (volatile char *)malloc(n * sizeof(volatile char));
	memset((void *)barrierBuffer, 0, n);
}