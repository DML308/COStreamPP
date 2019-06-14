#include "symboltableGenerate.h"
extern SymbolTable *symboltables[MAX_SCOPE_DEPTH][MAX_SCOPE_DEPTH];
SymbolTable S ;//全局
SymbolTable *top; //当前作用域
list<SymbolTable *>saved; //作用域栈

operatorNode *right_opt; // 正在判断是否为有状态的 operator节点
SymbolTable *right_opt_symboltable; // 正在判断是否为有状态的operator节点的 作用域
map<string,Node *> operator_state_identify; // operator 中 init work 外定义的 变量

bool isOperatorState = false; //是否进行 变量收集
bool isOperatorCheck = false; //是否进行 状态判断

void EnterScopeFn(){
    EnterScope(); /* 进入 composite 块级作用域 */ 
    saved.push_back(top);
    //saved=top;
    top=new SymbolTable(top);
}

void ExitScopeFn(){
    ExitScope(); /* 退出 composite 块级作用域 */ 
    top = saved.back();
    saved.pop_back();
}

// 检查 变量 是否已经定义
Node* checkIdentify(Node* node){
    string name;
    switch(node->type){
        case Id:{
            name = static_cast<idNode *>(node)->name;
            break;
        }
        case InOutdcl:{
            name = static_cast<inOutdeclNode *>(node)->id->name;
            break;
        }
        case WindowStmt:{
            name = static_cast<winStmtNode *>(node)->winName;
            break;
        }
    }

    Node* act_node = top->LookupIdentifySymbol(name);

    if(isOperatorCheck && operator_state_identify.find(name) != operator_state_identify.end()){ // 判断是否是有状态operator 
        int identify_level = static_cast<idNode *>(act_node)->level; //必然是变量节点
        int identify_version = static_cast<idNode *>(act_node)->version;
        if(symboltables[identify_level][identify_version] == right_opt_symboltable){
                right_opt->hasState = true;
                isOperatorCheck = false; // 找到一个变量就可以判断为 有状态operator 停止继续寻找
            }
        cout<<right_opt->operName<<" state :"<<right_opt->hasState<<endl;
    }

    if(act_node != NULL){
        if(node->type!=WindowStmt){
           // node = act_node;  //替换成真实 node 节点
        }     
        return act_node;
    }else{
        cout<<name<<" is not defined"<<endl; 
        return NULL;
    }
}
bool checkStreamIdentify(Node *stream, Node* node){
    string name = static_cast<idNode *>(node)->name;
    list<idNode *> id_list = static_cast<strdclNode *>(static_cast<inOutdeclNode *>(stream)->strType)->id_list;
    for(auto it = id_list.begin();it!=id_list.end();it++){
        if((*it)->name.compare(name) == 0){
            return true;
        }
    }
    cout<<name<<" is undefined in stream "<< static_cast<inOutdeclNode *>(stream)->id->name <<endl;
    return false;
}

// 前置声明
void generatorOperatorNode(operatorNode * optNode);
void generatorSplitjoinNode(splitjoinNode * splitjoinNode);
void generatorPipelineNode(pipelineNode * pipelineNode);
void generateComposite(compositeNode* composite);
void generatorBlcokNode(blockNode *blockNode);
void generateDeclareNode(declareNode* dlcNode);
void generateStrDlcNode(strdclNode* streamDeclearNode);

// 解析 NodeList
void generateNodeList(list<Node *> id_list){
    for(auto it = id_list.begin();it!=id_list.end();it++){
        top->InsertIdentifySymbol(static_cast<idNode *>(*it));
        static_cast<idNode *>(*it)->level = Level;
        static_cast<idNode *>(*it)->version = current_version[Level];
    }
}


