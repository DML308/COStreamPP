#include "compositeFlow.h"
#include "unfoldComposite.h"
extern UnfoldComposite *unfold;
extern vector<Node *> compositeCall_list;

/* 这个函数需要常量传播，目前为理想的情况 splitjoin,pipeline的循环结构都为常量*/
void compositeCallFlow(list<Node *> *stmts)
{
    //cout<<"stmts.size() = "<<stmts->size()<<endl;
    /*遍历splitjoin/pipeline结构中的statement，将compositecallNode加入到compositeCall_list中*/
    for (auto nd : *(stmts))
    {
        //cout<<"type:"<<nd->type<<endl;
        if (nd->type == Add)
        {
            /* add composite()的情况 */
            if (((addNode *)nd)->content->type == CompositeCall)
                compositeCall_list.push_back(((addNode *)nd)->content);
            /* add splitjoin{}的情况*/
            else if (((addNode *)nd)->content->type == SplitJoin)
            {
                Node *nd = unfold->workNodeCopy(((addNode *)nd)->content);
                compositeCall_list.push_back(nd);
            }
            /* add pipeline{}的情况 */
            else if (((addNode *)nd)->content->type == Pipeline)
            {
                Node *nd = unfold->workNodeCopy(((addNode *)nd)->content);
                compositeCall_list.push_back(nd);
            }
        }
        else if (nd->type == For)
        {
            /*获得for循环中的init，cond和next值 目前只处理for循环中数据是整型的情况 */
            long long initial = MAX_INF;
            long long condition = MAX_INF;
            forNode *for_nd = (forNode *)nd;
            Node *init = for_nd->init;
            expNode *cond = for_nd->cond;
            expNode *next = for_nd->next;
            list<Node *> *stmts = NULL;
            if (init->type == Decl)
            {
                declareNode *init_d = static_cast<declareNode *>(init);
                idNode *id_nd = init_d->id_list.front();
                /* 必须初始化 */
                if (id_nd->init == NULL)
                {
                    cout << "for init部分未初始化 " << endl;
                    exit(-1);
                }
                initNode *init_nd = (initNode *)(id_nd->init);
                Node *con_init = init_nd->value.front();
                assert(con_init->type == constant && ((constantNode *)con_init)->style == "integer");
                initial = ((constantNode *)con_init)->llval;
            }
            else if (init->type == Binop)
            {
                binopNode *init_b = (binopNode *)init;
                assert(init_b->right->type == constant);
                constantNode *con_init = (constantNode *)(init_b->right);
                assert(con_init->style == "integer");
                initial = con_init->llval;
            }
            /* 获取cond值 */
            if (cond->type == Binop)
            {
                binopNode *cond_b = (binopNode *)cond;
                assert(cond_b->right->type == constant);
                constantNode *con_cond = (constantNode *)(cond_b->right);
                assert(con_cond->style == "integer");
                condition = con_cond->llval;
                if (cond_b->op == "<" || cond_b->op == ">")
                    condition += 0;
                else
                    condition += 1;
            }
            //cout << "init= " << initial << " cond= " << condition << endl;
            if (initial == MAX_INF || condition == MAX_INF)
            {
                cout << "init or condition is not a constant !";
                exit(-1);
            }
            /* 获取next值 */
            if (next->type == Binop)
            {
                binopNode *next_b = ((binopNode *)next);
                expNode *right = next_b->right;
                assert(right->type == constant);
                long long step = ((constantNode *)right)->llval;
                if (next_b->op == "*=")
                {
                    int cnt = 0;
                    if (initial < condition)
                    {
                        for (int i = initial; i < condition; i *= step)
                            cnt++;
                        initial = 0;
                        condition = cnt;
                    }
                    else
                    {
                        initial = 0;
                        condition = 0;
                    }
                }
                else if (next_b->op == "/=")
                {
                    int cnt = 0;
                    if (initial > condition)
                    {
                        for (int i = initial; i < condition; i /= step)
                            cnt++;
                        initial = 0;
                        condition = cnt;
                    }
                    else
                    {
                        cout << " infinite loop " << endl;
                    }
                }
            }
            /* for循环中只有一条语句 */
            if (for_nd->stmt->type != Block)
            {
                Node *for_stmts = for_nd->stmt;
                if (for_stmts->type == IfElse)
                {
                }
                else if (for_stmts->type == Add)
                {
                    //cout << "type==" << ((addNode *)for_stmts)->content->type << endl;
                    for (long long i = initial; i < condition; ++i)
                    {
                        if (((addNode *)for_stmts)->content->type == CompositeCall)
                        {
                            compositeCall_list.push_back(((addNode *)for_stmts)->content);
                        }
                        /* add splitjoin{} */
                        else if (((addNode *)for_stmts)->content->type == SplitJoin)
                        {
                            Node *nd = unfold->workNodeCopy(((addNode *)for_stmts)->content);
                            compositeCall_list.push_back(nd);
                        }
                        /* add pipeline{}的情况 */
                        else if (((addNode *)for_stmts)->content->type == Pipeline)
                        {
                            Node *nd = unfold->workNodeCopy(((addNode *)for_stmts)->content);
                            compositeCall_list.push_back(nd);
                        }
                    }
                }
            }
            else
            {
                stmts = &((blockNode *)(for_nd->stmt))->stmt_list;
                auto ptr = stmts->front();
                assert(ptr->type == Add);
                // cout << "init = "<<initial << endl;
                // cout << "cond = "<<condition << endl;
                for (long long i = initial; i < condition; ++i)
                {
                    if (((addNode *)ptr)->content->type == CompositeCall)
                    {
                        compositeCall_list.push_back(((addNode *)ptr)->content);
                    }
                    /* add splitjoin{} */
                    else if (((addNode *)ptr)->content->type == SplitJoin)
                    {
                        Node *nd = unfold->workNodeCopy(((addNode *)ptr)->content);
                        compositeCall_list.push_back(nd);
                    }
                    /* add pipeline{}的情况 */
                    else if (((addNode *)ptr)->content->type == Pipeline)
                    {
                        Node *nd = unfold->workNodeCopy(((addNode *)ptr)->content);
                        compositeCall_list.push_back(nd);
                    }
                }
            }
        }
    }
}

/*
*   功能：对所有Main composite的composite调用进行实际流边量名的替换
*   输入参数：gMaincomposite
*/
void streamFlow(compositeNode *main)
{
    list<Node *> body_stmt = *(main->body->stmt_List);
    for (auto it : body_stmt)
    {
        switch (it->type)
        {
        case Binop:
        {
            expNode *right = static_cast<binopNode *>(it)->right;
            if (right->type == CompositeCall)
            {
                compositeNode *comp = ((compositeCallNode *)right)->actual_composite;
                ((compositeCallNode *)right)->actual_composite = unfold->streamReplace(comp,
                                                                                       ((compositeCallNode *)right)->inputs, ((compositeCallNode *)right)->outputs, 1);
            }
            break;
        }
        case CompositeCall:
        {
            compositeNode *comp = ((compositeCallNode *)it)->actual_composite;
            ((compositeCallNode *)it)->actual_composite = unfold->streamReplace(comp,
                                                                                ((compositeCallNode *)it)->inputs, ((compositeCallNode *)it)->outputs, 1);
            break;
        }
        default:
            break;
        }
    }
}