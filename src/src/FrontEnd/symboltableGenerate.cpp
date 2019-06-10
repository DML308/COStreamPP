#include "symboltableGenerate.h"

SymbolTable S ;//全局
SymbolTable *top;
list<SymbolTable *>saved;

operatorNode *right_opt;

map<string,Node *> operator_state_identify;
bool isOperatorState = false;
bool isOperatorCheck = false;
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
bool checkIdentify(Node* node){
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
    }
    Node* act_node = top->LookupIdentifySymbol(name);
    if(act_node != NULL){
        node = act_node;  //替换成真实 id 节点
        return true;
    }else{
        return false;
    }
}

// 解析 NodeList
void generateNodeList(list<Node *> id_list){
    for(auto it = id_list.begin();it!=id_list.end();it++){
        top->InserIdentifySymbol(static_cast<idNode *>(*it));
        static_cast<idNode *>(*it)->level = Level;
        static_cast<idNode *>(*it)->version = current_version[Level];
    }
}

// 解析 Declare 节点
void generateDeclareNode(declareNode* dlcNode){
    list<idNode *> id_list = dlcNode->id_list;
    //generatorIdList(id_list);  
    for(auto it = id_list.begin();it!=id_list.end();it++){
        top->InserIdentifySymbol(*it);
        if(isOperatorState){
            operator_state_identify.insert(make_pair((*it)->name,*it));
        }
    }
}

// 前置声明
void generatorOperatorNode(operatorNode * optNode);
void generatorSplitjoinNode(splitjoinNode * splitjoinNode);
void generatorPipelineNode(pipelineNode * pipelineNode);


// 解析 语句
void genrateStmt(Node *stmt){
    switch (stmt->type)
    {
        case Binop:{
            expNode *left = static_cast<binopNode *>(stmt)->left;
            expNode *right = static_cast<binopNode *>(stmt)->right; 
            genrateStmt(left);
            genrateStmt(right);
            break;
        }
        case Id :{
            
            Node * id = top->LookupIdentifySymbol(static_cast<idNode *>(stmt)->name);
            assert(id);
            stmt = id;  //?? 替换成真实的idNode 
            break;
        }
        case Decl :{
            generateDeclareNode(static_cast<declareNode *>(stmt));
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
        case While:{
            EnterScopeFn();
            genrateStmt(static_cast<whileNode *>(stmt)->exp);
            genrateStmt(static_cast<whileNode *>(stmt)->stmt);
            ExitScope();
            break;
        }
        case Do:{
            EnterScope();
            genrateStmt(static_cast<doNode *>(stmt)->exp);
            genrateStmt(static_cast<doNode *>(stmt)->stmt);
            ExitScope();
            break;
        }
        case For:{
            genrateStmt(static_cast<forNode *>(stmt)->init);
            genrateStmt(static_cast<forNode *>(stmt)->cond);
            genrateStmt(static_cast<forNode *>(stmt)->next);
            genrateStmt(static_cast<forNode *>(stmt)->stmt); 
            break;
        }
        case Call:{
            funcDclNode *func = top->LookupFunctionSymbol(static_cast<callNode *>(stmt)->name);
            assert(func != NULL);
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

void generatorBlcokNode(blockNode *blockNode){
    list<Node *> stmt_list =blockNode->stmt_list;
    if(&stmt_list != NULL){
        for(auto it = stmt_list.begin();it != stmt_list.end();it++){
            genrateStmt(*it);
        }
    }
}
// 解析 operator 节点
void generatorOperatorNode(operatorNode * optNode){
    list<Node *> *input_List = optNode->inputs; //输入输出参数
    list<Node *> *output_List = optNode->outputs; //
    operBodyNode *body = optNode->operBody; //body
    
    if(input_List != NULL){  //检查
        for(auto it = input_List->begin();it!=input_List->end();it++){
           assert(checkIdentify(*it));
        }
    }
    if(output_List != NULL){ //检查 
        for(auto it = output_List->begin();it!=output_List->end();it++){
           assert(checkIdentify(*it));
        }  
    } 

    if(body != NULL){
        paramNode *param = body->param;
        list<Node *> stmt_list = body->stmt_list; // 其中定义的为变量 在 work中被使用 会使 operator变为 有状态节点
        Node *init = body->init;
        Node *work = body->work; //
        windowNode *win = body->win;

        if(param != NULL){
            generateNodeList(*(param->param_list)); 
        }
        //解析 operator 中的语句
        isOperatorState = true;
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
            isOperatorCheck = true;
            generatorBlcokNode(static_cast<blockNode *>(work));
            ExitScopeFn();
            isOperatorCheck = false;
        }

        //window
   
    }
     
}

void generatorSplitjoinNode(splitjoinNode * splitjoinNode){
    list<Node *> *outputs = splitjoinNode->outputs;
    list<Node *> *inputs = splitjoinNode->inputs;

    if(outputs != NULL){  //检查
        for(auto it = outputs->begin();it!=outputs->end();it++){
            assert(checkIdentify(*it));
        }
    }
    if(inputs != NULL){ //检查 
        for(auto it = inputs->begin();it!=inputs->end();it++){
            assert(checkIdentify(*it));
        }  
    } 
    /*  1.argument.expression.list是一个identifier
        2.查找符号表 identifier是否出现过 */
    
}

// 解析 pipeline 节点 
void generatorPipelineNode(pipelineNode * pipelineNode){

}

// 入口
void generateSymbolTable(list<Node *> *program,SymbolTable *symbol_tables[][MAX_SCOPE_DEPTH]){
    for(int i=0;i<MAX_SCOPE_DEPTH;i++){
        for(int j=0;j<MAX_SCOPE_DEPTH;j++){
            symbol_tables[i][j] = NULL;
        }
    }
    S = new SymbolTable(NULL);
    symbol_tables[0][0] = &S;
    top = &S; 
    assert(program != NULL);
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
    }

    //解析 输入输出流 
    if(inout != NULL){
        list<Node *> *input_List = inout->input_List; //输入流
        list<Node *> *output_List = inout->output_List; //输出流
        if(input_List != NULL){
            for(auto it = input_List->begin();it!=input_List->end();it++){
                top->InserIdentifySymbol(static_cast<inOutdeclNode *>(*it));
            }
        }
        if(output_List != NULL){
            for(auto it = output_List->begin();it!=output_List->end();it++){
                top->InserIdentifySymbol(static_cast<inOutdeclNode *>(*it));
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


void printSymbolTable(SymbolTable *symbol_tables[][MAX_SCOPE_DEPTH]){
    for(int i=0;i<MAX_SCOPE_DEPTH;i++){
        for(int j=0;j<MAX_SCOPE_DEPTH;j++){
            if(symbol_tables[i][j] != NULL){
                cout<<"[Symbol Table] -- Level -- "<<i<<" Version -- "<<j<<endl;
                symbol_tables[i][j]->printSymbolTables();
            }
            
        }
    }
}