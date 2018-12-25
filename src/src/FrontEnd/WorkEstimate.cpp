#include "staticStreamGragh.h"
#include "workEstimate.h"
const int updateBuffer=20;
void WorkEstimate(StaticStreamGraph *ssg)
{
    int len = ssg->flatNodes.size();
    for (int i = 0; i < len; ++i)
    {
        int w_steady = 0, w_init = 0;
        FlatNode *flatNode = (ssg->flatNodes)[i];
        /* 检查每一个operatorNode的body（包括init，work和window)*/
        operBodyNode *body = flatNode->contents->operBody;
        if (body != NULL)
        {
            w_init = workEstimate_init(body);
            w_steady = workEstimate_steady(body);
        }
        w_steady += (flatNode->outFlatNodes.size() + flatNode->inFlatNodes.size()) * updateBuffer; //多核下调整缓冲区head和tail
        ssg->AddInitWork(flatNode, w_init);
        ssg->AddSteadyWork(flatNode, w_steady);
    }
}

int workEstimate_init(operBodyNode *body)
{
    int totalWork = 0;
    return totalWork;
}

int workEstimate_steady(operBodyNode *body)
{
    int totalWork = 0;
    return totalWork;
}