// 解析 语句
void genrateStmt(Node *stmt){
    switch (stmt->type)
    {
        // exp 节点
        case Binop:{
            Node *left = static_cast<binopNode *>(stmt)->left;
            Node *right = static_cast<binopNode *>(stmt)->right; 
            if((static_cast<binopNode *>(stmt)->op).compare(".") == 0){
                // 对于 点运算  比如: 取 stream 中的 变量
                Node* stream_node = checkIdentify(left);
                checkStreamIdentify(stream_node,right);
            }else{
                genrateStmt(left);
                genrateStmt(right);
            }
            break;
        }
        case Unary:{

            break;
        }
        case Ternary:{
            break;
        }
        case Paren:{
            break;
        }
        case Cast:{
            break;
        }
        case Id :{
            checkIdentify(stmt);
            break;
        }
        case constant:{
            break;
        }
        case Decl :{
            generateDeclareNode(static_cast<declareNode *>(stmt));
            break;
        }
        case StrDcl:{
            generateStrDlcNode( static_cast<strdclNode *>(stmt)); 
            break;
        }
        case Operator_:{
            right_opt = static_cast<operatorNode *>(stmt);
            top->InsertOperatorSymbol(static_cast<operatorNode *>(stmt)->operName,static_cast<operatorNode *>(stmt));
            EnterScopeFn();
            generatorOperatorNode(static_cast<operatorNode *>(stmt));  //解析 operator 节点
            ExitScopeFn();
            break;
        }
        case Block:{
            generatorBlcokNode(static_cast<blockNode *>(stmt));
            break;
        }
        case While:{
            EnterScopeFn();
            genrateStmt(static_cast<whileNode *>(stmt)->exp);
            genrateStmt(static_cast<whileNode *>(stmt)->stmt);
            ExitScopeFn();
            break;
        }
        case Do:{
            EnterScopeFn();
            genrateStmt(static_cast<doNode *>(stmt)->exp);
            genrateStmt(static_cast<doNode *>(stmt)->stmt);
            ExitScopeFn();
            break;
        }
        case For:{
            EnterScopeFn();
            genrateStmt(static_cast<forNode *>(stmt)->init);
            genrateStmt(static_cast<forNode *>(stmt)->cond);
            genrateStmt(static_cast<forNode *>(stmt)->next);
            genrateStmt(static_cast<forNode *>(stmt)->stmt);
            ExitScopeFn();
            break;
        }
        case Call:{
            funcDclNode *func = top->LookupFunctionSymbol(static_cast<callNode *>(stmt)->name);
            // print pow 等函数调用 如何与自定义函数 区分 
            //func != NULL;
            static_cast<callNode *>(stmt)->actual_callnode = func;
            // 检查传入的参数是否存在
            break;
        }
        case CompositeCall:{
            compositeNode *actual_comp = top->LookupCompositeSymbol(static_cast<compositeCallNode *>(stmt)->compName);
            static_cast<compositeCallNode *>(stmt)->actual_composite = actual_comp;
            // 检查传入的参数是否存在 以及 获得参数值 
            break;
        }
        case SplitJoin:{
            generatorSplitjoinNode(static_cast<splitjoinNode *>(stmt));

            break;  
        }
        case Pipeline:{
            generatorPipelineNode(static_cast<pipelineNode *>(stmt));
            break;
        }
        default:
            break;
    }
}

// 
void generateInitNode(Node* init_value){
    if(init_value != NULL){
        switch (init_value->type)
        {
            case Initializer:{
                list<Node *> init_values = static_cast<initNode *>(init_value)->value;
                for(auto it = init_values.begin();it!=init_values.end();it++){
                    generateInitNode(*it);
                }
                break;
            }
            default:{
                genrateStmt(init_value);
                break;
            }
                
        }
    }
}
// 解析 Declare 节点
void generateDeclareNode(declareNode* dlcNode){
    list<idNode *> id_list = dlcNode->id_list;
    //generatorIdList(id_list);  
    for(auto it = id_list.begin();it!=id_list.end();it++){
        // 处理初始化值
        Node* init_value = (*it)->init;
        generateInitNode(init_value); // 解析初始化值
        top->InsertIdentifySymbol(*it);
        if(isOperatorState){
            operator_state_identify.insert(make_pair((*it)->name,*it));
        }
    }
}

// 处理 stream 声明变量的语句
void generateStrDlcNode(strdclNode* streamDeclearNode){  //stream "<int x,int y>" 这部分
    list<idNode *> id_list = streamDeclearNode->declare_stream_id;
    for(auto it = id_list.begin();it!=id_list.end();it++){
        // 创建一个 stream 声明节点
        inOutdeclNode *stream_dlc = new inOutdeclNode();
        stream_dlc->strType = streamDeclearNode;
        stream_dlc->id = *it;
        stream_dlc->type = InOutdcl;
        top->InsertIdentifySymbol(stream_dlc);
    }

}
void generatorBlcokNode(blockNode *blockNode){
    list<Node *> stmt_list =blockNode->stmt_list;
    if(&stmt_list != NULL){
        for(auto it = stmt_list.begin();it != stmt_list.end();it++){
            genrateStmt(*it);
        }
    }
}

