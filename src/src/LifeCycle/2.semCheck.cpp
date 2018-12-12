#include "2.semCheck.h"
void SemCheck::findMainComposite(list<Node *> *program)
{
    int numOfMain = 0;
    assert(Program != NULL);
    for (auto it : *(program))
    {
        if (it->type == Composite && ((compositeNode *)it)->compName == "Main")
        {
            ++numOfMain;
            gMainComposite = (compositeNode *)it;
        }
    }
    if (numOfMain > 1)
    {
        error("Fatal error: the program cannot have more than one composite entrance:\n");
        exit(-1);
    }
    if (numOfMain == 0)
    {
        error("Fatal error: the program dose not have Main composite entrance:\n");
        exit(-1);
    }
}