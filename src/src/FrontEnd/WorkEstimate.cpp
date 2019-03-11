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
    if (node == NULL)
        return;
    int tmp = 0;
    int newWork = 0;
    int oldWork = 0;
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
    {
        declareNode *decl = static_cast<declareNode *>(node);
        if (decl->id_list.size() != 0)
        {
            for (auto id : decl->id_list)
            {
                assert(id != NULL);
                if (id->init != NULL && id->isArray == 1)
                {
                    //如果是数组声明
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
                        else if (it->type == constant)
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
                else
                    work += MEMORY_OP;
            }
        }
        break;
    }
    case Id:
        work += MEMORY_OP;
        break;
    case Unary:
        WEST_astwalk(node);
        {
            expNode *exp = static_cast<unaryNode *>(node)->exp;
            if (exp->type == constant)
            {
                if (((constantNode *)exp)->style == "double")
                {
                    tmp = FLOAT_ARITH_OP;
                }
                else if (((constantNode *)exp)->style == "integer")
                {
                    tmp = INT_ARITH_OP;
                }
                work += tmp;
            }
            else
                work += INT_ARITH_OP;
        }
        break;

    case Binop:
        if (static_cast<binopNode *>(node)->op != "=" && static_cast<binopNode *>(node)->op != ".")
        {
            {
                expNode *left = static_cast<binopNode *>(node)->left;
                expNode *right = static_cast<binopNode *>(node)->right;
                /*暂时缺乏对左操作树的工作量估计*/
                if (right->type == constant)
                {
                    if (((constantNode *)right)->style == "double")
                    {
                        tmp = FLOAT_ARITH_OP;
                        if (static_cast<binopNode *>(node)->op == "/") //仅当浮点的除法需要x16
                            tmp *= 16;
                    }
                    else if (((constantNode *)right)->style == "integer")
                    {
                        tmp = INT_ARITH_OP;
                    }
                }
                else
                    tmp = FLOAT_ARITH_OP;
            }
        }
        else if (static_cast<binopNode *>(node)->op == "=")
        {
            tmp = 0;
        }
        else if (static_cast<binopNode *>(node)->op == ".")
        {
            workCompute(static_cast<binopNode *>(node)->left); //如果是点操作，则仅取左边表达式计算
            break;
        }
        work += tmp;
        WEST_astwalk(node);
        break;
    case Ternary:
        WEST_astwalk(node);
        break;
    case For:
        workCompute(static_cast<forNode *>(node)->init);
        oldWork = work;
        {
            int condition = MAX_INF, init = MAX_INF, step = 1;
            Node *init_exp = static_cast<forNode *>(node)->init;
            expNode *cond_exp = static_cast<forNode *>(node)->cond;
            expNode *next_exp = static_cast<forNode *>(node)->next;
            tmp = LOOP_COUNT;
            /* for的init部分 */
            if (init_exp->type == Decl)
            {
                list<idNode *> id_list = static_cast<declareNode *>(init_exp)->id_list;
                idNode *first = id_list.front();
                Node *f_init = first->init;
                if (f_init->type == Initializer)
                {
                    list<Node *> value = static_cast<initNode *>(f_init)->value;
                    Node *val = value.front();
                    if (val->type == constant)
                    {
                        init = static_cast<constantNode *>(val)->llval;
                    }
                }
            }
            else if (init_exp->type = Binop)
            {
                expNode *init_r = ((binopNode *)(init_exp))->right;
                if (init_r->type == constant)
                    init = ((constantNode *)init_r)->llval;
            }
            /* for的cond部分 */
            if (cond_exp->type == Binop)
            {
                expNode *left = ((binopNode *)(cond_exp))->left;
                expNode *right = ((binopNode *)(cond_exp))->right;
                if (left->type == constant || right->type == constant)
                {
                    if (left->type == constant)
                    {
                        condition = ((constantNode *)(left))->llval;
                    }
                    if (right->type == constant)
                    {
                        condition = ((constantNode *)(right))->llval;
                    }
                }
                if (init != MAX_INF && condition != MAX_INF)
                {
                    tmp = abs(condition - init);
                }
                if (((binopNode *)cond_exp)->op == "<" || ((binopNode *)cond_exp)->op == ">")
                    tmp += 0;
                else
                    tmp += 1;
            }
            /* for的next部分 */
            if (next_exp->type == Binop)
            {
                string op = ((binopNode *)next_exp)->op;
                if (op == "*=")
                {
                    if (condition >= init)
                        tmp = condition / init;
                    else
                        tmp = init / condition;
                    tmp = log(tmp) / log(step);
                }
                else if (op == "/=")
                {
                    if (condition >= init)
                        tmp = condition / init;
                    else
                        tmp = init / condition;
                    tmp = log(tmp) / log(step);
                }
                else
                    tmp = (tmp + step - 1) / step;
            }
            if (init == MAX_INF || condition == MAX_INF)
                tmp = LOOP_COUNT;
        }
        WEST_astwalk(node);
        newWork = work;
        work = oldWork + tmp * (newWork - oldWork);
        break;
    case If:
    case IfElse:
        oldWork = work;
        WEST_astwalk(node);
        newWork = work;
        work += (newWork - oldWork) / 2;
        work += IF;
        break;
    case Do:
    case While:
        oldWork = work;
        WEST_astwalk(node);
        newWork = work;
        work = oldWork + LOOP_COUNT * (newWork - oldWork);
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
        workCompute(static_cast<caseNode *>(node)->exp);
        break;
    case Array:
        workCompute(node);
        break;
    case Call:
        if (static_cast<callNode *>(node)->arg_list.size() != 0)
        {
            for (auto it : static_cast<callNode *>(node)->arg_list)
                workCompute(it);
        }
        break;
    case Initializer:
        if (static_cast<initNode *>(node)->value.size() != 0)
        {
            for (auto it : static_cast<initNode *>(node)->value)
                workCompute(it);
        }
        break;

    case Label:
        break;
    case Switch:
        workCompute(static_cast<switchNode *>(node)->exp);
        workCompute(static_cast<switchNode *>(node)->stat);
        break;
    case Case:
        workCompute(static_cast<caseNode *>(node)->exp);
        workCompute(static_cast<caseNode *>(node)->stmt);
        break;
    case Default:
        workCompute(static_cast<defaultNode *>(node)->stmt);
        break;
    case If:
        workCompute(static_cast<ifNode *>(node)->exp);
        workCompute(static_cast<ifNode *>(node)->stmt);
        break;
    case IfElse:
        workCompute(static_cast<ifElseNode *>(node)->exp);
        workCompute(static_cast<ifElseNode *>(node)->stmt1);
        workCompute(static_cast<ifElseNode *>(node)->stmt2);
        break;
    case While:
        workCompute(static_cast<whileNode *>(node)->exp);
        workCompute(static_cast<whileNode *>(node)->stmt);
        break;
    case Do:
        workCompute(static_cast<doNode *>(node)->exp);
        workCompute(static_cast<doNode *>(node)->stmt);
        break;
    case For:
        workCompute(static_cast<forNode *>(node)->init);
        workCompute(static_cast<forNode *>(node)->cond);
        workCompute(static_cast<forNode *>(node)->next);
        workCompute(static_cast<forNode *>(node)->stmt);
        break;
    case Continue:
        break;
    case Break:
        break;
    case Return:
        workCompute(static_cast<returnNode *>(node)->exp);
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
    {
        operBodyNode *operbody = static_cast<operBodyNode *>(node);
        /* 初态调度只计算init部分工作量 */
        if (state == INIT)
        {
            workCompute(operbody->init);
        }
        /* 稳态调度需要计算statment_list和work函数的工作量 */
        else if (state == STEADY)
        {
            for (auto it : operbody->stmt_list)
            {
                workCompute(it);
            }
            workCompute(operbody->work);
        }
        break;
    }
    default:
        break;
    }
}