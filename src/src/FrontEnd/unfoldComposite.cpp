#include "unfoldComposite.h"
#include "compositeFlow.h"
#include <string.h>
extern SymbolTable S;
extern list<Node*> *Program;
extern squentialNode *globalSquential;
vector<Node *> compositeCall_list; //存储splitjoin/pipeline中的compositeCall调用

Node *UnfoldComposite::MakeRoundrobinWork(list<Node *> *inputs, list<Node *> *arguments, list<Node *> *outputs)
{
    list<Node *> *stmts = new list<Node *>();
    Node *work = NULL, *for_node = NULL;
    Node *init = NULL, *cond = NULL, *next_i = NULL, *next_j = NULL, *stmt = NULL;
    Node *input = inputs->front();
    assert(input->type == Id);
    constantNode *const_zero = new constantNode("integer", (long long)0);
    constantNode *const_i = new constantNode("integer", (long long)0);
    constantNode *const_j = new constantNode("integer", (long long)0);
    initNode *init_i = new initNode(const_i);
    initNode *init_j = new initNode(const_j);
    idNode *id_i = new idNode("i"), *id_j = new idNode("j");
    id_i->init = init_i;
    id_j->init = init_j;
    primNode *prim = new primNode("int");
    declareNode *declI = new declareNode(prim, id_i), *declJ = new declareNode(prim, id_j);
    stmts->push_back(declI);
    stmts->push_back(declJ);
    auto pos = outputs->begin();
    for (auto arg : *arguments)
    {
        init = new binopNode((expNode *)id_i, "=", (expNode *)const_zero);
        cond = new binopNode((expNode *)id_i, "<", (expNode *)arg);
        next_i = new unaryNode("PREINC", (expNode *)id_i);
        next_j = new unaryNode("POSTINC", (expNode *)id_j);
        idNode *left = new idNode((static_cast<idNode *>(*pos))->name);
        left->isArray = 1;
        left->arg_list.push_back(id_i);
        idNode *right = new idNode(static_cast<idNode *>(input)->name);
        right->isArray = 1;
        right->arg_list.push_back(next_j);

        stmt = new binopNode((expNode *)left, "=", (expNode *)right);
        for_node = new forNode(init, (expNode *)cond, (expNode *)next_i, stmt);

        stmts->push_back(for_node);
        pos++;
    }
    work = new blockNode(stmts);
    return work;
}

Node *UnfoldComposite::MakeJoinWork(list<Node *> *inputs, list<Node *> *arguments, list<Node *> *outputs)
{
    list<Node *> *stmts = new list<Node *>();
    Node *work = NULL, *for_node = NULL;
    Node *init = NULL, *cond = NULL, *next_i = NULL, *next_j = NULL, *stmt = NULL;
    Node *output = outputs->front();
    assert(output->type == Id);
    constantNode *const_zero = new constantNode("integer", (long long)0);
    constantNode *const_i = new constantNode("integer", (long long)0);
    constantNode *const_j = new constantNode("integer", (long long)0);
    initNode *init_i = new initNode(const_i);
    initNode *init_j = new initNode(const_j);
    idNode *id_i = new idNode("i"), *id_j = new idNode("j");
    id_i->init = init_i;
    id_j->init = init_j;
    primNode *prim = new primNode("int");
    declareNode *declI = new declareNode(prim, id_i), *declJ = new declareNode(prim, id_j);
    stmts->push_back(declI);
    stmts->push_back(declJ);
    auto pos = inputs->begin();
    for (auto arg : *arguments)
    {
        init = new binopNode((expNode *)id_i, "=", (expNode *)const_zero);
        cond = new binopNode((expNode *)id_i, "<", (expNode *)arg);
        next_i = new unaryNode("PREINC", (expNode *)id_i);
        next_j = new unaryNode("POSTINC", (expNode *)id_j);
        idNode *left = new idNode(static_cast<idNode *>(output)->name);
        left->isArray = 1;
        left->arg_list.push_back(next_j);
        idNode *right = new idNode((static_cast<idNode *>(*pos))->name);
        right->isArray = 1;
        right->arg_list.push_back(id_i);
        stmt = new binopNode((expNode *)left, "=", (expNode *)right);
        for_node = new forNode(init, (expNode *)cond, (expNode *)next_i, stmt);
        stmts->push_back(for_node);
        pos++;
    }
    work = new blockNode(stmts);
    return work;
}
operatorNode *UnfoldComposite::MakeSplitOperator(Node *input, list<Node *> *arguments, int style)
{
    operatorNode *res = NULL;
    Node *work = NULL;
    windowNode *window = NULL;
    operBodyNode *body = NULL;
    vector<string> operName({"Duplicate", "Roundrobin"});
    vector<string> streamName({"dup", "round"});
    list<Node *> *outputs = new list<Node *>();
    list<Node *> *inputs = new list<Node *>();
    list<Node *> *win_stmt = new list<Node *>();
    assert(input->type == Id);
    string input_name = ((idNode *)input)->name;
    int len = compositeCall_list.size();
    static int number = 0;
    long long sum = 0;
    inputs->push_back(input);
    //arguments可能为NULL，就分配一块内存
    if (arguments == NULL)
        arguments = new list<Node *>();
    assert(arguments->size() == 0 || arguments->size() == 1 || arguments->size() == len);

    /*若split roundroubin()参数为空 默认赋值一个数据大小*/
    if (arguments->size() == 0)
    {
        constantNode *nd = new constantNode("integer", (long long)1);
        arguments->push_back(nd);
    }
    Node *arg = arguments->front();

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
        /* 此处可以抽出写为函数 */
        if (style == 1)
        {
            tumblingNode *tum = new tumblingNode(new list<Node *>({it}));
            winStmtNode *win = new winStmtNode(tempName, tum);
            win_stmt->push_back(win);
        }
        else if (style == 0)
        {
            slidingNode *slid = new slidingNode(new list<Node *>({it, it}));
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
        slidingNode *slid = new slidingNode(new list<Node *>({constNode, constNode}));
        winStmtNode *win = new winStmtNode(input_name, slid);
        win_stmt->push_back(win);
    }
    // cout<<"win_stmt.size()= "<<win_stmt->size()<<endl;
    // cout<<"outputs.size()= "<<outputs->size()<<endl;
    work = MakeRoundrobinWork(inputs, arguments, outputs);
    window = new windowNode(win_stmt);
    body = new operBodyNode(NULL, NULL, work, window);
    res = new operatorNode(outputs, operName[style], inputs, body);
    number++;
    //cout << "-----------------split end---------------------" << endl;
    return res;
}

