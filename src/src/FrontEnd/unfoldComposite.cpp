#include "unfoldComposite.h"
extern SymbolTable S;
/* 这个函数需要常量传播，目前为理想的情况 splitjoin,pipeline的循环结构都为常量*/
void UnfoldComposite::setCallList(list<Node *> *stmts)
{
    /*遍历splitjoin/pipeline结构中的statement，将compositecallNode加入到call_List中*/
    for (auto it : *(stmts))
    {
        //cout << it->type << endl;
        if (it->type == Add)
        {
            if (((addNode *)it)->content->type == CompositeCall)
                call_List.push_back(((addNode *)it)->content);
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
                assert(con_init->style == "interger");
                initial = con_init->llval;
            }
            assert(cond->type == Binop);
            cond_b = (binopNode *)cond;
            assert(cond_b->right->type == constant);
            constantNode *con_cond = (constantNode *)(cond_b->right);
            assert(con_cond->style == "interger");
            condition = con_cond->llval;
            //cout << "init= " << initial << " cond= " << condition << endl;
            assert(initial != MAX_INF && condition != MAX_INF);
            /* 不一定是block */
            assert(node->stmt->type == Block);
            stmts = ((blockNode *)(node->stmt))->stmt_list;
            assert(stmts->size() == 1);
            auto ptr = stmts->front();
            assert(ptr->type == Add);
            for (long long i = initial; i < condition; ++i)
            {
                if (((addNode *)ptr)->content->type == CompositeCall)
                {
                    call_List.push_back(((addNode *)ptr)->content);
                }
            }
        }
    }
}

operatorNode *UnfoldComposite::MakeSplitOperator(Node *input, list<Node *> *arguments, int style)
{
    operatorNode *res = NULL;
    windowNode *window = NULL;
    operBodyNode *body = NULL;
    vector<string> operName({"Duplicate", "Roundrobin"});
    vector<string> streamName({"dup", "round"});
    list<Node *> *outputs = new list<Node *>();
    list<Node *> *inputs = new list<Node *>();
    list<Node *> *win_stmt = new list<Node *>();
    assert(input->type == Id);
    string input_name = ((idNode *)input)->name;
    int len = call_List.size();
    static int number = 0;
    long long sum = 0;
    inputs->push_back(input);
    assert(arguments->size() == 0 || arguments->size() == 1 || arguments->size() == len);
    Node *arg = arguments->front();
    /* arg的type可以为constatnt后者id 但是id必须是个常量（需要常量传播） */
    //cout << "arg.type= " << arg->type << endl;
    // if(arg->type==1)
    // cout<<"idName= "<<((idNode*)arg)->name<<endl;

    /*若split roundroubin()参数为空 默认赋值一个数据大小*/
    if (arguments->size() == 0)
    {
        constantNode *nd = new constantNode("interger", (long long)1);
        arguments->push_back(nd);
    }
    if (arguments->size() == 1)
    {
        for (int i = 1; i < len; ++i)
            arguments->push_back(arg);
    }
    //cout << "arguments->size()= "<<arguments->size() << endl;
    int cnt = 0;
    for (auto it : *(arguments))
    {
        sum += ((constantNode *)it)->llval;
        string tempName = streamName[style] + to_string(number) + "_" + to_string(cnt);
        cnt++;
        idNode *id = new idNode(tempName);
        outputs->push_back(id);
        if (style == 1)
        {
            tumblingNode *tum = new tumblingNode(new list<Node *>({it}));
            winStmtNode *win = new winStmtNode(tempName, tum);
            win_stmt->push_back(win);
        }
        else if (style == 0)
        {
            slidingNode *slid = new slidingNode(new list<Node *>({it}));
            winStmtNode *win = new winStmtNode(tempName, slid);
            win_stmt->push_back(win);
        }
    }
    //添加split节点输入窗口
    if (style == 0)
    {
        sum = ((constantNode *)arg)->llval;
    }
    constantNode *constNode = new constantNode("integer", sum);
    if (style == 1)
    {
        tumblingNode *tum = new tumblingNode(new list<Node *>({constNode}));
        winStmtNode *win = new winStmtNode(input_name, tum);
        win_stmt->push_back(win);
    }
    else if (style == 0)
    {
        slidingNode *slid = new slidingNode(new list<Node *>({constNode}));
        winStmtNode *win = new winStmtNode(input_name, slid);
        win_stmt->push_back(win);
    }
    // cout<<"win_stmt.size()= "<<win_stmt->size()<<endl;
    // cout<<"outputs.size()= "<<outputs->size()<<endl;
    window = new windowNode(win_stmt);
    body = new operBodyNode(NULL, NULL, NULL, window);
    res = new operatorNode(outputs, operName[style], inputs, body);
    number++;
    //cout << "-----------------split end---------------------" << endl;
    return res;
}

