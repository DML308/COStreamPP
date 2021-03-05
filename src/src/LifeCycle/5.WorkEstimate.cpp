#include "staticStreamGragh.h"
#include "5.workEstimate.h"
#include "symboltableGenerate.h"
#include "symbol.h"
extern list<Node *> *Program;

extern SymbolTable *top;

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
    /*case funcBody:
        WEST_astwalk(node);
        break;
        */
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
        /*{
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
       
        break;
        }*/
       {
           unaryNode * unary=static_cast<unaryNode *>(node);
           if(unary->op=="~"||unary->op=="!")
           {
               tmp=1;
           }
           if(unary->op=="++"||unary->op=="--")
           {
               tmp=2;
           }
           work+=tmp;
           break;
       }
    case Binop:
    {
        binopNode * binop = static_cast<binopNode *>(node);
        //关系运算
        if(binop->op=="<"||binop->op==">"||binop->op==">="||binop->op=="<="||binop->op=="=="||binop->op=="!=")
        {
            tmp=1;
        }
        //逻辑运算
        if(binop->op=="&&"||binop->op=="||")
        {
            tmp=2;
        }
        //位运算
        if(binop->op=="&"||binop->op=="^"||binop->op=="|"||binop->op=="<<"||binop->op==">>")
        {
            tmp=1;
        }
        //算数运算
        if(binop->op == "+"||binop->op == "-"||binop->op=="*")
        {
              if(((constantNode *)right)->type==constant)
              {

                if(((constantNode *)right)->style == "int")
                {
                    tmp=2;
                }
                else if(((constantNode *)right)->style == "float")
                {
                    tmp=4;
                }
                else if(((constantNode *)right)->style == "double")
                {
                    tmp=4;
                }
               
              }
               else
                {
                    tmp=2;
                }
            
            
        }
        if(binop->op=="/")
        {
            if(((constantNode *)right)->type==constant)
            {
                if(((constantNode *)right)->style == "int")
                {
                    tmp=2;
                }
                if(((constantNode *)right)->style == "float")
                {
                    tmp=7;
                }
                if(((constantNode *)right)->style == "double")
                {
                    tmp=8;
                }
            }
            else 
            {
                tmp=2;
            }
        }
       /* if (binop->op != "=" && binop->op != ".")
        {
                expNode *left = binop->left;
                expNode *right = binop->right;
                if (right->type == constant)
                {
                    if (((constantNode *)right)->style == "double")
                    {
                        tmp =  FLOAT_ARITH_OP;
                        if (binop->op == "/") //仅当浮点的除法需要x16
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
        */
        else if (binop->op == "=")
        {
            tmp = 0;
        }
        else if (binop->op == ".")
        {
            workCompute(binop->left); //如果是点操作，则仅取左边表达式计算
            break;
        }
        work += tmp;
        WEST_astwalk(node);
        break;
    }
    case Ternary:
    {
       expNode * aNode=static_cast<ternaryNode *>(node)->first;
        top=FindRightSymbolTable(aNode->loc->first_line);
        Constant *bool_result=getOperationResult(aNode);
        workCompute(aNode);
        if(bool_result->bval==true)
        workCompute(static_cast<ternaryNode *>(node)->second);
        else
        workCompute(static_cast<ternaryNode *>(node)->third);
        //WEST_astwalk(node);
        break;
    }
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
                else
                {
                    expNode *right=((binopNode *)(init_exp))->right;
                    top=FindRightSymbolTable(init_exp->loc->first_line);
                    Constant *init_res=getOperationResult(right);
                    init=init_res->llval;
                }
                 
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
                //如果条件是标识符
                if(right->type==Id)
                {
                    
                    top=FindRightSymbolTable(cond_exp->loc->first_line);
                    Variable *var=top->LookupIdentifySymbol(((idNode*)right)->name);
                    condition=var->value->llval;  
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

                /*解决当末尾循环体增减不为1时，循环次数计算错误的问题*/
                if(op == "+="||op =="-=")
                {
                    expNode *left = ((binopNode *)(next_exp))->left;
                    expNode *right = ((binopNode *)(next_exp))->right;
                    if(right->type==constant||left->type==constant)
                    {
                        if(right->type==constant)
                        {
                            long long span=((constantNode *)right)->llval;
                            if(span!=1)
                            tmp=ceil(tmp*1.0/span);
                        }
                        else if(left->type==constant)
                        {
                            long long span=((constantNode *)left)->llval;
                            if(span!=1)
                            tmp=ceil(tmp*1.0/span);
                        }
                    }
                }
                /*解决当末尾循环体的运算是*=和/=时循环次数计算失败的问题*/
                else if (op == "*="||op == "/=")
                {
                    expNode *left=((binopNode *)(next_exp))->left;
                    expNode *right=((binopNode *)(next_exp))->right;
                    //int ccc;
                    long long span=0;
                    if(right->type==constant||left->type==constant)
                    {
                        if(right->type==constant)
                        {
                            span=((constantNode *)right)->llval;
                        }
                        else if(left->type==constant)
                        {
                            span=((constantNode *)left)->llval;
                        }
                    }
                    if (condition >= init)
                        tmp = condition / init;
                    else
                        tmp = init / condition;
                    tmp =ceil(log(tmp) / log(span));
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
        /*{
            ifNode * ifnode = static_cast<ifNode *>(node);
            expNode *if_exp = ifnode->exp;
            top=FindRightSymbolTable(if_exp->loc->first_line);
            Constant *bool_result = getOperationResult(if_exp);
            if(bool_result->bval==false)
            {
                workCompute(static_cast<ifNode *>(node)->exp);
            } 
            else
            {
                WEST_astwalk(node);  
            }
            work+=IF;
            break;
        }*/
    case IfElse:
       /*{
            ifElseNode * ifelsenode = static_cast<ifElseNode *>(node);
            expNode *ifelse_exp = ifelsenode->exp;
            top=FindRightSymbolTable(ifelse_exp->loc->first_line);
            Constant *bool_result = getOperationResult(ifelse_exp);
            workCompute(ifelse_exp);
            if(bool_result->bval==true)
            workCompute(ifelsenode->stmt1);
            else{
                workCompute(ifelsenode->stmt2);
            }

            work += IF;
        break;
        }*/
        oldWork = work;
        WEST_astwalk(node);
        newWork = work;
        work -= (newWork - oldWork) / 2;
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
        work+=CAST;
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
            //if (funcName == "acos")
               // work += 515 / 1;
            if (funcName == "acos")
                work += 380 / 1;
            //else if (funcName == "acosh")
              //  work += 665 / 1;
            else if (funcName == "acosh")
                work += 39 / 1;
            //else if (funcName == "asin")
              //  work += 536;
            else if (funcName =="asin")
                work += 380;
            else if (funcName == "asinh")
                work += 95 / 1;
            //else if (funcName == "asinh")
              //  work += 578 / 1    
            else if (funcName == "atan")
                work += 155 / 1;
            //else if (funcName == "atan")
            //    work += 195 / 1;
            else if (funcName == "atan2")
                work += 192 / 1;
            //else if (funcName == "atan2")
                //work += 272 / 1;
            else if (funcName == "atanh")
                work += 45 / 1;
            //else if (funcName == "atanh")
              //  work += 304 / 1;
            else if (funcName == "ceil")   
                work += 38 / 1;
            //else if (funcName == "ceil")   
              //  work += 47 / 1;
            else if (funcName == "cos")
                work += 144 / 1;
            //else if (funcName == "cos")
              //  work += 120 / 1;
            else if (funcName == "cosh")
                work += 152 / 1;
            //else if (funcName == "cosh")
                //work += 368 / 1;
            else if (funcName == "exp")
                work += 137 / 1;
            //else if (funcName == "exp")
              //  work += 162 / 1;
            else if (funcName == "expm1")
                work += 220 / 1;
            else if (funcName == "floor")
                work += 38 / 1;
            //else if (funcName == "floor")
             //   work += 58 / 1;
            else if (funcName == "fmod")
                work += 80 / 1;
            //else if (funcName == "fmod")
              //  work += 147 / 1;
            else if (funcName == "frexp")
                work += 52 / 1;
            //else if (funcName == "frexp")
              //  work += 60 / 1;
            else if (funcName == "log")
                work += 102 / 1;
            //else if (funcName == "log")
              //  work += 146 / 1;
            else if (funcName == "log10")
                work += 102 / 1;
            //else if (funcName == "log10")
              //  work += 212 / 1;
            else if (funcName == "log1p")
                work += 53 / 1;
            //else if (funcName == "log1p")
              //  work += 233 / 1;
            else if (funcName == "modf")
                work += 45 / 1;
            //else if (funcName == "modf")
              //  work += 41 / 1;
            else if (funcName == "pow")
                work += 30 / 1;
            //else if (funcName == "pow")
                //work += 554 / 1;
            else if (funcName == "sin")
                work += 160 / 1;
            //else if (funcName == "sin")
              //  work += 97 / 1;
            else if (funcName == "sinh")
                work += 148 / 1;
            //else if (funcName == "sinh")
              //  work += 303 / 1;
            else if (funcName == "sqrt")
                work += 54 / 1;
            //else if (funcName == "sqrt")
              //  work += 297 / 1;
            else if (funcName == "tan")
                work += 167 / 1;
            //else if (funcName == "tan")
              //  work += 224 / 1;
            else if (funcName == "tanh")
                work += 151 / 1;
            //else if (funcName == "tanh")
             //   work += 288 / 1;
            else if (funcName == "round")
                work += (23 + FLOAT_ARITH_OP) / 1;
            //else if (funcName == "round")
              //  work += (58 + FLOAT_ARITH_OP) / 1;
            else if (funcName == "abs")
                work += 11 / 1;
            //else if (funcName == "abs")
              //  work += 60 / 1;
            else if (funcName == "max")
                work += 10 / 1;
            //else if (funcName == "max")
              //  work += 60 / 1;
            else if (funcName == "min")
                work += 10 / 1;
            //else if (funcName == "min")
              //  work += 60 / 1;
            else if (funcName == "frta")
                work += FRTA_OP / 1;
            else if (funcName == "println")
                work += PRINTLN_OP / 1;
            /* 对于外部函数工作量估计为60 */
            else
            {
                //work += UNKNOWN_METHOD_CALL / 1;
                /****************外部函数的工作量估计*****************/
                list<Node *> arg_list=static_cast<callNode *>(node)->arg_list;
                funcDclNode* func_dcl=static_cast<callNode *>(node)->actual_callnode;
                funcBodyNode* func_body;
                if(Program!=NULL)
                {
                    for(auto it:*Program)
                    {
                        if(it->type==FuncDcl)
                        {
                            if(((funcDclNode *)it)->funcBody!=NULL)
                            {
                                func_body=((funcDclNode *)it)->funcBody;
                                break;
                            }
                        }
                        
                    }
                }
                //不含参数，返回常量
                 if(arg_list.empty())
                 {
                    
                    WorkFunc_withoutarg(func_body);
                
                    
                 }
                 //含参数，返回带参数的工作量计算式
                 else
                 {
                     list<Node *> parm=func_dcl->param_list;
                     if(func_body!=NULL)
                     {
                         WorkFunc_witharg(func_body);
                     }
                 }
            }
            //work += METHOD_CALL_OVERHEAD / 1;
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

void WorkFunc_witharg(funcBodyNode *func_body)
{
    
}
void WorkFunc_withoutarg(funcBodyNode *func_body)
{
    //funcBodyNode *body=static_cast<funcBodyNode*>(func_body);
    list<Node *> func_stmt=*func_body->stmt_list;
    if(!func_stmt.empty())
    {
        for(auto it : func_stmt)
        {
            workCompute(it);
        }
    }
    
}
void WEST_astwalk(Node *node)
{
    assert(node != NULL);
    switch (node->type)
    {
    case constant:
        break;
    case Id:
        break;
    /*case Ternary:
        workCompute(static_cast<ternaryNode *>(node)->first);
        workCompute(static_cast<ternaryNode *>(node)->second);
        workCompute(static_cast<ternaryNode *>(node)->third);
        break;*/
    case Binop:
        workCompute(static_cast<binopNode *>(node)->left);
        workCompute(static_cast<binopNode *>(node)->right);
        break;
    case Unary:
        workCompute(static_cast<unaryNode *>(node)->exp);
        break;
    case Cast:
        workCompute(static_cast<castNode *>(node)->exp);
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
        for (auto it : static_cast<blockNode *>(node)->stmt_list)
        {
            workCompute(it);
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