operatorNode *UnfoldComposite::MakeJoinOperator(Node *output, list<Node *> *inputs, list<Node *> *arguments)
{
    Node *work = NULL;
    operatorNode *res = NULL;
    windowNode *window = NULL;
    operBodyNode *body = NULL;
    string operName = "join";
    assert(output->type == Id);
    string output_name = ((idNode *)output)->name;
    int len = compositeCall_list.size();
    long long sum = 0;
    list<Node *> *outputs = new list<Node *>();
    list<Node *> *win_stmt = new list<Node *>();
    //当arguments为空，即分配内存
    if (arguments == NULL)
        arguments = new list<Node *>();
    assert(arguments->size() == 0 || arguments->size() == 1 || arguments->size() == len);
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
        slidingNode *slid = new slidingNode(new list<Node *>({it, it}));
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
    work = MakeJoinWork(inputs, arguments, outputs);
    window = new windowNode(win_stmt);
    body = new operBodyNode(NULL, NULL, work, window);
    res = new operatorNode(outputs, operName, inputs, body);
    //cout << "-----------------join end---------------------" << endl;
    return res;
}

compositeNode *UnfoldComposite::UnfoldSplitJoin(splitjoinNode *node)
{
    compositeNode *tmp = NULL;
    string comName = MakeCompositeName("splitjoin");
    compositeCallFlow(node->body_stmts);
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
    static int number1 = 0;
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
    for (auto it : compositeCall_list)
    {
        assert(it->type == CompositeCall);
        string name = (((compositeCallNode *)it)->compName);
        string tempName = streamName + to_string(number1) + "_" + (to_string(cnt));
        idNode *id = new idNode(tempName);
        //compositeCall的输出流是join节点的输入流
        inputs_join->push_back(id);
        list<Node *> *call_outputs = new list<Node *>({id});
        //compositeCall的输入流
        list<Node *> *call_inputs = new list<Node *>({*iter});
        compositeNode *comp = S.LookupCompositeSymbol(name);
        assert(comp != NULL);
        /*修改composite节点的输入流,输出流*/
        compositeNode *actual_composite = compositeCallStreamReplace(comp, call_inputs, call_outputs);
        //修改compositeCall的输入输出流
        compositeCallNode *call = new compositeCallNode(call_outputs, name, NULL, call_inputs, actual_composite);
        //cout<<"address: "<<&(call->inputs)<<endl;
        comCallList.push_back(call);
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
    ++number1;
    compositeCall_list.clear();
    return roundrobin;
}

compositeNode *UnfoldComposite::UnfoldDuplicate(string comName, splitjoinNode *node)
{
    compositeNode *dup = NULL;
    string streamName = "Dstream";
    static int number2 = 0;
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

    for (auto it : compositeCall_list)
    {
        string name = (((compositeCallNode *)it)->compName);
        string tempName = streamName + to_string(number2) + "_" + (to_string(cnt));
        idNode *id = new idNode(tempName);
        //compositeCall的输出流是join节点的输入流
        inputs_join->push_back(id);
        list<Node *> *call_outputs = new list<Node *>({id});
        //compositeCall的输入流
        list<Node *> *call_inputs = new list<Node *>({*iter});
        compositeNode *comp = S.LookupCompositeSymbol(name);
        assert(comp != NULL);

        /*修改composite节点的输入流,输出流*/
        compositeNode *actual_composite = compositeCallStreamReplace(comp, call_inputs, call_outputs);
        compositeCallNode *call = new compositeCallNode(call_outputs, tempName, NULL, call_inputs, actual_composite);
        //cout<<"compName= "<<tempName<<endl;
        comCallList->push_back(call);
        iter++;
        cnt++;
    }
    joinOperator = MakeJoinOperator(outputs->front(), inputs_join, arg_list);
    comp_stmt_List->push_back(splitOperator);
    for (auto it : *comCallList)
    {
        comp_stmt_List->push_back(it);
    }
    comp_stmt_List->push_back(joinOperator);
    body = new compBodyNode(NULL, comp_stmt_List);
    dup = new compositeNode(head, body);
    ++number2;
    compositeCall_list.clear();
    return dup;
}

compositeNode *UnfoldComposite::UnfoldPipeline(pipelineNode *node)
{
    compositeCallFlow(node->body_stmts);
    vector<compositeCallNode *> comCallList;
    compositeNode *pipeline = NULL;
    string streamName = "Pstream";
    static int num = 0;
    string comName = MakeCompositeName("pipeline");
    list<Node *> *inputs = node->inputs;
    list<Node *> *outputs = node->outputs;
    // cout<<"pre input name ="<<((idNode*)inputs->front())->name<<endl;
    // cout<<"pre output name = "<<((idNode*)outputs->front())->name<<endl;

    ComInOutNode *inout = new ComInOutNode(inputs, outputs);
    compHeadNode *head = new compHeadNode(comName, inout);
    compBodyNode *body = NULL;
    assert(inputs != NULL && outputs != NULL);
    list<Node *> *comp_stmts = new list<Node *>();
    /* 构造pipeline内的节点的输入输出流 */
    int cnt = 0;
    list<Node *> *temp_stream = new list<Node *>({inputs->front()});
    /* 遍历所有的compositeCall，生成对应输入输出流的compositecallnode */
    //cout<<"compositeCall_list= "<<compositeCall_list.size()<<endl;
    for (auto it = compositeCall_list.begin(); it != compositeCall_list.end(); ++it)
    {
        string name = (((compositeCallNode *)*it)->compName);
        string tempName = streamName + to_string(num) + "_" + (to_string(cnt++));
        idNode *id = new idNode(tempName);
        /* pipeline中第一个composite节点 */
        //compositeCall的输入流
        list<Node *> *call_inputs = new list<Node *>({temp_stream->front()});
        list<Node *> *call_outputs = new list<Node *>({id});
        /*将现在的输出流作为下一次的输入流*/
        temp_stream->pop_back();
        temp_stream->push_back(id);

        /*  pipeline中最后一个composite节点那么输出流应该为pipeline的输出流 */
        if (it == --compositeCall_list.end())
        {
            call_outputs->pop_back();
            call_outputs->push_back(outputs->front());
        }
        compositeNode *comp = S.LookupCompositeSymbol(name);
        assert(comp != NULL);
        /*修改composite节点的输入流,输出流*/
        // cout<<"input: "<<((idNode*)call_inputs->front())->name<<endl;
        // cout<<"output: "<<((idNode*)call_outputs->front())->name<<endl;
        compositeNode *actual_composite = compositeCallStreamReplace(comp, call_inputs, call_outputs);
        compositeCallNode *call = new compositeCallNode(call_outputs, name, NULL, call_inputs, actual_composite);
        //cout<<"actual composite name ="<<actual_composite->compName<<endl;

        comCallList.push_back(call);
    }
    //cout<<"comCallList.size( )= "<<comCallList.size()<<endl;
    for (auto nd : comCallList)
        comp_stmts->push_back(nd);
    body = new compBodyNode(NULL, comp_stmts);
    pipeline = new compositeNode(head, body);
    ++num;
    compositeCall_list.clear();
    return pipeline;
}
/* 专用于splitjoin或者pipeline中展开流的替换，这些compositeCall可以指向相同的init，work*/
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
            //cout<<"exp->type ="<<exp->type<<endl;
            if (exp->type == Operator_)
            {
                /* 除了window都可以指向一块内存 对于window动态分配一块内存，替换window中的名字，在函数的结尾将流进行替换*/
                operBodyNode *operBody = ((operatorNode *)exp)->operBody;
                list<Node *> *preInputs = ((operatorNode *)exp)->inputs;
                list<Node *> *preOutputs = ((operatorNode *)exp)->outputs;
                string operName = ((operatorNode *)exp)->operName;
                paramNode *param = operBody->param;
                list<Node *> *stmts = operBody->stmt_list;
                Node *init = operBody->init;
                Node *work = operBody->work;
                list<Node *> *win_list = new list<Node *>();
                /*动态分配生成新的workNode 对work中的Node进行拷贝*/
                //assert(work->type == Block);
                work = workNodeCopy(work);
                //cout << "---------------------------------------------" << endl;
                /*动态分配生成新的windowNode*/
                for (auto it : *operBody->win->win_list)
                {
                    Node *winType = ((winStmtNode *)it)->winType;
                    string winName = ((winStmtNode *)it)->winName;
                    winStmtNode *stmt_node = new winStmtNode(winName, winType);
                    win_list->push_back(stmt_node);
                }
                windowNode *win = new windowNode(win_list);
                operBodyNode *body = new operBodyNode(stmts, init, work, win);
                operatorNode *oper = new operatorNode(preOutputs, operName, preInputs, body);
                /* 修改输入输出的流名 */
                modifyStreamName(oper, inputs, true);
                modifyStreamName(oper, outputs, false);
                stmt_list->push_back(oper);
                compBodyNode *com_body = new compBodyNode(NULL, stmt_list);
                copy = new compositeNode(head, com_body);
            }
            //若为pipeline或者splitjoin直接返回composite
            else if (exp->type == Pipeline || exp->type == SplitJoin)
                copy = comp;
        }
    }
    streamReplace(copy, inputs, outputs, 0);
    return copy;
}

