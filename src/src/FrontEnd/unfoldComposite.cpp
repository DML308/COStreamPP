#include "unfoldComposite.h"
#include "compositeFlow.h"
#include "symboltableGenerate.h"
#include <string.h>

extern SymbolTable S;
extern SymbolTable *runningTop;

extern list<Node*> *Program;
vector<Node *> compositeCall_list; //存储splitjoin/pipeline中的compositeCall调用

Node *UnfoldComposite::MakeRoundrobinWork(list<Node *> *inputs, list<constantNode *> *arguments, list<Node *> *outputs,int style)
{
    list<Node *> *stmts = new list<Node *>();
    Node *work = NULL, *for_node = NULL;
    Node *init = NULL, *cond = NULL, *next_i = NULL, *next_j = NULL, *stmt = NULL;
    Node *input = inputs->front();
    assert(input->type == Id);
    constantNode *const_zero = new constantNode("long long", (long long)0);
    constantNode *const_i = new constantNode("long long", (long long)0);
    constantNode *const_j = new constantNode("long long", (long long)0);
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

        if(style == 1){
            
        }
        if(style == 0){
            binopNode *reset_j = new binopNode((expNode *)id_j,"=",(expNode *)const_zero);
            stmts->push_back(reset_j);
        }
        pos++;
    }
    work = new blockNode(stmts);
    return work;
}

Node *UnfoldComposite::MakeJoinWork(list<Node *> *inputs, list<constantNode *> *arguments, list<Node *> *outputs)
{
    list<Node *> *stmts = new list<Node *>();
    Node *work = NULL, *for_node = NULL;
    Node *init = NULL, *cond = NULL, *next_i = NULL, *next_j = NULL, *stmt = NULL;
    Node *output = outputs->front();
    assert(output->type == Id);
    constantNode *const_zero = new constantNode("long long", (long long)0);
    constantNode *const_i = new constantNode("long long", (long long)0);
    constantNode *const_j = new constantNode("long long", (long long)0);
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
/*style
    1 表示 roundrobin 分割数据流
    0 表示 duplicate 复制数据流
*/
compositeCallNode *UnfoldComposite::MakeSplitOperator(Node *input, list<Node *> *arguments, int style)
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
    list<constantNode *> *arguments_value = new list<constantNode *>(); // 存储常量传播后的值
    assert(input->type == Id);

    string param_name = ((idNode *)input)->name;
    string input_name = param_name; //runningTop->LookUpStreamSymbol(param_name)->id->name; //查表找到真实的输入流
    idNode * real_input = new idNode(input_name);

    int len = compositeCall_list.size();
    static int number = 0;
    long long sum = 0;
    inputs->push_back(real_input);
    //arguments可能为NULL，就分配一块内存
    if (arguments == NULL)
        arguments = new list<Node *>();
    assert(arguments->size() == 0 || arguments->size() == 1 || arguments->size() == len);
    
    for (auto it : *(arguments))
    {
        Constant *value = getOperationResult(it);

        constantNode *copy;  //将整型都转化为 long long
        if(value->type.compare("int") == 0){
            copy = new constantNode("long long",(long long)value->ival);
            
        }
        if(value->type.compare("long") == 0){
            copy = new constantNode("long long",(long long)value->lval);
        }
        if(value->type.compare("long long") == 0){
            copy = new constantNode("long long",value->llval);
        }
        if(value->type.compare("double") == 0){
            cout<<"splitjoin 指定每一个数据流大小只能为整型,现在是double";
            exit(-1);
        }
        if(value->type.compare("float") == 0){
            cout<<"splitjoin 指定每一个数据流大小只能为整型,现在是float";
            exit(-1);
        }
        if(value->type.compare("string") == 0){
            cout<<"splitjoin 指定每一个数据流大小只能为整型,现在是string";
            exit(-1);
        }
         
        arguments_value->push_back(copy);
    }

    /*若split roundroubin()参数为空 默认赋值一个数据大小 1*/
    if (arguments_value->size() == 0)
    {
        //constantNode *nd = new constantNode("long long", (long long)1);
        constantNode *constant = new constantNode("long long",(long long)1);
        //arguments->push_back(nd);
        arguments_value->push_back(constant);
    }
    constantNode *arg = arguments_value->front();

    if (arguments_value->size() == 1)
    {
        for (int i = 1; i < len; ++i)
            arguments_value->push_back(arg);
            //arguments->push_back(arg);
    }
    //cout << "arguments->size()= "<<arguments->size() << endl;
    int cnt = 0;
    //构建输出窗口
    for (auto it : *(arguments_value))
    {
        sum += it->llval;
        string tempName = streamName[style] + to_string(number) + "_" + to_string(cnt);
        cnt++;
        idNode *id = new idNode(tempName);
        outputs->push_back(id);
        constantNode *argument = new constantNode("long long",it->llval); //将得到的常量值重新封装为语法树的一个节点
        /* 此处可以抽出写为函数 */
        if (style == 1)//roundrobin 分割
        {
            tumblingNode *tum = new tumblingNode(new list<Node *>({argument}));
            winStmtNode *win = new winStmtNode(tempName, tum);
            win_stmt->push_back(win);
        }
        else if (style == 0) //duplicate 重复
        {
            slidingNode *slid = new slidingNode(new list<Node *>({argument, argument}));
            winStmtNode *win = new winStmtNode(tempName, slid);
            win_stmt->push_back(win);
        }
    }
    //添加split节点输入窗口
    if (style == 0)
    {
        sum = arg->llval;
    }
    constantNode *constNode = new constantNode("long long", sum);
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
    work = MakeRoundrobinWork(inputs, arguments_value, outputs,style);
    window = new windowNode(win_stmt);
    body = new operBodyNode(NULL, NULL, work, window);
    res = new operatorNode(outputs, operName[style], inputs, body);
    number++;
    //cout << "-----------------split end---------------------" << endl;

    string compName = res->operName;
    list<Node *> *input_list = new list<Node *>();
    list<Node *> *output_list = new list<Node *>();

    inOutdeclNode *type_input = (runningTop->getStreamTable().begin())->second;
    for(auto it : *res->inputs){
        string name = ((idNode *)it)->name;
        idNode *copy_input_id = new idNode(name);
        inOutdeclNode *copy_input = new inOutdeclNode(type_input->strType,copy_input_id);
        input_list->push_back(copy_input);
    }
    for(auto it : *res->outputs){
        string name = ((idNode *)it)->name;
        idNode *copy_output_id = new idNode(name);
        inOutdeclNode *copy_output = new inOutdeclNode(type_input->strType,copy_output_id);
        output_list->push_back(copy_output);
    }

    ComInOutNode *split_comp_inout = new ComInOutNode(input_list,output_list);
    compHeadNode *split_comp_head = new compHeadNode(compName, split_comp_inout);
    list<Node *> *split_comp_stmts = new list<Node *>();
    split_comp_stmts->push_back(res);
    compBodyNode *split_comp_body = new compBodyNode(NULL,split_comp_stmts);
    compositeNode *split_comp = new compositeNode(split_comp_head,split_comp_body);

    split_comp->isUnfoldComposite = true;
    split_comp->splitType = style ? RoundRobin : Duplicate;

    compositeCallNode *split_call = new compositeCallNode(res->outputs,compName, NULL, res->inputs, split_comp);
    split_call->isOriginal = false;

    //cout << "-----------------join end---------------------" << endl;
    return split_call;
}

