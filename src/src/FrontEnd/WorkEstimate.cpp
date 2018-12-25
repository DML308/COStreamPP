#include "staticStreamGragh.h"
#include "workEstimate.h"

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
            w_init = workEstimate_init(body, w_init);
            w_steady = workEstimate_steady(body, w_steady);
        }
        w_steady += (flatNode->outFlatNodes.size() + flatNode->inFlatNodes.size()) * updateBuffer; //多核下调整缓冲区head和tail
        ssg->AddInitWork(flatNode, w_init);
        ssg->AddSteadyWork(flatNode, w_steady);
    }
}

/* 初始化初态调度工作量 */
int workEstimate_init(operBodyNode *body, int w_init)
{
    state = INIT;
    work = w_init;
    workCompute(body);
    return work;
}
/* 初始化稳态调度工作量 */
int workEstimate_steady(operBodyNode *body, int w_steady)
{
    state = STEADY;
    work = w_steady;
    workCompute(body);
    return work;
}

void workCompute(Node *node)
{
    switch (node->type)
    {
    case OperBody: 
        /* 初态调度只计算init部分工作量 */
        if (state == INIT)
        {
            if (static_cast<operBodyNode *>(node)->init != NULL)
            {
                workCompute(static_cast<operBodyNode *>(node)->init);
            }
        }
        /* 稳态调度需要计算statment_list和work函数的工作量 */
        else if (state == STEADY)
        {
            if (static_cast<operBodyNode *>(node)->stmt_list != NULL)
            {
                for (auto it : *static_cast<operBodyNode *>(node)->stmt_list)
                {
                    workCompute(it);
                }
            }
            if (static_cast<operBodyNode *>(node)->work != NULL)
            {
                workCompute(static_cast<operBodyNode *>(node)->work);
            }
        }
        break;
    case Comma:
        break;
    case Array:
        break;
    case Ternary:
        break;
    case Block:
        break;
    case FuncDcl:
        break;
    case Decl:
        break;
    case Id:
        break;
    case Unary:
        break;
    case Binop:
        break;
    case For:
        break;
    case If:
        break;
    case IfElse:
        break;
    case Do:
        break;
    case While:
        break;
    case Cast:
        break;
    case Switch:
        break;
    case Case:
        break;
    case Default:
        break;
    case Break:
        break;
    case Continue:
        break;
    case Call:
        break;
    case constant:
        break;
    case Initializer:
        break;
    case ImplicitCast:
        break;
    default:
        break;
    }
}