void genertatorExpList(list<Node *> *exp_list){
    for(auto it = exp_list->begin();it != exp_list->end();it++){
        // 需要实现 值的获取
       genrateStmt(*it);
    }
}
// 解析 window
void generatorWindow(winStmtNode *winstmtNode){
    checkIdentify(winstmtNode);
    Node *winType = winstmtNode->winType;
    switch (winType->type)
    {
        case Sliding:{
            list<Node *> *exp_list = static_cast<slidingNode*>(winType)->arg_list;
            if(exp_list != NULL){
                genertatorExpList(exp_list);
            }
            break;
        }
        case Tumbling:{
            list<Node *> *exp_list = static_cast<tumblingNode*>(winType)->arg_list;
            if(exp_list != NULL){
                genertatorExpList(exp_list);
            }
            break;
        }
    }
}

// 解析 operator 节点
void generatorOperatorNode(operatorNode * optNode){
    list<Node *> *input_List = optNode->inputs; //输入输出参数
    list<Node *> *output_List = optNode->outputs; //
    operBodyNode *body = optNode->operBody; //body
    
    right_opt_symboltable = top; // 保存oprator的作用域,因为 work 外部变量 都保存在此作用域下

    if(input_List != NULL){  //检查
        for(auto it = input_List->begin();it!=input_List->end();it++){
           checkIdentify(*it);
        }
    }
    if(output_List != NULL){ //检查 
        for(auto it = output_List->begin();it!=output_List->end();it++){
           checkIdentify(*it);
        }  
    } 

    if(body != NULL){
        //paramNode *param = body->param;
        list<Node *> stmt_list = body->stmt_list; // 其中定义的为变量 在 work中被使用 会使 operator变为 有状态节点
        Node *init = body->init;
        Node *work = body->work; //
        windowNode *win = body->win;

        //if(param != NULL){
        //    generateNodeList(*(param->param_list)); 
        //}
        //解析 operator 中的语句
        isOperatorState = true; // 收集 operator 在init work 外定义的变量
        if(&stmt_list != NULL){
            for(auto it = stmt_list.begin();it != stmt_list.end();it++){
                genrateStmt(*it);
            }
        }  
        isOperatorState = false;
        
        if(init != NULL){
            generatorBlcokNode(static_cast<blockNode *>(init));
        }
        
        if(work != NULL){
            EnterScopeFn();
            isOperatorCheck = true; //判断work中是否用到了 外部定义的变量
            generatorBlcokNode(static_cast<blockNode *>(work));
            ExitScopeFn();
            isOperatorCheck = false;
            // 判断是否为有状态operator结束 初始化状态
            right_opt = NULL;
            right_opt_symboltable = NULL;
            
        }

        //window
        list<Node *> *win_list = win->win_list;
        if(win_list != NULL){
            for(auto it = win_list->begin();it != win_list->end();it++){
               generatorWindow(static_cast<winStmtNode *>(*it));
            }
        }
    }
     
}


void generatorSplitjoinNode(splitjoinNode * splitjoinNode){
    list<Node *> *outputs = splitjoinNode->outputs;
    list<Node *> *inputs = splitjoinNode->inputs;
    splitNode *split = splitjoinNode->split;
    joinNode *join = splitjoinNode->join;
    list<Node *> *stmt_list = splitjoinNode->stmt_list;
    list<Node *> *body_stmts = splitjoinNode->body_stmts;

    //检查 输入输出
    /*  1.argument.expression.list是一个identifier
        2.查找符号表 identifier是否出现过 */
    if(outputs != NULL){  
        for(auto it = outputs->begin();it!=outputs->end();it++){
            checkIdentify(*it);
        }
    }
    if(inputs != NULL){
        for(auto it = inputs->begin();it!=inputs->end();it++){
            checkIdentify(*it);
        }  
    } 
    
    if(split != NULL){
        Node *node = split->dup_round;
        if(node->type == RoundRobin){
            list<Node *> *exp_list = static_cast<roundrobinNode *>(node)->arg_list;
             if(exp_list != NULL){
                genertatorExpList(exp_list);
            } 
        }else if(node->type == Duplicate){
            // 需要实现 值的获取
            if(static_cast<duplicateNode *>(node)->exp != NULL){
                genrateStmt(static_cast<duplicateNode *>(node)->exp);
            }
            
        }
    }
    if(stmt_list != NULL){
        for(auto it = stmt_list->begin();it != stmt_list->end();it++){
            genrateStmt(*it);
        }
    } 

    if(body_stmts != NULL){
        for(auto it = body_stmts->begin();it != body_stmts->end();it++){
            genrateStmt(*it);
        }
    } 

    if(join!=NULL){
        list<Node *> *exp_list = join->rdb->arg_list;
        if(exp_list != NULL){
            genertatorExpList(exp_list);
        }
    }
}