compositeCallNode *UnfoldComposite::MakeJoinOperator(Node *output, list<Node *> *inputs, list<Node *> *arguments)
{
    Node *work = NULL;
    operatorNode *res = NULL;
    windowNode *window = NULL;
    operBodyNode *body = NULL;
    string operName = "join";

    assert(output->type == Id);
    string param_name = ((idNode *)output)->name;
    string output_name = param_name;//runningTop->LookUpStreamSymbol(param_name)->id->name; //查表找到真实的输入流
    idNode *real_output = new idNode(output_name);

    int len = compositeCall_list.size();
    long long sum = 0;
    list<Node *> *outputs = new list<Node *>();
    list<Node *> *win_stmt = new list<Node *>();
    list<constantNode *> *arguments_value = new list<constantNode *>(); // 存储常量传播后的值
    //当arguments为空，即分配内存
    if (arguments == NULL)
        arguments = new list<Node *>();
    assert(arguments->size() == 0 || arguments->size() == 1 || arguments->size() == len);
    
    outputs->push_back(real_output);

    for (auto it : *(arguments))
    {
        Constant *value = getOperationResult(it);
        constantNode *copy;  //将整型都转化为 long long
        if(value->type.compare("int") == 0){
            copy = new constantNode("long long",(long long)value->ival);
        }
        if(value->type.compare("long") == 0){
            copy = new constantNode("long long",(long long)value->lval);
        }
        if(value->type.compare("long long") == 0){
            copy = new constantNode("long long",value->llval);
        }
        if(value->type.compare("double") == 0){
            cout<<"splitjoin 指定每一个数据流大小只能为整型,现在是double";
            exit(-1);
        }
        if(value->type.compare("float") == 0){
            cout<<"splitjoin 指定每一个数据流大小只能为整型,现在是float";
            exit(-1);
        }
        if(value->type.compare("string") == 0){
            cout<<"splitjoin 指定每一个数据流大小只能为整型,现在是string";
            exit(-1);
        }
        arguments_value->push_back(copy);
    }

    //Constant *constantIntOne = new Constant("long long", (long long)1);

    /*若split roundroubin()参数为空 默认赋值一个数据大小*/
    if (arguments_value->size() == 0)
    {
        //constantNode *nd = new constantNode("long long", (long long)1);
        constantNode *constant = new constantNode("long long",(long long)1);
        //arguments->push_back(nd);
        arguments_value->push_back(constant);
    }
    constantNode *arg = arguments_value->front();

    if (arguments_value->size() == 1)
    {
        for (int i = 1; i < len; ++i)
            arguments_value->push_back(arg);
            //arguments->push_back(arg);
    }
    assert(inputs->size() == arguments_value->size());
    auto iter = inputs->begin();

    //cout<<"iter->type="<<(*iter)->type<<endl;
    for (auto it : *(arguments_value))
    {
        sum += it->llval;
        //cout << ((idNode *)*iter)->name << endl;
        constantNode *argument = new constantNode("long long",it->llval); //将得到的常量值重新封装为语法树的一个节点

        slidingNode *slid = new slidingNode(new list<Node *>({argument, argument}));
        winStmtNode *win = new winStmtNode(((idNode *)*iter)->name, slid);
        win_stmt->push_back(win);
        iter++;
    }
    /*添加join节点输出窗口*/
    constantNode *constNode = new constantNode("long long", sum);
    tumblingNode *tum = new tumblingNode(new list<Node *>({constNode}));
    winStmtNode *win = new winStmtNode(output_name, tum);
    win_stmt->push_back(win);
    /*end*/
    work = MakeJoinWork(inputs, arguments_value, outputs);
    window = new windowNode(win_stmt);
    body = new operBodyNode(NULL, NULL, work, window);
    res = new operatorNode(outputs, operName, inputs, body);

    string compName = res->operName;
    list<Node *> *input_list = new list<Node *>();
    list<Node *> *output_list = new list<Node *>();
    inOutdeclNode *type_input = (runningTop->getStreamTable().begin())->second;
    for(auto it : *res->inputs){
        string name = ((idNode *)it)->name;
        idNode *copy_input_id = new idNode(name);
        inOutdeclNode *copy_input = new inOutdeclNode(type_input->strType,copy_input_id);
        input_list->push_back(copy_input);
    }
    for(auto it : *res->outputs){
        string name = ((idNode *)it)->name;
        idNode *copy_output_id = new idNode(name);
        inOutdeclNode *copy_output = new inOutdeclNode(type_input->strType,copy_output_id);
        output_list->push_back(copy_output);
    }

    ComInOutNode *join_comp_inout = new ComInOutNode(input_list,output_list);
    compHeadNode *join_comp_head = new compHeadNode(compName, join_comp_inout);
    list<Node *> *join_comp_stmts = new list<Node *>();
    join_comp_stmts->push_back(res);
    compBodyNode *join_comp_body = new compBodyNode(NULL,join_comp_stmts);
    compositeNode *join_comp = new compositeNode(join_comp_head,join_comp_body);

    join_comp->isUnfoldComposite = true;
    join_comp->splitType = RoundRobin;

    compositeCallNode *join_call = new compositeCallNode(res->outputs,compName, NULL, res->inputs, join_comp);
    join_call->isOriginal = false;

    //cout << "-----------------join end---------------------" << endl;
    return join_call;
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
    list<Node *> *comCallList = new list<Node *>();
    compositeNode *roundrobin = NULL;
    compositeCallNode *splitOperator = NULL, *joinOperator = NULL;
    list<Node *> *tempList = new list<Node *>();
    /* arg_list表示split roundrobin(size);的size参数列表 */
    /* join_arg_list join roundrobin(size);的size参数列表 */
    /* 二者都需要常量传播 */
    list<Node *> *arg_list = ((roundrobinNode *)node->split->dup_round)->arg_list;
    list<Node *> *join_arg_list = ((roundrobinNode *)node->join->rdb)->arg_list;
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
    splitOperator = MakeSplitOperator(inputs_split->front(), arg_list, 1); //split 大小 常量传播
    // 将 split operator 封装成 compositecall
    
    /*2.构建compositeCall，与split,join节点的输入输出流关联 */
    tempList = splitOperator->outputs;
    auto iter = tempList->begin();
    int cnt = 0;

    for (auto it : compositeCall_list)
    {
        if (it->type == CompositeCall)
        {
            //cout << "--------------------------------------" << it << "-------------------------------" << endl;
            // if (it->type == SplitJoin || it->type == Pipeline)
            // {
            //     comCallList->push_back(it);
            // }
            //cout << "--------------------" << endl;
            string tempName = streamName + to_string(number1) + "_" + (to_string(cnt));
            idNode *id = new idNode(tempName);
            //compositeCall的输出流是join节点的输入流
            inputs_join->push_back(id);
            list<Node *> *call_outputs = new list<Node *>({id});
            //compositeCall的输入流
            list<Node *> *call_inputs = new list<Node *>({*iter});
            /* 1.若为compositecall 将compositeCall替换为实际的composite */
            /*修改composite节点的输入流,输出流*/
            string name = (((compositeCallNode *)it)->compName);
            CompositeSymbol *comp_symbol = S.LookupCompositeSymbol(name);
            compositeNode *comp = comp_symbol->composite;
            comp_symbol->count++;
            assert(comp != NULL);
            //compositeNode *actual_composite = compositeCallStreamReplace(comp, call_inputs, call_outputs);
            
            //传参
            /*
            list<Node *> *copy_stream_List = NULL;
            if(((compositeCallNode *)it)->stream_List){
                list<Node *> *stream_List = ((compositeCallNode *)it)->stream_List;
                copy_stream_List = new list<Node*>(stream_List->size());
                std::copy(stream_List->begin(),stream_List->end(),copy_stream_List->begin());
            }
            else{
                copy_stream_List = new list<Node *>();
            }*/
            list<Node *> *stream_List = ((compositeCallNode *)it)->stream_List;
            compositeCallNode *call = new compositeCallNode(call_outputs, tempName, stream_List, call_inputs, comp,comp_symbol->count,*(it)->loc);
            call->isOriginal = false;
            //cout<<"compName= "<<tempName<<endl;
            call->scope = runningTop;
            comCallList->push_back(call);
        }
        /* 2.todo 若为splitjoin或者pipeline结构，赋予其输入和输出流 */
        else if (it->type == SplitJoin)
        {
           
                string split_join_name = MakeCompositeName("splitjoin");
                string tempName = streamName + to_string(number1) + "_" + (to_string(cnt));
                idNode *id = new idNode(tempName);
                
                // splitjoin 的输出为上一层join节点的输入
                inputs_join->push_back(id);

                //compositeCall的输入流
                list<Node *> *call_inputs = new list<Node *>({*iter});
                list<Node *> *call_outputs = new list<Node *>({id});
                
                //
                list<Node *> *input_list = new list<Node *>();
                list<Node *> *output_list = new list<Node *>();

                list<Node *> *param_input_list = new list<Node *>();
                list<Node *> *param_output_list = new list<Node *>();

                inOutdeclNode *type_input = (runningTop->getStreamTable().begin())->second;
                for(auto it : *call_inputs){ //单输入输出
                    //string name = ((idNode *)it)->name;
                    idNode *copy_input_id = new idNode("In");
                    inOutdeclNode *copy_input = new inOutdeclNode(type_input->strType,copy_input_id);
                    input_list->push_back(copy_input);
                    param_input_list->push_back(copy_input_id);
                }
                for(auto it : *call_outputs){
                    //string name = ((idNode *)it)->name;
                    idNode *copy_output_id = new idNode("Out");
                    inOutdeclNode *copy_output = new inOutdeclNode(type_input->strType,copy_output_id);
                    output_list->push_back(copy_output);
                    param_output_list->push_back(copy_output_id);
                }
                
                ComInOutNode *com_splitjoin_inout = new ComInOutNode(input_list,output_list);
                compHeadNode *com_splitjoin_head = new compHeadNode(split_join_name,com_splitjoin_inout);

                list<Node*> *com_splitjoin_body_list = new list<Node *>();
            
                //改造splitjoin 输入输出
                splitjoinNode *sp_node = (splitjoinNode *)(it);

                /*if(sp_node->inputs == NULL || sp_node->outputs == NULL){

                }*/
                sp_node->inputs = param_input_list;
                sp_node->outputs = param_output_list;
                
                com_splitjoin_body_list->push_back(it);
                compBodyNode *com_splitjoin_body = new compBodyNode(NULL,com_splitjoin_body_list);
                compositeNode *com_splitjoin = new compositeNode(com_splitjoin_head,com_splitjoin_body);
                

                //compositeNode* unfold_splitjoin_node = UnfoldSplitJoin((splitjoinNode *)*it);
                //第三个为传参 param,这里是空
                compositeCallNode *call = new compositeCallNode(call_outputs, split_join_name, NULL, call_inputs, com_splitjoin,*(it)->loc);
                call->isOriginal = false;
                call->scope = runningTop;
                comCallList->push_back(call);

            

        }

        else if (it->type == Pipeline)
        {
            string pipeline_name = MakeCompositeName("pipeline");
            string tempName = streamName + to_string(number1) + "_" + (to_string(cnt));
            idNode *id = new idNode(tempName);
            
            //compositeCall的输出流是join节点的输入流
            inputs_join->push_back(id);
            list<Node *> *call_outputs = new list<Node *>({id});
            //compositeCall的输入流
            list<Node *> *call_inputs = new list<Node *>({*iter});

            list<Node *> *input_list = new list<Node *>();
            list<Node *> *output_list = new list<Node *>();

            list<Node *> *param_input_list = new list<Node *>();
            list<Node *> *param_output_list = new list<Node *>();

                inOutdeclNode *type_input = (runningTop->getStreamTable().begin())->second;
                for(auto it : *call_inputs){ //单输入输出
                    //string name = ((idNode *)it)->name;
                    idNode *copy_input_id = new idNode("In");
                    inOutdeclNode *copy_input = new inOutdeclNode(type_input->strType,copy_input_id);
                    input_list->push_back(copy_input);
                    param_input_list->push_back(copy_input_id);
                }
                for(auto it : *call_outputs){
                    //string name = ((idNode *)it)->name;
                    idNode *copy_output_id = new idNode("Out");
                    inOutdeclNode *copy_output = new inOutdeclNode(type_input->strType,copy_output_id);
                    output_list->push_back(copy_output);
                    param_output_list->push_back(copy_output_id);
                }
                
                ComInOutNode *com_pipeline_inout = new ComInOutNode(input_list,output_list);
                compHeadNode *com_pipeline_head = new compHeadNode(pipeline_name,com_pipeline_inout);

                list<Node*> *com_pipeline_body_list = new list<Node *>();
            
                //改造 pipeline 输入输出
                pipelineNode *pipeline_node = (pipelineNode *)(it);

                //指定嵌套结构中pipeline的输入和输出流
                pipeline_node->inputs = param_input_list;
                pipeline_node->outputs = param_output_list;
                
                com_pipeline_body_list->push_back(it);
                compBodyNode *com_pipeline_body = new compBodyNode(NULL,com_pipeline_body_list);
                compositeNode *com_pipeline = new compositeNode(com_pipeline_head,com_pipeline_body);
                //todo composite 复用

                //compositeNode* unfold_splitjoin_node = UnfoldSplitJoin((splitjoinNode *)*it);
                //第三个为传参 param,这里是空
                compositeCallNode *call = new compositeCallNode(call_outputs, pipeline_name, NULL, call_inputs, com_pipeline,*(it)->loc);
                call->isOriginal = false;
                call->scope = runningTop;
                comCallList->push_back(call);

            
        }

        iter++;
        cnt++;
    }

    /*3.构建joinOperator，与compositeCall的输入输出流关联 */
    joinOperator = MakeJoinOperator(outputs->front(), inputs_join, join_arg_list); // //join 大小 常量传播
   
    comp_stmt_List->push_back(splitOperator);
    for (auto it : *comCallList)
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
    compositeCallNode *splitOperator = NULL, *joinOperator = NULL;
    /* arg_list表示split roundrobin(size);的size参数列表 */
    expNode *exp = ((duplicateNode *)(node->split->dup_round))->exp;
    list<Node *> *arg_list = new list<Node*>();
    arg_list->push_back(exp);
    
    list<Node *> *inputs_split = node->inputs;
    list<Node *> *outputs = node->outputs;
    list<Node *> *inputs_join = new list<Node *>();
    list<Node *> *call_outputs = NULL;
    list<Node *> *comCallList = new list<Node *>();
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
        

        if (it->type == CompositeCall)
        {
            //cout << "--------------------------------------" << it << "-------------------------------" << endl;
            // if (it->type == SplitJoin || it->type == Pipeline)
            // {
            //     comCallList->push_back(it);
            // }
            //cout << "--------------------" << endl;
            string tempName = streamName + to_string(number2) + "_" + (to_string(cnt));
            idNode *id = new idNode(tempName);
            //compositeCall的输出流是join节点的输入流
            inputs_join->push_back(id);
            list<Node *> *call_outputs = new list<Node *>({id});
        //compositeCall的输入流
            list<Node *> *call_inputs = new list<Node *>({*iter});
            /* 1.若为compositecall 将compositeCall替换为实际的composite */
            /*修改composite节点的输入流,输出流*/
            string name = (((compositeCallNode *)it)->compName);
            CompositeSymbol *comp_symbol = S.LookupCompositeSymbol(name);
            compositeNode *comp = comp_symbol->composite;
            comp_symbol->count++;
            assert(comp != NULL);
            //compositeNode *actual_composite = compositeCallStreamReplace(comp, call_inputs, call_outputs);

            //传参
            /*list<Node *> *copy_stream_List = NULL;
            if(((compositeCallNode *)it)->stream_List){
                list<Node *> *stream_List = ((compositeCallNode *)it)->stream_List;
                copy_stream_List = new list<Node*>(stream_List->size());
                std::copy(stream_List->begin(),stream_List->end(),copy_stream_List->begin());
            }
            else{
                copy_stream_List = new list<Node *>();
            }*/
            list<Node *> *stream_List = ((compositeCallNode *)it)->stream_List;
            compositeCallNode *call = new compositeCallNode(call_outputs, tempName, stream_List, call_inputs, comp,comp_symbol->count,*(it)->loc);
            call->scope = runningTop;
            call->isOriginal = false;
            //cout<<"compName= "<<tempName<<endl;
            comCallList->push_back(call);
        }
        /* 2.todo 若为splitjoin或者pipeline结构，赋予其输入和输出流 */
        else if (it->type == SplitJoin)
        {
           
                string split_join_name = MakeCompositeName("splitjoin");
                string tempName = streamName + to_string(number2) + "_" + (to_string(cnt));
                idNode *id = new idNode(tempName);
                
                // splitjoin 的输出为上一层join节点的输入
                inputs_join->push_back(id);

                //compositeCall的输入流
                list<Node *> *call_inputs = new list<Node *>({*iter});
                list<Node *> *call_outputs = new list<Node *>({id});
                
                //
                list<Node *> *input_list = new list<Node *>();
                list<Node *> *output_list = new list<Node *>();

                list<Node *> *param_input_list = new list<Node *>();
                list<Node *> *param_output_list = new list<Node *>();

                inOutdeclNode *type_input = (runningTop->getStreamTable().begin())->second;
                for(auto it : *call_inputs){ //单输入输出
                    //string name = ((idNode *)it)->name;
                    idNode *copy_input_id = new idNode("In");
                    inOutdeclNode *copy_input = new inOutdeclNode(type_input->strType,copy_input_id);
                    input_list->push_back(copy_input);
                    param_input_list->push_back(copy_input_id);
                }
                for(auto it : *call_outputs){
                    //string name = ((idNode *)it)->name;
                    idNode *copy_output_id = new idNode("Out");
                    inOutdeclNode *copy_output = new inOutdeclNode(type_input->strType,copy_output_id);
                    output_list->push_back(copy_output);
                    param_output_list->push_back(copy_output_id);
                }
                
                ComInOutNode *com_splitjoin_inout = new ComInOutNode(input_list,output_list);
                compHeadNode *com_splitjoin_head = new compHeadNode(split_join_name,com_splitjoin_inout);

                list<Node*> *com_splitjoin_body_list = new list<Node *>();
            
                //改造splitjoin 输入输出
                splitjoinNode *sp_node = (splitjoinNode *)(it);

                /*if(sp_node->inputs == NULL || sp_node->outputs == NULL){

                }*/
                sp_node->inputs = param_input_list;
                sp_node->outputs = param_output_list;
                
                com_splitjoin_body_list->push_back(it);
                compBodyNode *com_splitjoin_body = new compBodyNode(NULL,com_splitjoin_body_list);
                compositeNode *com_splitjoin = new compositeNode(com_splitjoin_head,com_splitjoin_body);
                

                //compositeNode* unfold_splitjoin_node = UnfoldSplitJoin((splitjoinNode *)*it);
                //第三个为传参 param,这里是空
                compositeCallNode *call = new compositeCallNode(call_outputs, split_join_name, NULL, call_inputs, com_splitjoin,*(it)->loc);
                call->isOriginal = false;
                call->scope = runningTop;
                comCallList->push_back(call);

        }

        else if (it->type == Pipeline)
        {
            string pipeline_name = MakeCompositeName("pipeline");
            string tempName = streamName + to_string(number2) + "_" + (to_string(cnt));
            idNode *id = new idNode(tempName);
            
            //compositeCall的输出流是join节点的输入流
            inputs_join->push_back(id);
            list<Node *> *call_outputs = new list<Node *>({id});
            //compositeCall的输入流
            list<Node *> *call_inputs = new list<Node *>({*iter});

            list<Node *> *input_list = new list<Node *>();
            list<Node *> *output_list = new list<Node *>();

            list<Node *> *param_input_list = new list<Node *>();
            list<Node *> *param_output_list = new list<Node *>();

                inOutdeclNode *type_input = (runningTop->getStreamTable().begin())->second;
                for(auto it : *call_inputs){ //单输入输出
                    //string name = ((idNode *)it)->name;
                    idNode *copy_input_id = new idNode("In");
                    inOutdeclNode *copy_input = new inOutdeclNode(type_input->strType,copy_input_id);
                    input_list->push_back(copy_input);
                    param_input_list->push_back(copy_input_id);
                }
                for(auto it : *call_outputs){
                    //string name = ((idNode *)it)->name;
                    idNode *copy_output_id = new idNode("Out");
                    inOutdeclNode *copy_output = new inOutdeclNode(type_input->strType,copy_output_id);
                    output_list->push_back(copy_output);
                    param_output_list->push_back(copy_output_id);
                }
                
                ComInOutNode *com_pipeline_inout = new ComInOutNode(input_list,output_list);
                compHeadNode *com_pipeline_head = new compHeadNode(pipeline_name,com_pipeline_inout);

                list<Node*> *com_pipeline_body_list = new list<Node *>();
            
                //改造 pipeline 输入输出
                pipelineNode *pipeline_node = (pipelineNode *)(it);

                //指定嵌套结构中pipeline的输入和输出流
                pipeline_node->inputs = param_input_list;
                pipeline_node->outputs = param_output_list;
                
                com_pipeline_body_list->push_back(it);
                compBodyNode *com_pipeline_body = new compBodyNode(NULL,com_pipeline_body_list);
                compositeNode *com_pipeline = new compositeNode(com_pipeline_head,com_pipeline_body);
                //todo composite 复用

                //compositeNode* unfold_splitjoin_node = UnfoldSplitJoin((splitjoinNode *)*it);
                //第三个为传参 param,这里是空
                compositeCallNode *call = new compositeCallNode(call_outputs, pipeline_name, NULL, call_inputs, com_pipeline,*(it)->loc);
                call->scope = runningTop;
                call->isOriginal = false;
                comCallList->push_back(call);
        }

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
    
    // input output 应该去查符号表中的stream
    ComInOutNode *inout = new ComInOutNode(inputs, outputs);
    compHeadNode *head = new compHeadNode(comName, inout);
    compBodyNode *body = NULL;
    assert(inputs != NULL && outputs != NULL);
    list<Node *> *comp_stmts = new list<Node *>();
    /* 构造pipeline内的节点的输入输出流 */
    int cnt = 0;
    idNode *input = (idNode *)(inputs->front());
    inOutdeclNode *real_input_stream = runningTop->LookUpStreamSymbol(input->name);
    idNode *real_input = new idNode(real_input_stream->id->name);
    list<Node *> *temp_stream = new list<Node *>({real_input});

    idNode *output = (idNode *)(outputs->front());
    inOutdeclNode *real_output_stream = runningTop->LookUpStreamSymbol(output->name);
    idNode *real_output = new idNode(real_output_stream->id->name);

    /* 遍历所有的compositeCall，生成对应输入输出流的compositecallnode */
    //cout<<"compositeCall_list= "<<compositeCall_list.size()<<endl;
    for (auto it = compositeCall_list.begin(); it != compositeCall_list.end(); ++it)
    {
        
        switch (((Node *)*it)->type)
        {
            case SplitJoin : {
                string split_join_name = MakeCompositeName("splitjoin");
                string tempName = streamName + to_string(num) + "_" + (to_string(cnt++)) + split_join_name;
                idNode *id = new idNode(tempName);
                
                list<Node *> *call_inputs = new list<Node *>({temp_stream->front()});
                list<Node *> *call_outputs = new list<Node *>({id});

                
                /*将现在的输出流作为下一次的输入流*/
                temp_stream->pop_back();
                temp_stream->push_back(id);

                if (it == --compositeCall_list.end())
                {
                    call_outputs->pop_back();
                    call_outputs->push_back(real_output);
                }


                list<Node *> *input_list = new list<Node *>();
                list<Node *> *output_list = new list<Node *>();

                list<Node *> *param_input_list = new list<Node *>();
                list<Node *> *param_output_list = new list<Node *>();

                inOutdeclNode *type_input = (runningTop->getStreamTable().begin())->second;
                for(auto it : *call_inputs){ //单输入输出
                    //string name = ((idNode *)it)->name;
                    idNode *copy_input_id = new idNode("In");
                    inOutdeclNode *copy_input = new inOutdeclNode(type_input->strType,copy_input_id);
                    input_list->push_back(copy_input);
                    param_input_list->push_back(copy_input_id);
                }
                for(auto it : *call_outputs){
                    //string name = ((idNode *)it)->name;
                    idNode *copy_output_id = new idNode("Out");
                    inOutdeclNode *copy_output = new inOutdeclNode(type_input->strType,copy_output_id);
                    output_list->push_back(copy_output);
                    param_output_list->push_back(copy_output_id);
                }
                
                ComInOutNode *com_splitjoin_inout = new ComInOutNode(input_list,output_list);
                compHeadNode *com_splitjoin_head = new compHeadNode(split_join_name,com_splitjoin_inout);

                list<Node*> *com_splitjoin_body_list = new list<Node *>();
            
                //改造splitjoin 输入输出
                splitjoinNode *sp_node = (splitjoinNode *)(*it);

                /*if(sp_node->inputs == NULL || sp_node->outputs == NULL){

                }*/
                sp_node->inputs = param_input_list;
                sp_node->outputs = param_output_list;
                
                com_splitjoin_body_list->push_back(*it);
                compBodyNode *com_splitjoin_body = new compBodyNode(NULL,com_splitjoin_body_list);
                compositeNode *com_splitjoin = new compositeNode(com_splitjoin_head,com_splitjoin_body);
                

                //compositeNode* unfold_splitjoin_node = UnfoldSplitJoin((splitjoinNode *)*it);
                //第三个为传参 param,这里是空
                compositeCallNode *call = new compositeCallNode(call_outputs, split_join_name, NULL, call_inputs, com_splitjoin,*(*it)->loc);
                call->scope = runningTop;
                call->isOriginal = false;
                comCallList.push_back(call);


                /*
                string split_join_name = MakeCompositeName("splitjoin");
                string tempName = streamName + to_string(num) + "_" + (to_string(cnt++)) + split_join_name;
                idNode *id = new idNode(tempName);
                /* pipeline中第一个composite节点 */
                //compositeCall的输入流

                /*list<Node *> *call_inputs = new list<Node *>({temp_stream->front()});
                list<Node *> *call_outputs = new list<Node *>({id});

                /*将现在的输出流作为下一次的输入流*/
                /*temp_stream->pop_back();
                temp_stream->push_back(id);

                ComInOutNode *com_splitjoin_inout = new ComInOutNode(call_inputs,call_outputs);
                compHeadNode *com_splitjoin_head = new compHeadNode(split_join_name,com_splitjoin_inout);

                list<Node*> *com_splitjoin_body_list = new list<Node *>();

                //改造splitjoin 输入输出
                splitjoinNode *sp_node = (splitjoinNode *)(*it);
                sp_node->inputs = call_inputs;
                sp_node->outputs = call_outputs;

                com_splitjoin_body_list->push_back(*it);
                compBodyNode *com_splitjoin_body = new compBodyNode(NULL,com_splitjoin_body_list);
                compositeNode *com_splitjoin = new compositeNode(com_splitjoin_head,com_splitjoin_body);
                

                //compositeNode* unfold_splitjoin_node = UnfoldSplitJoin((splitjoinNode *)*it);
                //第三个为传参 param,这里是空
                compositeCallNode *call = new compositeCallNode(call_outputs, split_join_name, NULL, call_inputs, com_splitjoin,*(*it)->loc);

                comCallList.push_back(call);
                */
                break;
            }
        
            case Pipeline :{
                
                string pipeline_name = MakeCompositeName("pipeline");
                string tempName = streamName + to_string(num) + "_" + (to_string(cnt));
                idNode *id = new idNode(tempName);
            
                list<Node *> *call_inputs = new list<Node *>({temp_stream->front()});
                list<Node *> *call_outputs = new list<Node *>({id});

                

                /*将现在的输出流作为下一次的输入流*/
                temp_stream->pop_back();
                temp_stream->push_back(id);
                
                if (it == --compositeCall_list.end())
                {
                    call_outputs->pop_back();
                    call_outputs->push_back(real_output);
                }

                list<Node *> *input_list = new list<Node *>();
                list<Node *> *output_list = new list<Node *>();

                list<Node *> *param_input_list = new list<Node *>();
                list<Node *> *param_output_list = new list<Node *>();

                inOutdeclNode *type_input = (runningTop->getStreamTable().begin())->second;
                for(auto it : *call_inputs){ //单输入输出
                    //string name = ((idNode *)it)->name;
                    idNode *copy_input_id = new idNode("In");
                    inOutdeclNode *copy_input = new inOutdeclNode(type_input->strType,copy_input_id);
                    input_list->push_back(copy_input);
                    param_input_list->push_back(copy_input_id);
                }
                for(auto it : *call_outputs){
                    //string name = ((idNode *)it)->name;
                    idNode *copy_output_id = new idNode("Out");
                    inOutdeclNode *copy_output = new inOutdeclNode(type_input->strType,copy_output_id);
                    output_list->push_back(copy_output);
                    param_output_list->push_back(copy_output_id);
                }
                
                ComInOutNode *com_pipeline_inout = new ComInOutNode(input_list,output_list);
                compHeadNode *com_pipeline_head = new compHeadNode(pipeline_name,com_pipeline_inout);

                list<Node*> *com_pipeline_body_list = new list<Node *>();
            
                //改造 pipeline 输入输出
                pipelineNode *pipeline_node = (pipelineNode *)(*it);

                //指定嵌套结构中pipeline的输入和输出流
                pipeline_node->inputs = param_input_list;
                pipeline_node->outputs = param_output_list;
                
                com_pipeline_body_list->push_back(*it);
                compBodyNode *com_pipeline_body = new compBodyNode(NULL,com_pipeline_body_list);
                compositeNode *com_pipeline = new compositeNode(com_pipeline_head,com_pipeline_body);
                //todo composite 复用

                //compositeNode* unfold_splitjoin_node = UnfoldSplitJoin((splitjoinNode *)*it);
                //第三个为传参 param,这里是空
                compositeCallNode *call = new compositeCallNode(call_outputs, pipeline_name, NULL, call_inputs, com_pipeline,*(*it)->loc);
                call->scope = runningTop;
                call->isOriginal = false;
                comCallList.push_back(call);

                break;
            }
            case CompositeCall:{
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
                    call_outputs->push_back(real_output);
                }
                CompositeSymbol *comp_symbol = S.LookupCompositeSymbol(name);
                compositeNode *comp = comp_symbol->composite;
                comp_symbol->count++;
                assert(comp != NULL);
                /*修改composite节点的输入流,输出流*/
                // cout<<"input: "<<((idNode*)call_inputs->front())->name<<endl;
                // cout<<"output: "<<((idNode*)call_outputs->front())->name<<endl;
                //compositeNode *actual_composite = compositeCallStreamReplace(comp, call_inputs, call_outputs);
        
                //传参
                /*list<Node *> *copy_stream_List = NULL;
                if(((compositeCallNode *)*it)->stream_List){
                    list<Node *> *stream_List = ((compositeCallNode *)*it)->stream_List;
                    copy_stream_List = new list<Node*>(stream_List->size());
                    std::copy(stream_List->begin(),stream_List->end(),copy_stream_List->begin());
                }
                else{
                    copy_stream_List = new list<Node *>();
                }*/

                list<Node *> *stream_List = ((compositeCallNode *)*it)->stream_List;
                compositeCallNode *call = new compositeCallNode(call_outputs, name, stream_List, call_inputs, comp,comp_symbol->count,*(*it)->loc);
                call->scope = runningTop;
                //cout<<"actual composite name ="<<actual_composite->compName<<endl;
                call->isOriginal = false;
                comCallList.push_back(call);
                break;
            }
            
        }

        
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
    head->originalInOut = comp->head->inout;
    /* 取得compositeNode的operBody 指向同一块内存 替换输入输出流*/
    for (auto it : *comp->body->stmt_List)
    {  //todo 就返回一个copy?
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
                //paramNode *param = operBody->param;
                list<Node *> stmts = operBody->stmt_list;
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
                operBodyNode *body = new operBodyNode(&stmts, init, work, win);
                operatorNode *oper = new operatorNode(preOutputs, operName, preInputs, body);
                /* 修改输入输出的流名 */
                modifyStreamName(oper, inputs, true);
                modifyStreamName(oper, outputs, false);
                stmt_list->push_back(oper);

                // param
                paramNode *param = NULL;
                if(comp->body->param){
                    list<Node *> *param_list = comp->body->param->param_list;
                    list<Node *> *copy_param_list = new list<Node*>(param_list->size());
                    std::copy(param_list->begin(),param_list->end(),copy_param_list->begin());
                    param = new paramNode(copy_param_list);
                }

                compBodyNode *com_body = new compBodyNode(param, stmt_list);
                copy = new compositeNode(head, com_body,*comp->loc);
            }
        }
        else {
            stmt_list -> push_back(it);
        }
    }
    compBodyNode *com_body = new compBodyNode(NULL, stmt_list);
    copy = new compositeNode(head, com_body);
    return streamReplace(copy, inputs, outputs, 0);
}

