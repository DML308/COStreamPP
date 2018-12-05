#include "symbol.h"
/* enterScope */
void EnterScope()
{
    Level++;
    if (Level == MAX_SCOPE_DEPTH) {
	    cout<<"Internal Error: out of nesting levels!\n";
	    exit(-1);
    }
    current_version[Level]++;
}