/* 对于composite节点内的operatorNode进行流替换 可修改一个compositeNode包含多个operator */
compositeNode *UnfoldComposite::streamReplace(compositeNode *comp, list<Node *> *inputs, list<Node *> *outputs, int flag)
{
    //cout<<"compName = "<<comp->compName<<endl;
    list<Node *> *stmt_list = NULL;
    assert(comp->body != NULL);
    stmt_list = comp->body->stmt_List;
    assert(stmt_list != NULL);
    //cout<<"stmts.size()= "<<stmt_list->size();
    Node *top = stmt_list->front();
    Node *back = stmt_list->back();
    // 修改composite输入对应的operator
    switch (top->type)
    {
    case Operator_:
        if (flag)
            modifyStreamName(((operatorNode *)top), inputs, true);
        ((operatorNode *)top)->inputs = inputs;
        break;
    case Binop:
        expNode *exp = ((binopNode *)top)->right;
        if (exp->type == Operator_)
        {
            if (flag)
                modifyStreamName(((operatorNode *)exp), inputs, true);
            ((operatorNode *)exp)->inputs = inputs;
        }
        else if (exp->type == SplitJoin)
        {
            ((splitjoinNode *)exp)->inputs = inputs;
        }
        else if (exp->type == Pipeline)
        {
            ((pipelineNode *)exp)->inputs = inputs;
        }
        break;
    }
    // 修改composite输出对应的operator
    switch (back->type)
    {
    case Operator_:
        if (flag)
            modifyStreamName(((operatorNode *)back), outputs, false);
        ((operatorNode *)back)->outputs = outputs;
        break;
    case Binop:
    {
        expNode *exp = ((binopNode *)back)->right;
        if (exp->type == Operator_)
        {
            if (flag)
                modifyStreamName(((operatorNode *)exp), outputs, false);
            ((operatorNode *)exp)->outputs = outputs;
        }
        else if (exp->type == SplitJoin)
        {
            ((splitjoinNode *)exp)->outputs = outputs;
        }
        else if (exp->type == Pipeline)
        {
            ((pipelineNode *)exp)->outputs = outputs;
        }
    }
    }
    return comp;
}