operatorNode *UnfoldComposite::MakeJoinOperator(Node *output, list<Node *> *inputs, list<Node *> *arguments)
{
    operatorNode *res = NULL;
    windowNode *window = NULL;
    operBodyNode *body = NULL;
    string operName = "join";
    assert(output->type == Id);
    string output_name = ((idNode *)output)->name;
    int len = call_List.size();
    long long sum = 0;
    list<Node *> *outputs = new list<Node *>();
    list<Node *> *win_stmt = new list<Node *>();
    assert(arguments->size() == 1 || arguments->size() == len);
    Node *arg = arguments->front();
    outputs->push_back(output);
    if (arguments->size() == 1)
    {
        for (int i = 1; i < len; ++i)
            arguments->push_back(arg);
    }
    assert(inputs->size() == arguments->size());
    auto iter = inputs->begin();

    //cout<<"iter->type="<<(*iter)->type<<endl;
    for (auto it : *(arguments))
    {
        sum += ((constantNode *)it)->llval;
        //cout << ((idNode *)*iter)->name << endl;
        slidingNode *slid = new slidingNode(new list<Node *>({it}));
        winStmtNode *win = new winStmtNode(((idNode *)*iter)->name, slid);
        win_stmt->push_back(win);
        iter++;
    }
    /*添加join节点输出窗口*/
    constantNode *constNode = new constantNode("integer", sum);
    tumblingNode *tum = new tumblingNode(new list<Node *>({constNode}));
    winStmtNode *win = new winStmtNode(output_name, tum);
    win_stmt->push_back(win);
    /*end*/
    window = new windowNode(win_stmt);
    body = new operBodyNode(NULL, NULL, NULL, window);
    res = new operatorNode(outputs, operName, inputs, body);
    //cout << "-----------------join end---------------------" << endl;
    return res;
}

compositeNode *UnfoldComposite::UnfoldSplitJoin(splitjoinNode *node)
{
    compositeNode *tmp = NULL;
    string comName = MakeCompositeName("splitjoin");
    setCallList(node->bodyStmt_List);
    if (node->split->dup_round->type == RoundRobin)
    {
        tmp = UnfoldRoundrobin(comName, node);
    }
    else
    {
        tmp = UnfoldDuplicate(comName, node);
    }

    return tmp;
}