// 解析 pipeline 节点 
void generatorPipelineNode(pipelineNode *pipelineNode){
    list<Node *> *outputs = pipelineNode->outputs;
    list<Node *> *inputs = pipelineNode->inputs;
    list<Node *> *body_stmts = pipelineNode->body_stmts;

    if(outputs != NULL){  
        for(auto it = outputs->begin();it!=outputs->end();it++){
            checkIdentify(*it);
        }
    }
    if(inputs != NULL){
        for(auto it = inputs->begin();it!=inputs->end();it++){
            checkIdentify(*it);
        }  
    } 

    if(body_stmts != NULL){
        for(auto it = body_stmts->begin();it != body_stmts->end();it++){
            genrateStmt(*it);
        }
    }
}

//入口 传入整棵 AST树
void generateSymbolTable(list<Node *> *program,SymbolTable *symbol_tables[][MAX_SCOPE_DEPTH]){
    for(int i=0;i<MAX_SCOPE_DEPTH;i++){
        for(int j=0;j<MAX_SCOPE_DEPTH;j++){
            symbol_tables[i][j] = NULL;
        }
    }
    S = new SymbolTable(NULL);
    symbol_tables[0][0] = &S;
    top = &S; 
    program != NULL;
    for (auto it : *(program))
    {
        switch(it->type){
            case Decl:{
                generateDeclareNode(static_cast<declareNode *>(it));  
                break;
            }
            case Composite:{ 
                top->InsertCompositeSymbol(static_cast<compositeNode *>(it)->compName,static_cast<compositeNode *>(it));
                EnterScopeFn();/* 进入 composite 块级作用域 */ 
                generateComposite(static_cast<compositeNode *>(it));
                ExitScopeFn(); /* 退出 composite 块级作用域 */ 
                break;
            }
            case FuncDcl:{ // 仅支持全局作用域下的函数声明
                top->InsertFunctionSymbol(static_cast<funcDclNode *>(it)); 
                //以下实现函数内部解析

                //
            }
        }
    }
}



// 解析 Composite 节点 
void generateComposite(compositeNode* composite){
    ComInOutNode *inout = composite->head->inout; //输入输出参数
    compBodyNode *body = composite->body; //body
    paramNode *param;
    list<Node *> *body_stmt;

    if(body != NULL){
        param = body->param; // param
        body_stmt = body->stmt_List;
    

        //解析 输入输出流 stream 作为参数加入符号表
        if(inout != NULL){
            list<Node *> *input_List = inout->input_List; //输入流
            list<Node *> *output_List = inout->output_List; //输出流
            if(input_List != NULL){
                for(auto it = input_List->begin();it!=input_List->end();it++){
                    top->InsertIdentifySymbol(static_cast<inOutdeclNode *>(*it));
                }
            }
            if(output_List != NULL){
                for(auto it = output_List->begin();it!=output_List->end();it++){
                     top->InsertIdentifySymbol(static_cast<inOutdeclNode *>(*it));
                }  
            } 
        } 

    // 解析 param
        if(param != NULL){
            generateNodeList(*(param->param_list)); 
        }

    // 解析 body 
        if(body_stmt != NULL){
            for(auto it = body_stmt->begin();it != body_stmt->end();it++){
                genrateStmt(*it);
            }
        }
    }
}


void printSymbolTable(SymbolTable *symbol_tables[][MAX_SCOPE_DEPTH]){
    for(int i=0;i<MAX_SCOPE_DEPTH;i++){
        for(int j=0;j<MAX_SCOPE_DEPTH;j++){
            if(symbol_tables[i][j] != NULL){
                cout<<"[Symbol Table] -- Level -- "<<i<<" Version -- "<<j<<"--------"<<endl;
                symbol_tables[i][j]->printSymbolTables();
                cout<<endl;   
            }
                   
        }
    }
}