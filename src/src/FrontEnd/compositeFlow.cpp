#include "compositeFlow.h"
#include "unfoldComposite.h"
#include "symboltableGenerate.h"
extern UnfoldComposite *unfold;
extern vector<Node *> compositeCall_list;
extern SymbolTable *runningTop;
extern SymbolTable *top;
extern list<SymbolTable *> runningStack;

/*
    功能：替换 CompositeCall 语句中的变量为常量
    输入参数：
*/
void variableReplace(Node *node){
        if(node->type == CompositeCall){
                compositeCallNode *call_composite = (compositeCallNode *)node;
                list<Node *> *params= call_composite->stream_List;
                list<Node *> *actual_params = new list<Node *>();
                if(params){
                for(auto param : *params){
                    if(param->type == Id){
                        Variable *value = runningTop->LookupIdentifySymbol(((idNode *)param)->name);
                        string type = value->value->type;
                        constantNode *constant_value;
                        if(type.compare("long long") == 0){
                            constant_value = new constantNode(type,value->value->llval);
                        }
                        if(type.compare("int") == 0){
                            constant_value = new constantNode(type,value->value->ival);
                        }
                        if(type.compare("long") == 0){
                            constant_value = new constantNode(type,value->value->lval);
                        }
                        if(type.compare("float") == 0){
                            constant_value = new constantNode(type,value->value->fval);
                        }
                        if(type.compare("double") == 0){
                            constant_value = new constantNode(type,value->value->dval);
                        }
                        actual_params->push_back(constant_value);
                    }
                    else if(param->type == constant){
                        actual_params->push_back(param);
                    }else if(param->type == Binop || param->type == Unary){
                        
                        Constant *value = getOperationResult(param);
                        string type = value->type;
                        constantNode *constant_value;
                        if(type.compare("long long") == 0){
                            constant_value = new constantNode(type,value->llval);
                        }
                        if(type.compare("int") == 0){
                            constant_value = new constantNode(type,value->ival);
                        }
                        if(type.compare("long") == 0){
                            constant_value = new constantNode(type,value->lval);
                        }
                        if(type.compare("float") == 0){
                            constant_value = new constantNode(type,value->fval);
                        }
                        if(type.compare("double") == 0){
                            constant_value = new constantNode(type,value->dval);
                        }
                        actual_params->push_back(constant_value);
                    }
                    call_composite->stream_List = actual_params;
                }
                }
        }
        if(node->type == SplitJoin){
            //找到for中的变量,然后替换,不支持for中变量的赋值
        }
        if(node->type == Pipeline){

        }
}

