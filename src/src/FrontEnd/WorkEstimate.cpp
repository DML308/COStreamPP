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
        WEST_astwalk(node);
        break;
    case Array:
        work += MEMORY_OP;
        break;
    case Block:
        WEST_astwalk(node);
        break;
    case Decl:
        if (static_cast<declareNode *>(node)->id_list.size() != 0)
        {
            for (auto id : static_cast<declareNode *>(node)->id_list)
            {
                if (id->init != NULL)
                {
                    //如果是数组声明
                    if (id->isArray == 1)
                    {
                        int cnt = 0;
                        /* 标识是否含有int a[][10];前一个参数为空的情况 */
                        bool flag = true;
                        for (auto it : id->arg_list)
                        {
                            /* 如果含有为空的情况 需要通过初始化推导 */
                            if (it == NULL)
                            {
                                flag = false;
                            }
                            if (it->type == constant)
                            {
                                cnt += (static_cast<constantNode *>(it)->llval);
                            }
                        }
                        if (flag)
                            work += cnt * MEMORY_OP;
                        else
                        {
                            int args = static_cast<initNode *>(id->init)->value.size();
                            int num = ceil(args * 1.0 / cnt);
                            cnt += num;
                            work += cnt * MEMORY_OP;
                        }
                    }
                }
                else
                    work += MEMORY_OP;
            }
        }
        break;
    case Id:
        work += MEMORY_OP;
        break;
    case Unary:
        break;
    case Binop:
        break;
    case Ternary:
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
        WEST_astwalk(node);
        break;
    case Switch:
        WEST_astwalk(node);
        work += SWITCH;
        break;
    case Case:
    case Default:
        WEST_astwalk(node);
        break;
    case Break:
        WEST_astwalk(node);
        work += BREAK;
        break;
    case Continue:
        WEST_astwalk(node);
        work += CONTINUE;
        break;
    case Call:
        WEST_astwalk(node);
        {
            string funcName = static_cast<callNode *>(node)->name;
            if (funcName == "acos")
                work += 515 / 1;
            else if (funcName == "acosh")
                work += 665 / 1;
            else if (funcName == "acosh")
                work += 665 / 1;
            else if (funcName == "asin")
                work += 536 / 1;
            else if (funcName == "asinh")
                work += 578 / 1;
            else if (funcName == "atan")
                work += 195 / 1;
            else if (funcName == "atan2")
                work += 272 / 1;
            else if (funcName == "atanh")
                work += 304 / 1;
            else if (funcName == "ceil")
                work += 47 / 1;
            else if (funcName == "cos")
                work += 120 / 1;
            else if (funcName == "cosh")
                work += 368 / 1;
            else if (funcName == "exp")
                work += 162 / 1;
            else if (funcName == "expm1")
                work += 220 / 1;
            else if (funcName == "floor")
                work += 58 / 1;
            else if (funcName == "fmod")
                work += 147 / 1;
            else if (funcName == "frexp")
                work += 60 / 1;
            else if (funcName == "log")
                work += 146 / 1;
            else if (funcName == "log10")
                work += 212 / 1;
            else if (funcName == "log1p")
                work += 233 / 1;
            else if (funcName == "modf")
                work += 41 / 1;
            else if (funcName == "pow")
                work += 554 / 1;
            else if (funcName == "sin")
                work += 97 / 1;
            else if (funcName == "sinh")
                work += 303 / 1;
            else if (funcName == "sqrt")
                work += 297 / 1;
            else if (funcName == "tan")
                work += 224 / 1;
            else if (funcName == "tanh")
                work += 288 / 1;
            else if (funcName == "round")
                work += (58 + FLOAT_ARITH_OP) / 1;
            else if (funcName == "abs")
                work += 60 / 1;
            else if (funcName == "max")
                work += 60 / 1;
            else if (funcName == "min")
                work += 60 / 1;
            else if (funcName == "frta")
                work += FRTA_OP / 1;
            else if (funcName == "println")
                work += PRINTLN_OP / 1;
            /* 对于外部函数工作量估计为60 */
            else
            {
                work += UNKNOWN_METHOD_CALL / 1;
            }
            work += METHOD_CALL_OVERHEAD / 1;
        }
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

void WEST_astwalk(Node *node)
{
    switch (node->type)
    {
    case constant:
        break;
    case Id:
        break;
    case Ternary:
        workCompute(static_cast<ternaryNode *>(node)->first);
        workCompute(static_cast<ternaryNode *>(node)->second);
        workCompute(static_cast<ternaryNode *>(node)->third);
        break;
    case Binop:
        workCompute(static_cast<binopNode *>(node)->left);
        workCompute(static_cast<binopNode *>(node)->right);
        break;
    case Unary:
        workCompute(static_cast<unaryNode *>(node)->exp);
        break;
    case Cast:
        break;
    case Array:
        break;
    case Call:
        break;
    case Initializer:
        break;
    case ImplicitCast:
        break;
    case Label:
        break;
    case Switch:
        break;
    case Case:
        break;
    case Default:
        break;
    case If:
        break;
    case IfElse:
        break;
    case While:
        break;
    case Do:
        break;
    case For:
        break;
    case Continue:
        break;
    case Break:
        break;
    case Return:
        break;
    case Block:
        if (static_cast<blockNode *>(node)->stmt_list != NULL)
        {
            for (auto it : *static_cast<blockNode *>(node)->stmt_list)
            {
                workCompute(it);
            }
        }
        break;
    case primary:
        break;
    case Decl:
        break;
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
    default:
        break;
    }
}