/* style标识输入流还是输出流 */
void UnfoldComposite::modifyStreamName(operatorNode *oper, list<Node *> *stream, bool style)
{
    if (stream != NULL)
    {
        list<Node *> *inputs = NULL, *outputs = NULL;
        assert(stream->front()->type == Id);
        string replaceName = ((idNode *)stream->front())->name;
        list<Node *> *win_stmts = oper->operBody->win->win_list;
        Node *work = oper->operBody->work;
        assert(work->type == Block);
        list<Node *> *work_stmts = ((blockNode *)work)->stmt_list;
        switch (style)
        {
        case true:
            inputs = oper->inputs;
            if (inputs != NULL && inputs->size() != 0)
            {
                //operatorNode中只有一个输入流和一个输出流
                string name = ((idNode *)(inputs->front()))->name;
                /* 替换window中流输入名*/
                if (win_stmts != NULL)
                {
                    for (auto it : *win_stmts)
                    {
                        if (((winStmtNode *)it)->winName == name)
                            ((winStmtNode *)it)->winName = replaceName;
                    }
                }
                /* 替换work中使用的形式参数流输入名 */
                if (work_stmts != NULL)
                {
                    for (auto it : *work_stmts)
                        modifyWorkName(it, replaceName, name);
                }
            }
            break;
        case false:
            outputs = oper->outputs;
            if (outputs != NULL && outputs->size() != 0)
            {
                string name = ((idNode *)(outputs->front()))->name;
                /* 替换window中流输出名*/
                if (win_stmts != NULL)
                {
                    for (auto it : *win_stmts)
                    {
                        if (((winStmtNode *)it)->winName == name)
                            ((winStmtNode *)it)->winName = replaceName;
                    }
                }
                /* 替换work中使用的形式参数流输出名 */
                if (work_stmts != NULL)
                {
                    for (auto it : *work_stmts)
                        modifyWorkName(it, replaceName, name);
                }
            }
            break;
        }
    }
}

Node *UnfoldComposite::workNodeCopy(Node *u)
{
    switch (u->type)
    {
    case constant:
        break;
    case Decl:
    {
        Node *prim = workNodeCopy(static_cast<declareNode *>(u)->prim);
        Node *id = static_cast<declareNode *>(u)->id_list.front();
        declareNode *tmp = new declareNode((primNode *)prim, (idNode *)id);
        for (auto it = ++static_cast<declareNode *>(u)->id_list.begin(); it != static_cast<declareNode *>(u)->id_list.end(); ++it)
            tmp->id_list.push_back((idNode *)workNodeCopy(*it));
        return tmp;
        break;
    }
    case Id:
    {
        idNode *node = static_cast<idNode *>(u);
        idNode *tmp = new idNode(node->name);
        tmp->arg_list = node->arg_list;
        tmp->init = node->init;
        tmp->isArray = node->isArray;
        tmp->isParam = node->isParam;
        tmp->isStream = node->isStream;
        tmp->level = node->level;
        tmp->version = node->version;
        tmp->type = node->type;
        tmp->valType = node->valType;
        //cout << "location:" << node->loc->first_line << " idname= " << node->name << "   " << node << " " << u << endl;
        return tmp;
        break;
    }
    case Binop:
    {
        Node *left = workNodeCopy(static_cast<binopNode *>(u)->left);
        Node *right = workNodeCopy(static_cast<binopNode *>(u)->right);
        binopNode *tmp = new binopNode((expNode *)left, static_cast<binopNode *>(u)->op, (expNode *)right);
        return tmp;
        break;
    }
    case Paren:
    {
        Node *exp = workNodeCopy(static_cast<parenNode *>(u)->exp);
        parenNode *tmp = new parenNode((expNode *)exp);
        return tmp;
        break;
    }
    case Unary:
    {
        Node *exp = workNodeCopy(static_cast<unaryNode *>(u)->exp);
        unaryNode *tmp = new unaryNode(static_cast<unaryNode *>(u)->op, (expNode *)exp);
        return tmp;
        break;
    }
    case Cast:
    {

        workNodeCopy(static_cast<castNode *>(u)->exp);
        break;
    }
    case Ternary:
    {
        Node *first = workNodeCopy(static_cast<ternaryNode *>(u)->first);
        Node *second = workNodeCopy(static_cast<ternaryNode *>(u)->second);
        Node *third = workNodeCopy(static_cast<ternaryNode *>(u)->third);
        ternaryNode *tmp = new ternaryNode((expNode *)first, (expNode *)second, (expNode *)third);
        return tmp;
        break;
    }
    case Initializer:
    case Label:
        break;
    case Switch:
    {
        Node *exp = workNodeCopy(static_cast<switchNode *>(u)->exp);
        Node *stat = workNodeCopy(static_cast<switchNode *>(u)->stat);
        switchNode *tmp = new switchNode((expNode *)exp, stat);
        return tmp;
        break;
    }
    case Case:
    {
        Node *exp = workNodeCopy(static_cast<caseNode *>(u)->exp);
        Node *stmt = workNodeCopy(static_cast<caseNode *>(u)->stmt);
        caseNode *tmp = new caseNode((expNode *)exp, (expNode *)stmt);
        return tmp;
        break;
    }
    case Default:
    {
        return static_cast<defaultNode *>(u);
        break;
    }
    case If:
    {
        Node *exp = workNodeCopy(static_cast<ifNode *>(u)->exp);
        Node *stmt = workNodeCopy(static_cast<ifNode *>(u)->stmt);
        ifNode *tmp = new ifNode((expNode *)exp, stmt);
        return tmp;
        break;
    }
    case IfElse:
    {
        Node *exp = workNodeCopy(static_cast<ifElseNode *>(u)->exp);
        Node *stmt1 = workNodeCopy(static_cast<ifElseNode *>(u)->stmt1);
        Node *stmt2 = workNodeCopy(static_cast<ifElseNode *>(u)->stmt2);
        ifElseNode *tmp = new ifElseNode((expNode *)exp, (expNode *)stmt1, (expNode *)stmt2);
        return tmp;
        break;
    }
    case While:
    {
        workNodeCopy(static_cast<whileNode *>(u)->exp);
        workNodeCopy(static_cast<whileNode *>(u)->stmt);
        break;
    }
    case Do:
    {
        workNodeCopy(static_cast<doNode *>(u)->exp);
        workNodeCopy(static_cast<doNode *>(u)->stmt);
        break;
    }
    case For:
    {
        Node *init = workNodeCopy(static_cast<forNode *>(u)->init);
        Node *cond = workNodeCopy(static_cast<forNode *>(u)->cond);
        Node *next = workNodeCopy(static_cast<forNode *>(u)->next);
        Node *stmt = workNodeCopy(static_cast<forNode *>(u)->stmt);
        forNode *tmp = new forNode(init, (expNode *)cond, (expNode *)next, stmt);
        return tmp;
        break;
    }
    case Continue:
    {
        return static_cast<continueNode *>(u);
        break;
    }
    case Break:
    {
        return static_cast<breakNode *>(u);
        break;
    }
    case Return:
    {
        Node *exp = workNodeCopy(static_cast<returnNode *>(u)->exp);
        returnNode *tmp = new returnNode((expNode *)tmp);
        return tmp;
        break;
    }
    case Block:
    {
        list<Node *> *stmt_list = new list<Node *>();
        list<Node *> *stmts = static_cast<blockNode *>(u)->stmt_list;
        if (stmts != NULL)
        {
            for (auto it : *stmts)
            {
                stmt_list->push_back(workNodeCopy(it));
            }
        }
        blockNode *block = new blockNode(stmt_list);
        return block;
        break;
    }
    case primary:
    {
        return static_cast<primNode *>(u);
        break;
    }
    case Array:
    {
        return static_cast<arrayNode *>(u);
        break;
    }
    case Call:
    {
        list<Node *> *ids = new list<Node *>();
        for (auto it : static_cast<callNode *>(u)->arg_list)
        {
            ids->push_back(workNodeCopy(it));
        }
        callNode *tmp = new callNode(static_cast<callNode *>(u)->name, ids);
        return tmp;
        break;
    }
    default:
        break;
    }
    return u;
}