/*
    功能：解析 for 语句中的 CompositeCall,添加到 compositeCall_list 中,其他调用compositeFlow递归解析
    输入参数： 
*/
void addComposite(list<Node *> stmts){
    for(auto it : stmts){
        if (it->type == IfElse)
        {
            list<Node*> *if_list = new list<Node *>();
            if_list->push_back(it);
            compositeCallFlow(if_list);
        }
        else if(it->type == If){
            list<Node*> *if_list = new list<Node *>();
            if_list->push_back(it);
            compositeCallFlow(if_list);
        }
        else if (it->type == Add)
        {
                //cout << "type==" << ((addNode *)for_stmts)->content->type << endl;
            if (((addNode *)it)->content->type == CompositeCall)
            {
                compositeCallNode* composite_call = (compositeCallNode *)((addNode *)it)->content;
                //compositeCallNode* replace_composite = 
                Node *nd = unfold->workNodeCopy(((addNode *)it)->content);
                variableReplace(nd);
                compositeCall_list.push_back(nd);
            }
            /* add splitjoin{} */
            else if (((addNode *)it)->content->type == SplitJoin)
            {
                Node *nd = unfold->workNodeCopy(((addNode *)it)->content);
                variableReplace(nd);
                compositeCall_list.push_back(nd);
            }
            /* add pipeline{}的情况 */
            else if (((addNode *)it)->content->type == Pipeline)
            {
                Node *nd = unfold->workNodeCopy(((addNode *)it)->content);
                variableReplace(nd);
                compositeCall_list.push_back(nd);
            }

        }else if(it->type == For){
            list<Node*> *for_list = new list<Node *>();
            for_list->push_back(it);
            compositeCallFlow(for_list);
        }else if(it->type == Block){
            addComposite( ((blockNode *)it)->stmt_list);
        }else{
            genrateStmt(it); //解析变量值的变化
        }
    }
}


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
                Node *copy_nd = unfold->workNodeCopy(((addNode *)nd)->content);
                compositeCall_list.push_back(copy_nd);
            }
            /* add pipeline{}的情况 */
            else if (((addNode *)nd)->content->type == Pipeline)
            {
                Node *copy_nd = unfold->workNodeCopy(((addNode *)nd)->content);
                compositeCall_list.push_back(copy_nd);
            }
        }
        else if (nd->type == For)
        {
            runningTop = new SymbolTable(runningTop,NULL);
            top = runningTop; // test
            /*获得for循环中的init，cond和next值 目前只处理for循环中数据是整型的情况 */
            long long initial = MAX_INF;
            long long condition = MAX_INF;
            forNode *for_nd = (forNode *)nd;
            Node *init = for_nd->init;
            expNode *cond = for_nd->cond;
            expNode *next = for_nd->next;
            string con_op;
            string con_id;
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
                assert(con_init->type == constant && ((constantNode *)con_init)->style == "long long");
                initial = ((constantNode *)con_init)->llval; //todo 支持浮点数
                con_id = id_nd->name;
            }
            else if (init->type == Binop)
            {
                binopNode *init_b = (binopNode *)(init);
                Node *left = init_b->left;
                if(init_b->op.compare("=") == 0){
                    con_id = ((idNode *)left)->name;
                }
                Constant *con_init = getOperationResult(init_b->right);
                //binopNode *init_b = (binopNode *)init;
                //assert(init_b->right->type == constant);
                //constantNode *con_init = (constantNode *)(init_b->right);
                //assert(con_init->style == "integer");
                if(con_init->type.compare("int") == 0){
                    initial = con_init->ival;
                }
                if(con_init->type.compare("long") == 0){
                    initial = con_init->lval;
                }
                if(con_init->type.compare("long long") == 0){
                    initial = con_init->llval;
                }
            }

            // 将循环变量加入到 执行上下文 的 符号表
            Variable *init_v = new Variable("long long",con_id,initial);
            runningTop->InsertIdentifySymbol(init_v);

            /* 获取cond值 */
            if (cond->type == Binop)
            {
                binopNode *cond_b = (binopNode *)cond;
                //assert(cond_b->right->type == constant); todo
                //assert(con_cond->style == "integer");
                Constant *con_cond = getOperationResult(cond_b->right);
                if(con_cond->type.compare("int") == 0){
                    condition = con_cond->ival;
                }
                if(con_cond->type.compare("long") == 0){
                    condition = con_cond->lval;
                }
                if(con_cond->type.compare("long long") == 0){
                    condition = con_cond->llval;
                }
                /*if(condition_variable->type.compare("double") == 0){ // TODO 支持浮点数
                    condition = condition_variable->value->dval;
                }
                if(condition_variable->type.compare("float") == 0){
                    condition =  condition_variable->value->dval;
                }
                if(condition_variable->type.compare("string") == 0){
                    cout<<"string is not suitable in for";
                    exit(-1);
                }
                /*if(cond_b->right->type == constant){
                    constantNode *con_cond = (constantNode *)(cond_b->right);
                    condition = con_cond->llval;
                }else if(cond_b->right->type == Id){
                    //获得变量对应的值
                    idNode *con_cond = (idNode *)(cond_b->right);
                    Variable *condition_variable = runningTop->LookupIdentifySymbol(con_cond->name);
                    if(condition_variable->type.compare("int") == 0){
                        condition = condition_variable->value->llval;
                    }
                    if(condition_variable->type.compare("long") == 0){
                        condition = condition_variable->value->llval;
                    }
                    if(condition_variable->type.compare("long long") == 0){
                        condition = condition_variable->value->llval;
                    }
                    if(condition_variable->type.compare("integer") == 0){ // attention!
                        condition = condition_variable->value->llval;
                    }
                    /*if(condition_variable->type.compare("double") == 0){
                        condition = condition_variable->value->dval;
                    }
                    if(condition_variable->type.compare("float") == 0){
                        condition =  condition_variable->value->dval;
                    }
                    if(condition_variable->type.compare("string") == 0){
                        cout<<"string is not suitable in for";
                        exit(-1);
                    }
                }else if(cond_b->right->type = Binop){

                }
                */
                con_op = cond_b->op;
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
            list<long long> step_value = list<long long>(); //存储每次循环变量的值,不支持循环体内部改变该值
            if (next->type == Binop)
            {
                binopNode *next_b = ((binopNode *)next);
                Node *right = next_b->right;
                string op;
                long long step;

                if(right->type == constant){
                    Constant *step_v = getOperationResult(right);
                    if(step_v->type.compare("int") == 0){ //todo 支持浮点数
                        step = step_v->ival;
                    }
                    if(step_v->type.compare("long") == 0){
                        step = step_v->lval;
                    }
                    if(step_v->type.compare("long long") == 0){
                        step = step_v->llval;
                    }
                    op = next_b->op;
                }else if(right->type == Binop){ // 解析 i = i + x;
                    Node *next_left = ((binopNode *)right)->left;
                    Node *next_right = ((binopNode *)right)->right;
                    Constant *step_v;
                    if(((idNode *)next_left)->name.compare(con_id) != 0){
                        step_v = getOperationResult(next_left);
                    }else{
                        step_v = getOperationResult(next_right);
                    }
                    step = step_v->llval;// 只支持整型
                    op = ((binopNode *)right)->op;
                }
                if(con_op.compare("<") == 0 || con_op.compare("<=") == 0){
                    if (op == "*=" || op == "*")
                    {
                        int cnt = 0;
                        if (initial < condition)
                        {
                            for (int i = initial; i < condition; i *= step)
                                step_value.push_back(i);
                                //cnt++;
                            initial = 0;
                            //condition = cnt;
                        }
                        else
                        {
                            initial = 0;
                            condition = 0;
                        }
                    }
                    else if (op == "/=" || op == "/")
                    {
                        int cnt = 0;
                        if (initial < condition && step < 1)
                        {
                            for (int i = initial; i < condition; i /= step)
                                step_value.push_back(i);
                                //cnt++;
                            initial = 0;
                            //condition = cnt;
                        }
                        else
                        {
                            cout << " infinite loop " << endl;
                        }
                    }
                    else if(op == "+=" || op == "+"){
                        int cnt = 0;
                        if (initial < condition && step > 0)
                        {
                            for (int i = initial; i < condition; i += step)
                                step_value.push_back(i);
                                //cnt++;
                            initial = 0;
                            //condition = cnt;
                        }
                        else
                        {
                            cout << " infinite loop " << endl;
                        }
                    }
                    else if(op == "-=" || op == "-"){
                        int cnt = 0;
                        if (initial < condition && step < 0) //todo 未考虑step为负
                        {
                            for (int i = initial; i < condition; i -= step)
                                step_value.push_back(i);
                                //cnt++;
                            initial = 0;
                            //condition = cnt;
                        }
                        else
                        {
                            cout << " infinite loop " << endl;
                        }
                    }       
                }else{
                    if (op == "*=" || op == "*")
                    {
                        int cnt = 0;
                        if (initial > condition && step < 1)
                        {
                            for (int i = initial; i > condition; i *= step)
                                step_value.push_back(i);
                                //cnt++;
                            initial = 0;
                            //condition = cnt;
                        }
                        else
                        {
                            initial = 0;
                            condition = 0;
                        }
                    }
                    else if (op == "/=" || op == "/")
                    {
                        int cnt = 0;
                        if (initial > condition)
                        {
                            for (int i = initial; i > condition; i /= step)
                                step_value.push_back(i);
                                //cnt++;
                            initial = 0;
                            //condition = cnt;
                        }
                        else
                        {
                            cout << " infinite loop " << endl;
                        }
                    }
                    else if(op == "+=" || op == "+"){
                        int cnt = 0;
                        if (initial > condition && step < 0)
                        {
                            for (int i = initial; i > condition; i += step)
                                step_value.push_back(i);
                                //cnt++;
                            initial = 0;
                            //condition = cnt;
                        }
                        else
                        {
                            cout << " infinite loop " << endl;
                        }
                    }
                    else if(op == "-=" || op == "-"){
                        int cnt = 0;
                        if (initial > condition && step > 0) //todo 未考虑step为负
                        {
                            for (int i = initial; i > condition; i -= step)
                                step_value.push_back(i);
                                //cnt++;
                            initial = 0;
                            //condition = cnt;
                        }
                        else
                        {
                            cout << " infinite loop " << endl;
                        }
                    } 
                }
                
            }else if(next->type == Unary){
                unaryNode *next_u = (unaryNode *)(next);
                int cnt = 0;
                if(next_u->op.compare("POSTINC") == 0){ //++
                    if(con_op.compare("<") == 0 || con_op.compare("<=") == 0){
                        if(initial < condition){
                            for (int i = initial; i < condition; i ++)
                                step_value.push_back(i);
                                //cnt++;
                            initial = 0;
                            //condition = cnt;
                        }else{
                            cout << " infinite loop " << endl;
                            exit(-1);
                        }
                    }else{
                        cout << " infinite loop " << endl;
                        exit(-1);
                    }
                }else if(next_u->op.compare("POSTDEC") == 0){ //--
                    if(con_op.compare(">") == 0 || con_op.compare(">=") == 0){
                        if(initial > condition){
                            for (int i = initial; i > condition; i--)
                                step_value.push_back(i);
                                //cnt++;
                            initial = 0;
                            //condition = cnt;
                        }else{
                            cout << " infinite loop " << endl;
                            exit(-1);
                        } 
                    }else{
                        cout << " infinite loop " << endl;
                        exit(-1);
                    }
                }    
            }
            for(auto it : step_value){
                list<Node *> stmt = list<Node *>();
                init_v->value->llval = it;
                stmt.push_back(for_nd->stmt);
                addComposite(stmt);
            }
            /* for循环中只有一条语句 */
            /*if (for_nd->stmt->type != Block)
            {
                Node *for_stmts = for_nd->stmt;
                if (for_stmts->type == IfElse)
                {
                        // todo
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
                        //add splitjoin{} 
                        else if (((addNode *)for_stmts)->content->type == SplitJoin)
                        {
                            Node *nd = unfold->workNodeCopy(((addNode *)for_stmts)->content);
                            compositeCall_list.push_back(nd);
                        }
                        // add pipeline{}的情况 
                        else if (((addNode *)for_stmts)->content->type == Pipeline)
                        {
                            Node *nd = unfold->workNodeCopy(((addNode *)for_stmts)->content);
                            compositeCall_list.push_back(nd);
                        }
                    }
                }
            }
            else // for语句中多句
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
                    //add splitjoin{} 
                    else if (((addNode *)ptr)->content->type == SplitJoin)
                    {
                        Node *nd = unfold->workNodeCopy(((addNode *)ptr)->content);
                        compositeCall_list.push_back(nd);
                    }
                    //add pipeline{}的情况 
                    else if (((addNode *)ptr)->content->type == Pipeline)
                    {
                        Node *nd = unfold->workNodeCopy(((addNode *)ptr)->content);
                        compositeCall_list.push_back(nd);
                    }
                }
            }*/
        }
        else if (nd->type == IfElse)
        {
            runningTop = new SymbolTable(runningTop,NULL);
            top = runningTop; // test

            list<Node *> *ifelse_list = new list<Node*>();
            ifElseNode *if_node = (ifElseNode *)nd;
            Constant *bool_result = getOperationResult(if_node->exp);
            if(bool_result->bval){
                ifelse_list->push_back(if_node->stmt1);
            }else{
                ifelse_list->push_back(if_node->stmt2);
            }
            compositeCallFlow(ifelse_list);
            // todo
        }
        else if(nd->type == If){
            runningTop = new SymbolTable(runningTop,NULL); // 新作用域
            top = runningTop; // test

            list<Node *> *ifelse_list = new list<Node*>();
            ifNode *if_node = (ifNode *)nd;
            Constant *bool_result = getOperationResult(if_node->exp);
            if(bool_result->bval){
                ifelse_list->push_back(if_node->stmt);
            }
            compositeCallFlow(ifelse_list);
        }else{
            genrateStmt(nd);
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