/* 对于composite节点内的operatorNode进行流替换 可修改一个compositeNode包含多个operator */
compositeNode *UnfoldComposite::streamReplace(compositeNode *comp, list<Node *> *inputs, list<Node *> *outputs, int flag)
{
    //cout<<"compName = "<<comp->compName<<endl;
    list<Node *> *stmt_list = NULL;
    assert(comp->body != NULL);
    // 深度复制composite
    compositeNode  *newComp = new compositeNode(comp);
    stmt_list = newComp->body->stmt_List;
    // stmt_list = comp->body->stmt_List;
    assert(stmt_list != NULL);
    //cout<<"stmts.size()= "<<stmt_list->size();
    Node *top = NULL;
    Node *back = NULL;

    //第一个语句可能是声明语句
    for(auto it : *stmt_list){
        if(it->type == Operator_){
            if(!top) top = it;
            back = it;
        }
        if(it->type == Binop){
            expNode *exp = ((binopNode *)it)->right;
            if (exp->type == Operator_)
            {
                if(!top) top = it;
                back = it;
            }
            else if (exp->type == SplitJoin)
            {
                if(!top) top = it;
                back = it;
            }
            else if (exp->type == Pipeline)
            {
                if(!top) top = it;
                back = it;
            }
        }
    }
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
    return newComp;
    // return comp;
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
                for (auto it : ((blockNode *)work)->stmt_list)
                    modifyWorkName(it, replaceName, name);
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
                for (auto it : ((blockNode *)work)->stmt_list)
                    modifyWorkName(it, replaceName, name);
            }
            break;
        }
    }
}