/*递归遍历work中的语法树  替换stream形式参数(in out等)为实际的流变量名 */
void UnfoldComposite::modifyWorkName(Node *u, string replaceName, string name)
{
    switch (u->type)
    {
    case Id:
    {
        if (static_cast<idNode *>(u)->name == name)
        {
            //cout << "*******************" << name << " " << u << "*************************" << endl;
            static_cast<idNode *>(u)->name = replaceName;
        }
        break;
    }
    case Cast:
    {
        modifyWorkName(static_cast<castNode *>(u)->prim, replaceName, name);
        modifyWorkName(static_cast<castNode *>(u)->exp, replaceName, name);
        break;
    }

    case Binop:
    {
        modifyWorkName(static_cast<binopNode *>(u)->left, replaceName, name);
        modifyWorkName(static_cast<binopNode *>(u)->right, replaceName, name);
        break;
    }
    case Unary:
    {
        modifyWorkName(static_cast<unaryNode *>(u)->exp, replaceName, name);
        break;
    }
    case Ternary:
    {
        modifyWorkName(static_cast<ternaryNode *>(u)->first, replaceName, name);
        modifyWorkName(static_cast<ternaryNode *>(u)->second, replaceName, name);
        modifyWorkName(static_cast<ternaryNode *>(u)->third, replaceName, name);
        break;
    }
    case Switch:
    {
        modifyWorkName(static_cast<switchNode *>(u)->exp, replaceName, name);
        modifyWorkName(static_cast<switchNode *>(u)->stat, replaceName, name);
        break;
    }
    case Case:
    {
        modifyWorkName(static_cast<caseNode *>(u)->exp, replaceName, name);
        modifyWorkName(static_cast<caseNode *>(u)->stmt, replaceName, name);
        break;
    }
    case Default:
    {
        modifyWorkName(static_cast<defaultNode *>(u)->stmt, replaceName, name);
        break;
    }
    case If:
    {
        modifyWorkName(static_cast<ifNode *>(u)->stmt, replaceName, name);
        break;
    }
    case Paren:
    {
        modifyWorkName(static_cast<parenNode *>(u)->exp, replaceName, name);
        break;
    }
    case IfElse:
    {
        modifyWorkName(static_cast<ifElseNode *>(u)->stmt1, replaceName, name);
        modifyWorkName(static_cast<ifElseNode *>(u)->stmt2, replaceName, name);
        break;
    }
    case While:
    {
        modifyWorkName(static_cast<whileNode *>(u)->exp, replaceName, name);
        modifyWorkName(static_cast<whileNode *>(u)->stmt, replaceName, name);
        break;
    }
    case Do:
    {
        modifyWorkName(static_cast<doNode *>(u)->exp, replaceName, name);
        modifyWorkName(static_cast<doNode *>(u)->stmt, replaceName, name);
        break;
    }
    case For:
    {
        modifyWorkName(static_cast<forNode *>(u)->stmt, replaceName, name);
        break;
    }
    case Return:
    {
        modifyWorkName(static_cast<returnNode *>(u)->exp, replaceName, name);
        break;
    }
    case Block:
    {
        list<Node *> *stmts = static_cast<blockNode *>(u)->stmt_list;
        if (stmts != NULL)
        {
            for (auto it : *stmts)
            {
                modifyWorkName(it, replaceName, name);
            }
        }
        break;
    }
    case Decl:
    {
        for (auto it : static_cast<declareNode *>(u)->id_list)
            modifyWorkName(it, replaceName, name);
        break;
    }
    case Call:
    {
        for (auto it : static_cast<callNode *>(u)->arg_list)
            modifyWorkName(it, replaceName, name);
        break;
    }
    default:
        break;
    }
}
compositeNode *UnfoldComposite::UnfoldSquential(squentialNode *node) {
    globalSquential = node;
    cout<<node->body_stmts->size()<<endl;
    compositeCallFlow(node->body_stmts); // 将通过add加入的层,依次push到compositeCall_list中
    vector<compositeCallNode *> comCallList; // 用于存储展开后的compositeCallNode
    compositeNode *squential = NULL;
    string streamName = "Sstream";
    static int num = 0;
    string comName = MakeCompositeName("squential");
    list<Node *> *inputs = node->inputs;
    list<Node *> *outputs = node->outputs;
    list<Node *> *arg_list = node->arg_list;
    list<Node *> *comp_stmts = new list<Node *>();
    cout<<"unfold squential"<<endl;
    ComInOutNode *inout = new ComInOutNode(inputs, outputs);
    compHeadNode *head = new compHeadNode(comName, inout);
    compBodyNode *body = NULL;
    // ???
    // EnterScope();
    // ???
    SymbolTable *top = new SymbolTable(NULL);
    // squential有两个输入流, 分别是是训练集和标签
    assert(inputs != NULL && outputs != NULL);
    //  temp del
    // assert(inputs->size() == 2);
    int levelNum = compositeCall_list.size();
    int currentLevel = 0;
    // next step在composite中加入stream的声明!!!!
    cout << ((constantNode *)(node->arg_list->front()))->style << endl;
    // assert(((constantNode *)(node->arg_list->front()))->style == "integer");
    // 将层连接起来
    for (auto iter = compositeCall_list.begin(); (iter + 1) != compositeCall_list.end(); iter++) {
        ((layerNode *)*iter)->level = ++currentLevel;
        ((layerNode *)*iter)->nextLayer  = ((layerNode *)*(iter+1));
        ((layerNode *)*(iter+1))->prevLayer  = ((layerNode *)*iter);
        // assert(((constantNode *)(((layerNode *)*iter)->arg_list->front()))->style == "integer");
    }
    ((layerNode *)compositeCall_list.back())->level = ++currentLevel;
    Node *weightType = new primNode("double");
    Node *prevDim = new constantNode("integer",((constantNode *)(node->arg_list->front()))->llval);
    // 以全局变量声明参数
    for (auto iter = compositeCall_list.begin(); iter != compositeCall_list.end(); iter++) {
        // 声明_weight_[prevDim][dim]
        Node *dim = new constantNode("integer", ((constantNode *)(((layerNode *)*iter)->arg_list->front()))->llval);
        string weightName = "_weight_" + to_string(((layerNode *)*iter)-> level);
        Node *weight =  new idNode(weightName);
        ((idNode *)weight)->isArray = 1;
        Node* arrDecl = new arrayNode((expNode *)prevDim);
        (static_cast<arrayNode *>(arrDecl))->arg_list.push_back((expNode *)dim);
        (static_cast<idNode *>(weight))->arg_list = (static_cast<arrayNode *>(arrDecl))->arg_list;
        Node* weightDecl = new declareNode((primNode*)weightType,(static_cast<idNode*>(weight)));
        Program->push_front(weightDecl);
        cout<<"Program size=" << Program->size() << endl;
        prevDim = dim;
    }
    cout<<"Program size=" << Program->size() << endl;
    // 取得输入到squential的训练集
    list<Node *> *temp_stream = new list<Node *>({inputs->front()});
    // 取得输入到squential的标签
    list<Node *> *y_stream = new list<Node *>({inputs->back()});
    // 声明stream
    idNode *streamDeclId = new idNode("x");
    top->put("x", streamDeclId);
    primNode *streamType = new primNode("double");
    streamDeclId->valType = streamType->name;
    // 声明成Node还是strdclNode
    Node *streamDecl = new strdclNode(streamDeclId);
    // 
    // streamDecl->id_list.push_back()
    // 展开前向传播composite
    for (auto iter = compositeCall_list.begin(); iter != compositeCall_list.end(); iter++) {
        list<Node *> *call_inputs, *call_outputs;
        string name = ((layerNode *)*iter)->layerName + to_string(((layerNode *)*iter)->level);
        // cout<<((layerNode *)*iter)->level<<endl;
        if (*iter != compositeCall_list.back()) {
            // cout<< ((layerNode *)*iter)->layerName;
            // cout<< to_string(((layerNode *)*iter)->level)<<endl;
            string namePrefix = streamName + "_F" + ((layerNode *)*iter)->layerName + to_string(((layerNode *)*iter)->level) + "_";
            // 正向传递给下一层的stream名称
            string tempName1 = namePrefix + "F" + ((layerNode *)*iter)->nextLayer->layerName + to_string(((layerNode *)*iter)->nextLayer->level);
            idNode *id1 = new idNode(tempName1);
            // 传递给反向传播中本层的stream名称
            //string tempName2 = namePrefix + "B" + ((layerNode *)*iter)->nextLayer->layerName + to_string(((layerNode *)*iter)->nextLayer->level);
            //idNode *id2 = new idNode(tempName2);
            // 将反向传播要连接的id存储到符号表中???
            //top->put(tempName2 ,id2);
            // 将数据流声明加入
            ((strdclNode*)streamDecl)->id_list.push_back(id1);
            call_inputs = new list<Node *>({temp_stream->front()});
            //call_outputs = new list<Node *>({id1, id2});
            call_outputs = new list<Node *>({id1});
            temp_stream->pop_back();
            temp_stream->push_back(call_outputs->front());
        } else {
            // 对正向传播过程的最后一层要做特别的处理, 他只有一个输出流
            // 只有正向传播的时候, output为输出
            call_inputs = new list<Node *>({temp_stream->front()});
            call_outputs = new list<Node *>({outputs->front()});
            // 带有反向传播
            // cout<<"forward last layer"<<endl;
            // string tempName =  streamName  + "_F" + ((layerNode *)*iter)->layerName + to_string(((layerNode *)*iter)->level) + "_B" + ((layerNode *)*iter)->layerName + to_string(((layerNode *)*iter)-> level);
            // idNode *id = new idNode(tempName);
            // call_inputs = new list<Node *>({temp_stream->front()});
            // call_outputs = new list<Node *>({id});
            // temp_stream->pop_back();
            // temp_stream->push_back(call_outputs->front());
        }
        // 构造实际的正向传播composite  ???未完成
        compositeNode* actual_composite = makeForwardComposite((layerNode *) *iter, call_inputs, call_outputs);
        compositeCallNode *call = new compositeCallNode(call_outputs, name, NULL, call_inputs, actual_composite);
        ((layerNode *)*iter)->fp_composite = call; 
        comCallList.push_back(call);
    }
    cout<<"unfold forward propogation"<<endl;
    // 展开反向传播composite, 最后一层的composite的输入为实际预测和期望预测的输入流 也即temp_stream和 与y_stream
    temp_stream->push_back(y_stream->front());
    // 额外创建一个新的节点计算dL/dy
    // 暂时搁置
    /*for (auto iter = compositeCall_list.rbegin(); iter != compositeCall_list.rend(); iter++) {
        list<Node *> *call_inputs, *call_outputs;
        string name = "B" + ((layerNode *)*iter)->layerName + to_string(((layerNode *)*iter)->level);
        string namePrefix = "B" + streamName + "_" + ((layerNode *)*iter)->layerName + to_string(((layerNode *)*iter)->level) + "_";
        string tempName =  namePrefix + ((layerNode *)*iter)->prevLayer->layerName + to_string(((layerNode *)*iter)->prevLayer->level);
        idNode *id = new idNode(tempName);
        call_inputs = new list<Node *>({temp_stream->front(), temp_stream->back()});
        call_outputs = new list<Node *>({id});
        temp_stream->clear();
        temp_stream->push_back(call_outputs->front());
        // 上面的未实现,这里就会有问题
        temp_stream->push_back(((layerNode *)*iter)->fp_composite->outputs->back());
        // 构造实际的正向传播composite  ???未完成
        // compositeNode* actual_composite = makeBackComposite((layerNode *) *iter,call_inputs, call_outputs);
        // compositeCallNode *call = new compositeCallNode(call_outputs, name, NULL, call_inputs, actual_composite);
        // ((layerNode *) *iter)-> bp_composite = call; 
        // comCallList.push_back(call);
    }*/
    // cout<<"unfold back propogation"<<endl;
    // ...
    // 生成squential composite
    // 将声明的数据流加入
    comp_stmts->push_back(streamDecl);
    // 遍历计算节点, 依次加入
    for (auto nd : comCallList)
        comp_stmts->push_back(nd);
    body = new compBodyNode(NULL, comp_stmts);
    squential = new compositeNode(head, body);
    // ++num; // ???
    compositeCall_list.clear();
    return squential;
}
compositeNode* UnfoldComposite::makeForwardComposite(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs) {
    ComInOutNode *inout = new ComInOutNode(inputs, outputs);
    string comName = MakeCompositeName("F" + layer->layerName + to_string(layer-> level));
    compHeadNode *compHead = new compHeadNode(comName, inout);
    compBodyNode *compBody = NULL;
    list<Node *> *comp_stmt_list = new list<Node *>();
    Node *layerOperator = NULL;
    // 声明数据流, 修改makeDenseOperator的传入参数
    // operatorNode *activationOperator = NULL;
    if (layer->layerName == "dense") layerOperator = makeDenseOperator(layer, inputs, outputs);
    Node* layerExp = new binopNode((expNode*)outputs,"=",(expNode*)layerOperator) ;
    comp_stmt_list->push_back(layerExp);
    compBody = new compBodyNode(NULL, comp_stmt_list);
    compositeNode *comp = new compositeNode(compHead, compBody);
    cout << "makeForwardComposite"<<endl;
    return comp;
 }
