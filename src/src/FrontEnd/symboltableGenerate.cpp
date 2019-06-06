#include "symboltableGenerate.h"

SymbolTable S ;//全局
SymbolTable *top;
SymbolTable *saved;

operatorNode *right_opt;

void EnterScopeFn(){
    EnterScope(); /* 进入 composite 块级作用域 */ 
    saved=top;
    top=new SymbolTable(top);
}

void ExitScopeFn(){
    ExitScope(); /* 退出 composite 块级作用域 */ 
    top = saved;
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
    
    if(input_List != NULL){
        for(auto it = input_List->begin();it!=input_List->end();it++){
            top->InserInoutSymbol(static_cast<inOutdeclNode *>(*it));
        }
    }
    if(output_List != NULL){
        for(auto it = output_List->begin();it!=output_List->end();it++){
            top->InserInoutSymbol(static_cast<inOutdeclNode *>(*it));
        }  
    } 

    if(body != NULL){
        paramNode *param = body->param;
        list<Node *> stmt_list = body->stmt_list;
        Node *init = body->init;
        Node *work = body->work;
        windowNode *win = body->win;

        if(param != NULL){
            generateNodeList(*(param->param_list)); 
        }
        //解析 operator 中的语句
        if(&stmt_list != NULL){
            for(auto it = stmt_list.begin();it != stmt_list.end();it++){
                genrateStmt(*it);
            }
        }  

        if(init != NULL){
            generatorBlcokNode(static_cast<blockNode *>(init));
        }
        
        if(work != NULL){
            generatorBlcokNode(static_cast<blockNode *>(work));
        }

        //window
   
    }
     
}
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
            top->InserIdentifySymbol(stmt);
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
        default:
            break;
    }
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
    saved = top;
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

                generateSymbolTableComposite(static_cast<compositeNode *>(it));

                ExitScopeFn(); /* 退出 composite 块级作用域 */ 

                break;
            }
        }
    }
}



// 解析 Composite 节点 
void generateSymbolTableComposite(compositeNode* composite){
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
                top->InserInoutSymbol(static_cast<inOutdeclNode *>(*it));
            }
        }
        if(output_List != NULL){
            for(auto it = output_List->begin();it!=output_List->end();it++){
                top->InserInoutSymbol(static_cast<inOutdeclNode *>(*it));
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