Node *UnfoldComposite::workNodeCopy(Node *u)
{
    if(!u){
        return NULL;
    }
    switch (u->type)
    {
    case CompositeCall:{
        
        compositeCallNode * comCallNode = static_cast<compositeCallNode *>(u);
        compositeNode *actual_composite = comCallNode->actual_composite;
        list<Node *> *outputs = comCallNode->outputs;
        list<Node *> *inputs = comCallNode->inputs;
        list<Node *> *stream_List = comCallNode->stream_List;
        list<Node *> *copy_stream_List;
        if(stream_List){
            int length = stream_List->size();
            copy_stream_List =  new list<Node *>(length);
            std::copy(stream_List->begin(),stream_List->end(),copy_stream_List->begin());
        }else{
            copy_stream_List = stream_List;
        }
        
        string compName = comCallNode->compName;
        compositeCallNode *tmp = new compositeCallNode(outputs, compName, copy_stream_List, inputs,actual_composite);
        return tmp;
        break;
    }
    case Split:
    {
        Node *dup_round = workNodeCopy(static_cast<splitNode *>(u)->dup_round);
        splitNode *tmp = new splitNode(dup_round);
        return tmp;
    }
    case RoundRobin:
    {
        list<Node *> *arg_list = new list<Node *>();
        if (static_cast<roundrobinNode *>(u)->arg_list != NULL)
            for (auto it : *static_cast<roundrobinNode *>(u)->arg_list)
                arg_list->push_back(workNodeCopy(it));
        roundrobinNode *tmp = new roundrobinNode(arg_list);
        return tmp;
    }
    case Duplicate:
    {
        Node *exp = workNodeCopy(static_cast<duplicateNode *>(u)->exp);
        duplicateNode *tmp = new duplicateNode((expNode *)exp);
        return tmp;
    }
    case Join:
    {
        Node *rdb = workNodeCopy(static_cast<joinNode *>(u)->rdb);
        joinNode *tmp = new joinNode((roundrobinNode *)rdb);
        return tmp;
    }
    case SplitJoin:
    {
        list<Node *> *outputs = new list<Node *>();
        list<Node *> *inputs = new list<Node *>();

        Node *split = workNodeCopy(static_cast<splitjoinNode *>(u)->split);

        Node *join = workNodeCopy(static_cast<splitjoinNode *>(u)->join);
        list<Node *> *stmt_list = new list<Node *>();
        list<Node *> *body_list = new list<Node *>();
        if (static_cast<splitjoinNode *>(u)->outputs != NULL)
            for (auto it : *static_cast<splitjoinNode *>(u)->outputs)
                outputs->push_back(workNodeCopy(it));
        if (static_cast<splitjoinNode *>(u)->inputs != NULL)
            for (auto it : *static_cast<splitjoinNode *>(u)->inputs)
                inputs->push_back(workNodeCopy(it));
        if (static_cast<splitjoinNode *>(u)->stmt_list != NULL)
            for (auto it : *static_cast<splitjoinNode *>(u)->stmt_list)
                stmt_list->push_back(workNodeCopy(it));
        if (static_cast<splitjoinNode *>(u)->body_stmts != NULL)
            for (auto it : *static_cast<splitjoinNode *>(u)->body_stmts)
                body_list->push_back(workNodeCopy(it));
        splitjoinNode *tmp = new splitjoinNode(inputs, outputs, (splitNode *)split, stmt_list, body_list, (joinNode *)join);
        tmp->replace_composite = NULL;
        return tmp;
    }
    case Pipeline:
    {
        list<Node *> *outputs = new list<Node *>();
        list<Node *> *inputs = new list<Node *>();
        list<Node *> *body_stmts = new list<Node *>();
        if (static_cast<pipelineNode *>(u)->outputs != NULL)
            for (auto it : *static_cast<pipelineNode *>(u)->outputs)
                outputs->push_back(workNodeCopy(it));
        if (static_cast<pipelineNode *>(u)->inputs != NULL)
            for (auto it : *static_cast<pipelineNode *>(u)->inputs)
                inputs->push_back(workNodeCopy(it));
        if (static_cast<pipelineNode *>(u)->body_stmts != NULL)
            for (auto it : *static_cast<pipelineNode *>(u)->body_stmts)
                body_stmts->push_back(workNodeCopy(it));
        pipelineNode *tmp = new pipelineNode(outputs, body_stmts, inputs);
        tmp->replace_composite = NULL;
        return tmp;
    }
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
        for (auto it : static_cast<blockNode *>(u)->stmt_list)
        {
            stmt_list->push_back(workNodeCopy(it));
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
        for (auto it : static_cast<blockNode *>(u)->stmt_list)
        {
            modifyWorkName(it, replaceName, name);
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
