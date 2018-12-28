#include "compositeFlow.h"
#include "unfoldComposite.h"
extern UnfoldComposite *unfold;
extern vector<Node *> compositeCall_list;

/* 这个函数需要常量传播，目前为理想的情况 splitjoin,pipeline的循环结构都为常量*/
void compositeCallFlow(list<Node *> *stmts){
     /*遍历splitjoin/pipeline结构中的statement，将compositecallNode加入到compositeCall_list中*/
    for (auto it : *(stmts))
    {
        cout << "it->type = "<<it->type << endl;
        if (it->type == Add)
        {
            if (((addNode *)it)->content->type == CompositeCall)
                compositeCall_list.push_back(((addNode *)it)->content);
            cout<<"-------------if------------"<<endl;
        }
        else if (it->type == For)
        {
            long long initial = MAX_INF;
            long long condition = MAX_INF;
            forNode *node = (forNode *)it;
            Node *init = node->init;
            expNode *cond = node->cond;
            expNode *next = node->next;
            list<Node *> *stmts = NULL;
            binopNode *init_b = NULL, *cond_b = NULL, *next_b = NULL;
            if (init->type == Decl)
            {
            }
            else if (init->type == Binop)
            {
                init_b = (binopNode *)init;
                assert(init_b->right->type == constant);
                constantNode *con_init = (constantNode *)(init_b->right);
                assert(con_init->style == "integer");
                initial = con_init->llval;
            }
            assert(cond->type == Binop);
            cond_b = (binopNode *)cond;
            assert(cond_b->right->type == constant);
            constantNode *con_cond = (constantNode *)(cond_b->right);
            assert(con_cond->style == "integer");
            condition = con_cond->llval;
            //cout << "init= " << initial << " cond= " << condition << endl;
            assert(initial != MAX_INF && condition != MAX_INF);
            // 不一定是block 
            assert(node->stmt->type == Block);
            stmts = ((blockNode *)(node->stmt))->stmt_list;
            assert(stmts->size() == 1);
            auto ptr = stmts->front();
            assert(ptr->type == Add);
            cout<<initial<<endl;
            cout<<cond<<endl;
            for (long long i = initial; i < condition; ++i)
            {
                if (((addNode *)ptr)->content->type == CompositeCall)
                {
                    compositeCall_list.push_back(((addNode *)ptr)->content);
                }
            }
        }
    }
     cout<<"comcall.size()= "<<compositeCall_list.size()<<endl;

}


void streamFlow(compositeNode *main)
{
    list<Node *> body_stmt = *(main->body->stmt_List);

    for (auto it : body_stmt)
    {
        //cout<<it->type<<endl;
        switch (it->type)
        {

        case Binop:
        {
            expNode *right = static_cast<binopNode *>(it)->right;
            if(right->type == CompositeCall)
            {
                compositeNode *comp = ((compositeCallNode *)right)->actual_composite;
                ((compositeCallNode *)right)->actual_composite = unfold->streamReplace(comp, 
                ((compositeCallNode *)right)->inputs, ((compositeCallNode *)right)->outputs,1);
            }
            if(right->type==Pipeline){
                cout<<"body.size()= "<<((pipelineNode*)right)->body_stmts->size()<<endl;
                for(auto it:*((pipelineNode*)right)->body_stmts){
                    cout<<it->type<<endl;
                }
            }
            
            break;
        }
        case CompositeCall:
        {
            compositeNode *comp = ((compositeCallNode *)it)->actual_composite;
            ((compositeCallNode *)it)->actual_composite = unfold->streamReplace(comp, 
            ((compositeCallNode *)it)->inputs, ((compositeCallNode *)it)->outputs,1);
            break;
        }
        default:
            break;
        }
    }
}