compositeNode* UnfoldComposite::makeBackComposite(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs) {
    ComInOutNode *inout = new ComInOutNode(inputs, outputs);
    compHeadNode *compHead = new compHeadNode("F" + layer->layerName + to_string(layer-> level), inout);
    compBodyNode *compBody = NULL;
    compositeNode *comp = new compositeNode(compHead, compBody);
    cout << "makeBackComposite"<<endl;
    return comp;
}
operatorNode* UnfoldComposite::makeDenseOperator(layerNode *layer, list<Node *> *inputs, list<Node *> *outputs) {
    string operName = "dense";
    operBodyNode *body = NULL;
    Node *init = NULL, *work = NULL;
    windowNode *window = NULL;
    Node *row = NULL, *col = NULL;
    list<Node *> *win_stmt = new list<Node *>();
    // In0 sliding(row, row);
    if (layer->prevLayer != NULL) {
        row = layer->prevLayer->arg_list->front();
    } else {
        row = globalSquential->arg_list->front();
    }
    col = layer->arg_list->front();
    
    // 添加输入窗口
    auto iter = inputs->begin();
    slidingNode *slid = new slidingNode(new list<Node *>({row, row}));
    winStmtNode *win1 = new winStmtNode(((idNode *)*iter)->name, slid);
    win_stmt->push_back(win1);
    // 添加输出窗口
    iter = outputs->begin();
    tumblingNode *tumb = new tumblingNode(new list<Node *>({col}));
    winStmtNode *win2 = new winStmtNode(((idNode *)*iter)->name, tumb);
    win_stmt->push_back(win2);
    window = new windowNode(win_stmt);
    init = MakeDenseInit(layer, inputs, outputs);
    work = MakeDenseWork(layer, inputs, outputs);
    // statement.list, operator.selfdefine.body.init, operator.selfdefine.body.work, operator.selfdefine.body.window.list
    body = new operBodyNode(NULL, init, work, window);
    return new operatorNode(outputs, operName, inputs, body);
}
operatorNode* UnfoldComposite::makeActivationOperator(layerNode* layer, list<Node *> *inputs, list<Node *> *outputs) {
    string operName;
    operBodyNode *operBody;
    return new operatorNode(outputs, operName, inputs, operBody);
}
// 全局变量初始化 下一步???
Node* UnfoldComposite::MakeDenseInit(layerNode *layer, list<Node *> *inputs, list<Node*> *outputs) {
    list<Node *> *stmts = new list<Node *>();
    Node *init =  NULL;
    // 获得当前层的权值id
    string weightName = "_weight_" + to_string(layer-> level);
    Node*  weightId = new idNode(weightName);
    ((idNode *)weightId)->isArray = 1;
    // 利用for循环初始化
    constantNode *const_zero = new constantNode("integer", (long long)0);
    constantNode *const_i = new constantNode("integer", (long long)0);
    constantNode *const_j = new constantNode("integer", (long long)0);
    initNode *init_i = new initNode(const_i);
    initNode *init_j = new initNode(const_j);
    idNode *id_i = new idNode("i"), *id_j = new idNode("j");
    id_i->init = init_i;
    id_j->init = init_j;
    primNode *prim = new primNode("int");
    declareNode *declI = new declareNode(prim, id_i), *declJ = new declareNode(prim, id_j);
    stmts->push_back(declI);
    stmts->push_back(declJ);
    Node *init1 = NULL, *cond1 = NULL, *next_i = NULL, *stmt1 = NULL, *forNode1;
    Node *init2 = NULL, *cond2 = NULL, *next_j = NULL, *stmt2 = NULL, *forNode2;
    init1 = new binopNode((expNode *)id_i, "=", (expNode *)const_zero);
    if (layer->prevLayer == NULL) {
        cond1 = new binopNode((expNode *)id_i, "<", (expNode *)(globalSquential->arg_list->front()));
    } else {
        cond1 = new binopNode((expNode *)id_i, "<", (expNode *)(layer->prevLayer->arg_list->front()));
    }
    next_i = new unaryNode("POSTINC", (expNode *)id_i);
    init2 = new binopNode((expNode *)id_j, "=", (expNode *)const_zero);
    cond2 = new binopNode((expNode *)id_j, "<", (expNode *)(layer->arg_list->front()));
    next_j = new unaryNode("POSTINC", (expNode *)id_j);
    Node *weightArrDec = new arrayNode((expNode *)id_i);
    (static_cast<arrayNode *> (weightArrDec))->arg_list.push_back((expNode *)id_j);
    (static_cast<idNode*>(weightId))->arg_list = (static_cast<arrayNode*>(weightArrDec))->arg_list;
    stmt2 = new binopNode((expNode *)weightId, "=", (expNode *)const_zero);
    forNode2 = new forNode(init2, (expNode *)cond2, (expNode *)next_j, stmt2);
    stmt1 = forNode2;
    forNode1 = new forNode(init1, (expNode *)cond1, (expNode *)next_i, stmt1);
    stmts->push_back(forNode1);
    init  = new blockNode(stmts);

    return init;
}
// 计算Dense
Node* UnfoldComposite::MakeDenseWork(layerNode *layer, list<Node *> *inputs, list<Node*> *outputs) {
    list<Node *> *stmts = new list<Node *>();
    Node *work =  NULL;
    string weightName = "_weight_" + to_string(layer-> level);
    Node*  weightId = new idNode(weightName);
    ((idNode *)weightId)->isArray = 1;

    // int i = 0, j = 0;
    constantNode *const_zero = new  constantNode("integer", (long long)0);
    constantNode *const_i = new constantNode("integer", (long long)0);
    constantNode *const_j = new constantNode("integer", (long long)0);
    initNode *init_i = new initNode(const_i);
    initNode *init_j = new initNode(const_j);
    idNode *id_i = new idNode("i"), *id_j = new idNode("j");
    id_i->init = init_i;
    id_j->init = init_j;
    primNode *prim = new primNode("int");
    declareNode *declI = new declareNode(prim, id_i), *declJ = new declareNode(prim, id_j);
    stmts->push_back(declI);
    stmts->push_back(declJ);

    idNode *id_temp = new idNode("temp");
    id_temp->init = new initNode(const_zero);
    declareNode *declTemp = new declareNode(prim, id_temp);
    stmts->push_back(declTemp);

    Node *forNode1 = NULL, *init1 = NULL, *cond1 = NULL, *next_j = NULL, *stmt1 = NULL;
    init1 = new binopNode((expNode *)id_j, "=", (expNode *)const_zero);
    cond1 = new binopNode((expNode *)id_j, "<", (expNode *)(layer->arg_list->front()));
    next_j = new unaryNode("POSTINC", (expNode *)id_j);
    list<Node *> *stmts1 = new list<Node *>();
    stmts1->push_back(new binopNode((expNode *)id_temp, "=", (expNode *)const_zero));

    Node *forNode2 = NULL, *init2 = NULL, *cond2 = NULL, *next_i = NULL, *stmt2 = NULL, *block2 = NULL;
    init2 = new binopNode((expNode *)id_i, "=", (expNode *)const_zero);
    cond2 = new binopNode((expNode *)id_i, "<", (expNode *)(* (layer->arg_list->begin()++)));
    next_i = new unaryNode("POSTINC", (expNode *)id_i);
    list<Node *> *stmts2 = new list<Node *>();
    // 取得_weight_level[i][j]
    Node *weightArrDec = NULL;
    weightArrDec =  new arrayNode((expNode *)(id_i));
    (static_cast<arrayNode *> (weightArrDec))->arg_list.push_back((expNode *)(id_j));
    (static_cast<idNode*>(weightId))->arg_list = (static_cast<arrayNode*>(weightArrDec))->arg_list;
    // 取得In1[i].x
    idNode* x = new idNode("x");
    idNode* input1 = new idNode(static_cast<idNode *>(inputs->front())->name);
    input1->isArray = 1;
    input1->arg_list.push_back(id_i);
    Node* In1 = new binopNode((expNode *)input1, ".", (expNode *)x);
    // temp+=In1[i].x*_weight_level[i][j];
    Node* midRes = new binopNode((expNode *)id_temp, "+=", (expNode *)(new binopNode((expNode *)In1, "*", (expNode *)weightId)));
    stmts2->push_back(midRes);
    block2 = new blockNode(stmts2);
    // for(i = 0; i < arg:row; i++) {temp+=In0[i].x*weight1[i][j];}
    forNode2 = new forNode(init2, (expNode *)cond2, (expNode *)next_i, block2);
    stmts1->push_back(forNode2);
    work  = new blockNode(stmts);
    // 取得Out1[j].x
    idNode* output1 = new idNode(static_cast<idNode *>(outputs->front())->name);
    output1->isArray = 1;
    output1->arg_list.push_back(id_j);
    Node* Out1 = new binopNode((expNode *)output1, ".", (expNode *)x);
    Node* res = new binopNode((expNode *)Out1, "=", (expNode *)id_temp);
    stmts1->push_back(res);
    Node* block1 = new blockNode(stmts1);
    forNode1 = new forNode(init1, (expNode *)cond1, (expNode *)next_j, block1);
    stmts->push_back(forNode1);
    
    work = new blockNode(stmts);
    return work;
}