#include "barrier_sync.h"
extern int ThreadNum;
int count = ThreadNum;
bool sense = false;
void await()
{
	bool mySense = !sense;
	int position = __sync_fetch_and_sub(&count,1);
	if(position != 1)
	{
		while(sense != mySense);
	}
	else
	{
		count = ThreadNum;
		sense = mySense;
	}
}