compositeNode *UnfoldComposite::UnfoldRoundrobin(string comName, splitjoinNode *node)
{
    string streamName = "Rstream";
    static int number = 0;
    vector<compositeCallNode *> comCallList;
    compositeNode *roundrobin = NULL;
    operatorNode *splitOperator = NULL, *joinOperator = NULL;
    list<Node *> *tempList = new list<Node *>();
    /* arg_list表示split roundrobin(size);的size参数列表 */
    list<Node *> *arg_list = ((roundrobinNode *)node->split->dup_round)->arg_list;
    list<Node *> *inputs_split = node->inputs;
    list<Node *> *outputs = node->outputs;
    list<Node *> *inputs_join = new list<Node *>();
    list<Node *> *call_outputs = NULL;
    ComInOutNode *inout = new ComInOutNode(inputs_split, outputs);
    compHeadNode *head = new compHeadNode(comName, inout);
    compBodyNode *body = NULL;
    /* comp_stmt_List表示所构建的compositeNode语句 */
    list<Node *> *comp_stmt_List = new list<Node *>();
    assert(inputs_split != NULL && outputs != NULL);
    //cout << "inputs.size()= " << inputs->size() << " outputs.size()= " << outputs->size() << endl;
    //1.构建splitoperator，构建输出输入流 与composite调用关联
    splitOperator = MakeSplitOperator(inputs_split->front(), arg_list, 1);
    /*2.构建compositeCall，与split,join节点的输入输出流关联 */
    tempList = splitOperator->outputs;
    auto iter = tempList->begin();
    int cnt = 0;
    for (auto it : call_List)
    {
        assert(it->type == CompositeCall);
        string name = (((compositeCallNode *)it)->compName);
        string tempName = streamName + to_string(number) + "_" + (to_string(cnt));
        idNode *id = new idNode(tempName);
        //compositeCall的输出流是join节点的输入流
        inputs_join->push_back(id);
        list<Node *> *outputs = new list<Node *>({id});
        //compositeCall的输入流
        list<Node *> *inputs = new list<Node *>({*iter});
        compositeNode *comp = S.LookupCompositeSymbol(name);
        assert(comp != NULL);
        /*修改composite节点的输入流,输出流*/
        compositeNode *actual_composite = compositeCallStreamReplace(comp, inputs, outputs);
        //修改compositeCall的输入输出流
        compositeCallNode *call = new compositeCallNode(outputs, tempName, NULL, inputs, actual_composite);
        //cout<<"address: "<<&(call->inputs)<<endl;
        comCallList.push_back(call);
        // auto kkk = comCallList.front()->actual_composite;
        // list<Node *> *stmts = kkk->body->stmt_List;
        // Node *node = ((binopNode *)stmts->front())->right;
        // cout << "address = " << node << endl;
        // list<Node *> *ss = ((operatorNode *)node)->outputs;
        // list<Node *> *ss2 = ((operatorNode *)node)->inputs;
        // cout << "address: "<<&((idNode *)(ss->front()))->name<<"  output: " << ((idNode *)(ss->front()))->name << "  input: " << ((idNode *)(ss2->front()))->name << endl;

        iter++;
        cnt++;
    }
    /*3.构建joinOperator，与compositeCall的输入输出流关联 */
    joinOperator = MakeJoinOperator(outputs->front(), inputs_join, arg_list);
    comp_stmt_List->push_back(splitOperator);
    for (auto it : comCallList)
    {
        comp_stmt_List->push_back(it);
    }
    comp_stmt_List->push_back(joinOperator);
    body = new compBodyNode(NULL, comp_stmt_List);
    roundrobin = new compositeNode(head, body);
    ++number;
    call_List.clear();
    return roundrobin;
}

compositeNode *UnfoldComposite::UnfoldDuplicate(string comName, splitjoinNode *node)
{
    compositeNode *dup = NULL;
    static int number = 0;
    list<Node *> *tempList = new list<Node *>();
    operatorNode *splitOperator = NULL, *joinOperator = NULL;
    /* arg_list表示split roundrobin(size);的size参数列表 */
    list<Node *> *arg_list = ((roundrobinNode *)node->split->dup_round)->arg_list;
    list<Node *> *inputs_split = node->inputs;
    list<Node *> *outputs = node->outputs;
    list<Node *> *inputs_join = new list<Node *>();
    list<Node *> *call_outputs = NULL;
    list<compositeCallNode *> *comCallList = new list<compositeCallNode *>();
    ComInOutNode *inout = new ComInOutNode(inputs_split, outputs);
    compHeadNode *head = new compHeadNode(comName, inout);
    compBodyNode *body = NULL;
    /* comp_stmt_List表示所构建的compositeNode语句 */
    list<Node *> *comp_stmt_List = new list<Node *>();
    assert(inputs_split != NULL && outputs != NULL);
    //cout << "inputs.size()= " << inputs->size() << " outputs.size()= " << outputs->size() << endl;
    //1.构建splitoperator，构建输出输入流 与composite调用关联
    splitOperator = MakeSplitOperator(inputs_split->front(), arg_list, 0);
    tempList = splitOperator->outputs;
    auto iter = tempList->begin();
    int cnt = 0;
    for (auto it : call_List)
    {
        assert(it->type == CompositeCall);
        string name = (((compositeCallNode *)it)->compName);
        string tempName = name + to_string(number) + "_" + (to_string(cnt));
        idNode *id = new idNode(tempName);
        //compositeCall的输出流是join节点的输入流
        inputs_join->push_back(id);
        list<Node *> *outputs = new list<Node *>({id});
        //compositeCall的输入流
        list<Node *> *inputs = new list<Node *>({*iter});
        compositeNode *actual_composite = S.LookupCompositeSymbol(name);
        assert(actual_composite != NULL);
        compositeCallNode *call = new compositeCallNode(outputs, tempName, NULL, inputs, actual_composite);
        comCallList->push_back(call);
        iter++;
        cnt++;
    }
    //cout<<"comCallList->size()= "<<comCallList->size()<<endl;
    joinOperator = MakeJoinOperator(outputs->front(), inputs_join, arg_list);
    comp_stmt_List->push_back(splitOperator);
    for (auto it : *comCallList)
    {
        comp_stmt_List->push_back(it);
    }
    comp_stmt_List->push_back(joinOperator);
    //cout<<"comp_stmt_List->size()= "<<comp_stmt_List->size()<<endl;
    body = new compBodyNode(NULL, comp_stmt_List);
    dup = new compositeNode(head, body);
    ++number;
    call_List.clear();
    return dup;
}

compositeNode *UnfoldComposite::UnfoldPipeline(Node *node)
{
    compositeNode *pipeline = NULL;

    return pipeline;
}

compositeNode *UnfoldComposite::compositeCallStreamReplace(compositeNode *comp, list<Node *> *inputs, list<Node *> *outputs)
{
    compositeNode *copy = NULL;
    list<Node *> *stmt_list = NULL;
    stmt_list = new list<Node *>();
    ComInOutNode *inout = new ComInOutNode(inputs, outputs);
    compHeadNode *head = new compHeadNode(comp->compName, inout);
    /* 取得compositeNode的operBody 指向同一块内存 替换输入输出流*/
    for (auto it : *comp->body->stmt_List)
    {
        if (it->type == Binop)
        {
            expNode *exp = ((binopNode *)it)->right;
            if (exp->type == Operator_)
            {
                operBodyNode *operBody = ((operatorNode *)exp)->operBody;
                operatorNode *oper = new operatorNode(outputs, comp->compName, inputs, operBody);
                stmt_list->push_back(oper);
            }
            /* 未来可扩展splitjoin嵌套splitjoin的结构 */
        }
    }
    compBodyNode *body = new compBodyNode(NULL, stmt_list);
    copy = new compositeNode(head, body);
    return copy;
}

compositeNode *UnfoldComposite::splitJoinStreamReplace(compositeNode *composite, list<Node *> *inputs, list<Node *> *outputs)
{
    list<Node *> *stmt_list = NULL;
    stmt_list = composite->body->stmt_List;
    assert(stmt_list != NULL);
    Node *top = stmt_list->front();
    Node *back = stmt_list->back();
    assert(top->type == Operator_ && back->type == Operator_);
    ((operatorNode *)top)->inputs = inputs;
    ((operatorNode *)back)->outputs = outputs;
    return composite;
}

compositeNode *UnfoldComposite::streamReplace(compositeNode *comp, list<Node *> *inputs, list<Node *> *outputs)
{
    // if(inputs!=NULL){
    //     for(auto it:*inputs){
    //         cout<<it->type<<endl;
    //     }
    // }
    // Node *nd1 = NULL, *nd2 = NULL;
    // if (inputs != NULL && inputs->size() != 0)
    // {
    //     nd1 = inputs->front();
    // }
    // if (outputs != NULL && outputs->size() != 0)
    // {
    //     nd1 = inputs->front();
    // }
    
    // if(nd1==NULL) cout<<"inputs : NULL  ";
    // else cout<<"inputs : "<<((idNode*)nd1)->name<<"  ";
    // if(nd2==NULL) cout<<"outputs : NULL  ";
    // else cout<<"outputs : "<<((idNode*)nd2)->name<<"  ";

    list<Node *> *stmt_list = NULL;
    stmt_list = comp->body->stmt_List;
    assert(stmt_list != NULL);
    Node *top = stmt_list->front();
    Node *back = stmt_list->back();
    if (top->type == Binop)
    {
        expNode *exp = ((binopNode *)top)->right;
        if (exp->type == Operator_)
        {
            ((operatorNode *)top)->inputs = (inputs != NULL) ? inputs : new list<Node *>();
        }
    }
    if (back->type == Binop)
    {
        expNode *exp = ((binopNode *)top)->right;
        if (exp->type == Operator_)
        {
            ((operatorNode *)back)->outputs = (outputs != NULL) ? outputs : new list<Node *>();
        }
    }
    return comp;
}