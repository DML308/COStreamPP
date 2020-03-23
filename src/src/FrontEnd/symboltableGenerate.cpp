#include "symboltableGenerate.h"
extern SymbolTable *symboltables[MAX_SCOPE_DEPTH][MAX_SCOPE_DEPTH];
extern SymbolTable *runningTop;
extern bool ifConstantFlow;
SymbolTable S ;//全局
SymbolTable *top; //当前作用域
list<SymbolTable *>saved; //作用域栈

operatorNode *right_opt; // 正在判断是否为有状态的 operator节点
SymbolTable *right_opt_symboltable; // 正在判断是否为有状态的operator节点的 作用域
map<string,Node *> operator_state_identify; // operator 中 init work 外定义的 变量

bool isOperatorState = false; //是否进行 变量收集
bool isOperatorCheck = false; //是否进行 状态判断

void EnterScopeFn(Node *node){
    EnterScope(); /* 进入 composite 块级作用域 */ 
    saved.push_back(top);
    //saved=top;
    top=new SymbolTable(top,node->loc);
}

void ExitScopeFn(){
    ExitScope(); /* 退出 composite 块级作用域 */ 
    top = saved.back();
    saved.pop_back();
}

// todo 检查 变量 是否已经定义 
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

    Variable* act_node = top->LookupIdentifySymbol(name);

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



Constant* getResult(string op,Constant *left,Constant *right){
    if(left && right){
if(op.compare("+") == 0){
        if(left->type.compare("int") == 0 && right->type.compare("int") == 0 ){
            return new Constant("int",left->ival+right->ival);
        }
        if(left->type.compare("long") == 0 && right->type.compare("long") == 0){
            return new Constant("long",left->lval+right->lval);
        }
        if(left->type.compare("long long") == 0 && right->type.compare("long long") == 0){
            return new Constant("long long",left->llval+right->llval);
        }

        if(left->type.compare("float") == 0 && right->type.compare("float") == 0){
            return new Constant("float",left->fval+right->fval);
        }
        if(left->type.compare("float") == 0 && right->type.compare("int") == 0){
            return new Constant("float",left->fval+right->ival);
        }
        if(left->type.compare("int") == 0 && right->type.compare("float") == 0){
            return new Constant("float",left->ival+right->fval);
        }
        
        if(left->type.compare("int") == 0 && right->type.compare("long") == 0){
            return new Constant("long",left->ival+right->lval);
        }
        if(left->type.compare("long") == 0 && right->type.compare("int") == 0){
            return new Constant("long",left->lval+right->ival);
        }
        if(left->type.compare("float") == 0 && right->type.compare("long") == 0){
            return new Constant("float",left->fval+right->lval);
        }
        if(left->type.compare("long") == 0 && right->type.compare("float") == 0){
            return new Constant("float",left->lval+right->fval);
        }
        
        if(left->type.compare("double") == 0 && right->type.compare("long") == 0){
            return new Constant("double",left->dval+right->lval);
        }
        if(left->type.compare("long") == 0 && right->type.compare("double") == 0){
            return new Constant("double",left->lval+right->dval);
        }

        if(left->type.compare("double") == 0 && right->type.compare("double") == 0){
            return new Constant("double",left->fval+right->fval);
        }
        if(left->type.compare("double") == 0 && right->type.compare("int") == 0){
            return new Constant("double",left->fval+right->ival);
        }
        if(left->type.compare("int") == 0 && right->type.compare("double") == 0){
            return new Constant("double",left->ival+right->dval);
        }
        if(left->type.compare("float") == 0 && right->type.compare("double") == 0){
            return new Constant("double",left->fval+right->dval);
        }
        if(left->type.compare("double") == 0 && right->type.compare("float") == 0){
            return new Constant("double",left->dval+right->fval);
        }
        
        if(left->type.compare("string") == 0 && right->type.compare("string") == 0){
            return new Constant("string",left->sval+right->sval);
        }  
    }
    if(op.compare("-") == 0){
        if(left->type.compare("int") == 0 && right->type.compare("int") == 0 ){
            return new Constant("int",left->ival-right->ival);
        }
        if(left->type.compare("long") == 0 && right->type.compare("long") == 0){
            return new Constant("long",left->lval-right->lval);
        }
        if(left->type.compare("long long") == 0 && right->type.compare("long long") == 0){
            return new Constant("long long",left->llval-right->llval);
        }

        if(left->type.compare("int") == 0 && right->type.compare("long") == 0){
            return new Constant("long",left->ival-right->lval);
        }
        if(left->type.compare("long") == 0 && right->type.compare("int") == 0){
            return new Constant("long",left->lval-right->ival);
        }
        if(left->type.compare("float") == 0 && right->type.compare("long") == 0){
            return new Constant("float",left->fval-right->lval);
        }
        if(left->type.compare("long") == 0 && right->type.compare("float") == 0){
            return new Constant("float",left->lval-right->fval);
        }
        if(left->type.compare("double") == 0 && right->type.compare("long") == 0){
            return new Constant("double",left->dval-right->lval);
        }
        if(left->type.compare("long") == 0 && right->type.compare("double") == 0){
            return new Constant("double",left->lval-right->dval);
        }

        if(left->type.compare("float") == 0 && right->type.compare("float") == 0){
            return new Constant("float",left->fval-right->fval);
        }
        if(left->type.compare("float") == 0 && right->type.compare("int") == 0){
            return new Constant("float",left->fval-right->ival);
        }
        if(left->type.compare("int") == 0 && right->type.compare("float") == 0){
            return new Constant("float",left->ival-right->fval);
        }

        if(left->type.compare("double") == 0 && right->type.compare("double") == 0){
            return new Constant("double",left->fval-right->fval);
        }
        if(left->type.compare("double") == 0 && right->type.compare("int") == 0){
            return new Constant("double",left->fval-right->ival);
        }
        if(left->type.compare("int") == 0 && right->type.compare("double") == 0){
            return new Constant("double",left->ival-right->dval);
        }
        if(left->type.compare("float") == 0 && right->type.compare("double") == 0){
            return new Constant("double",left->fval-right->dval);
        }
        if(left->type.compare("double") == 0 && right->type.compare("float") == 0){
            return new Constant("double",left->dval-right->fval);
        }

        if(left->type.compare("string") == 0 && right->type.compare("string") == 0){
            cout << "字符串无法相减";
            exit(-1);
        }

    }
    if(op.compare("*") == 0){
        if(left->type.compare("int") == 0 && right->type.compare("int") == 0 ){
            return new Constant("int",left->ival*right->ival);
        }
        if(left->type.compare("long") == 0 && right->type.compare("long") == 0){
            return new Constant("long",left->lval*right->lval);
        }
        if(left->type.compare("long long") == 0 && right->type.compare("long long") == 0){
            return new Constant("long long",left->llval*right->llval);
        }

        if(left->type.compare("int") == 0 && right->type.compare("long") == 0){
            return new Constant("long",left->ival*right->lval);
        }
        if(left->type.compare("long") == 0 && right->type.compare("int") == 0){
            return new Constant("long",left->lval*right->ival);
        }
        if(left->type.compare("float") == 0 && right->type.compare("long") == 0){
            return new Constant("float",left->fval*right->lval);
        }
        if(left->type.compare("long") == 0 && right->type.compare("float") == 0){
            return new Constant("float",left->lval*right->fval);
        }
        if(left->type.compare("double") == 0 && right->type.compare("long") == 0){
            return new Constant("double",left->dval*right->lval);
        }
        if(left->type.compare("long") == 0 && right->type.compare("double") == 0){
            return new Constant("double",left->lval*right->dval);
        }

        if(left->type.compare("float") == 0 && right->type.compare("float") == 0){
            return new Constant("float",left->fval*right->fval);
        }
        if(left->type.compare("float") == 0 && right->type.compare("int") == 0){
            return new Constant("float",left->fval*right->ival);
        }
        if(left->type.compare("int") == 0 && right->type.compare("float") == 0){
            return new Constant("float",left->ival*right->fval);
        }
        


        if(left->type.compare("double") == 0 && right->type.compare("double") == 0){
            return new Constant("double",left->fval*right->fval);
        }
        if(left->type.compare("double") == 0 && right->type.compare("int") == 0){
            return new Constant("double",left->fval*right->ival);
        }
        if(left->type.compare("int") == 0 && right->type.compare("double") == 0){
            return new Constant("double",left->ival*right->dval);
        }
        if(left->type.compare("float") == 0 && right->type.compare("double") == 0){
            return new Constant("double",left->fval*right->dval);
        }
        if(left->type.compare("double") == 0 && right->type.compare("float") == 0){
            return new Constant("double",left->dval*right->fval);
        }
        

        if(left->type.compare("string") == 0 && right->type.compare("string") == 0){
            cout << "字符串无法相乘";
            exit(-1);
        }

    }
    if(op.compare("/") == 0){
        if(left->type.compare("int") == 0 && right->type.compare("int") == 0 ){
            return new Constant("int",left->ival/right->ival);
        }
        if(left->type.compare("long") == 0 && right->type.compare("long") == 0){
            return new Constant("long",left->lval/right->lval);
        }
        if(left->type.compare("long long") == 0 && right->type.compare("long long") == 0){
            return new Constant("long long",left->llval/right->llval);
        }

        if(left->type.compare("int") == 0 && right->type.compare("long") == 0){
            return new Constant("long",left->ival/right->lval);
        }
        if(left->type.compare("long") == 0 && right->type.compare("int") == 0){
            return new Constant("long",left->lval/right->ival);
        }
        if(left->type.compare("float") == 0 && right->type.compare("long") == 0){
            return new Constant("float",left->fval/right->lval);
        }
        if(left->type.compare("long") == 0 && right->type.compare("float") == 0){
            return new Constant("float",left->lval/right->fval);
        }
        if(left->type.compare("double") == 0 && right->type.compare("long") == 0){
            return new Constant("double",left->dval/right->lval);
        }
        if(left->type.compare("long") == 0 && right->type.compare("double") == 0){
            return new Constant("double",left->lval/right->dval);
        }

        if(left->type.compare("float") == 0 && right->type.compare("float") == 0){
            return new Constant("float",left->fval/right->fval);
        }
        if(left->type.compare("float") == 0 && right->type.compare("int") == 0){
            return new Constant("float",left->fval/right->ival);
        }
        if(left->type.compare("int") == 0 && right->type.compare("float") == 0){
            return new Constant("float",left->ival/right->fval);
        }

        if(left->type.compare("double") == 0 && right->type.compare("double") == 0){
            return new Constant("double",left->fval/right->fval);
        }
        if(left->type.compare("double") == 0 && right->type.compare("int") == 0){
            return new Constant("double",left->fval/right->ival);
        }
        if(left->type.compare("int") == 0 && right->type.compare("double") == 0){
            return new Constant("double",left->ival/right->dval);
        }
        if(left->type.compare("float") == 0 && right->type.compare("double") == 0){
            return new Constant("double",left->fval/right->dval);
        }
        if(left->type.compare("double") == 0 && right->type.compare("float") == 0){
            return new Constant("double",left->dval/right->fval);
        }

        if(left->type.compare("string") == 0 && right->type.compare("string") == 0){
            cout << "字符串无法相除";
            exit(-1);
        }  
    }
    if(op.compare("%") == 0){
        if(left->type.compare("int") == 0 && right->type.compare("int") == 0 ){
            return new Constant("int",left->ival%right->ival);
        }
        if(left->type.compare("long") == 0 && right->type.compare("long") == 0){
            return new Constant("long",left->lval%right->lval);
        }
        if(left->type.compare("long long") == 0 && right->type.compare("long long") == 0){
            return new Constant("long long",left->llval%right->llval);
        }

        if(left->type.compare("int") == 0 && right->type.compare("long") == 0){
            return new Constant("long",left->ival%right->lval);
        }
        if(left->type.compare("long") == 0 && right->type.compare("int") == 0){
            return new Constant("long",left->lval%right->ival);
        }

        if(left->type.compare("double") == 0 || right->type.compare("double") == 0){
             cout << "浮点数无法取余";
             exit(-1);
        }
        if(left->type.compare("float") == 0 || right->type.compare("float") == 0){
            cout << "浮点数无法取余";
            exit(-1);
        }
        if(left->type.compare("string") == 0 || right->type.compare("string") == 0){
            cout << "字符串无法取余";
            exit(-1);
        }
    }
    if(op.compare("|") == 0){
        if(left->type.compare("int") == 0 && right->type.compare("int") == 0 ){
            return new Constant("int",left->ival|right->ival);
        }
        if(left->type.compare("long") == 0 && right->type.compare("long") == 0){
            return new Constant("long",left->lval|right->lval);
        }
        if(left->type.compare("long long") == 0 && right->type.compare("long long") == 0){
            return new Constant("long long",left->llval|right->llval);
        }

        if(left->type.compare("int") == 0 && right->type.compare("long") == 0){
            return new Constant("long",left->ival|right->lval);
        }
        if(left->type.compare("long") == 0 && right->type.compare("int") == 0){
            return new Constant("long",left->lval|right->ival);
        }

        if(left->type.compare("double") == 0 || right->type.compare("double")){
             cout << "浮点数无法或运算";
             exit(-1);
        }
        if(left->type.compare("float") == 0 || right->type.compare("float")){
            cout << "浮点数无法或运算";
            exit(-1);
        }

        if(left->type.compare("string") == 0 && right->type.compare("string") == 0){
            cout << "字符串无法或运算";
            exit(-1);
        }  
    }
    if(op.compare("&") == 0){
        if(left->type.compare("int") == 0 && right->type.compare("int") == 0 ){
            return new Constant("int",left->ival&right->ival);
        }
        if(left->type.compare("long") == 0 && right->type.compare("long") == 0){
            return new Constant("long",left->lval&right->lval);
        }
        if(left->type.compare("long long") == 0 && right->type.compare("long long") == 0){
            return new Constant("long long",left->llval&right->llval);
        }
        if(left->type.compare("int") == 0 && right->type.compare("long") == 0){
            return new Constant("long",left->ival&right->lval);
        }
        if(left->type.compare("long") == 0 && right->type.compare("int") == 0){
            return new Constant("long",left->lval&right->ival);
        }

        if(left->type.compare("double") == 0 || right->type.compare("double")){
             cout << "浮点数无法且运算";
             exit(-1);
        }
        if(left->type.compare("float") == 0 || right->type.compare("float")){
            cout << "浮点数无法且运算";
            exit(-1);
        }

        if(left->type.compare("string") == 0 && right->type.compare("string") == 0){
            cout << "字符串无法且运算";
            exit(-1);
        }  
    }
    if(op.compare("^") == 0){
        if(left->type.compare("int") == 0 && right->type.compare("int") == 0 ){
            return new Constant("int",left->ival^right->ival);
        }
        if(left->type.compare("long") == 0 && right->type.compare("long") == 0){
            return new Constant("long",left->lval^right->lval);
        }
        if(left->type.compare("long long") == 0 && right->type.compare("long long") == 0){
            return new Constant("long long",left->llval^right->llval);
        }

        if(left->type.compare("int") == 0 && right->type.compare("long") == 0){
            return new Constant("long",left->ival^right->lval);
        }
        if(left->type.compare("long") == 0 && right->type.compare("int") == 0){
            return new Constant("long",left->lval^right->ival);
        }

        if(left->type.compare("double") == 0 || right->type.compare("double")){
             cout << "浮点数无法异或运算";
             exit(-1);
        }
        if(left->type.compare("float") == 0 || right->type.compare("float")){
            cout << "浮点数无法异或运算";
            exit(-1);
        }

        if(left->type.compare("string") == 0 && right->type.compare("string") == 0){
            cout << "字符串无法异或运算";
            exit(-1);
        }  
    }
    if(op.compare("<<") == 0){
        if(left->type.compare("int") == 0 && right->type.compare("int") == 0 ){
            return new Constant("int",left->ival<<right->ival);
        }
        if(left->type.compare("long") == 0 && right->type.compare("long") == 0){
            return new Constant("long",left->lval<<right->lval);
        }
        if(left->type.compare("long long") == 0 && right->type.compare("long long") == 0){
            return new Constant("long long",left->llval<<right->llval);
        }

        if(left->type.compare("int") == 0 && right->type.compare("long") == 0){
            return new Constant("long",left->ival<<right->lval);
        }
        if(left->type.compare("long") == 0 && right->type.compare("int") == 0){
            return new Constant("long",left->lval<<right->ival);
        }

        if(left->type.compare("double") == 0 || right->type.compare("double")){
             cout << "浮点数无法左移运算";
             exit(-1);
        }
        if(left->type.compare("float") == 0 || right->type.compare("float")){
            cout << "浮点数无法左移运算";
            exit(-1);
        }

        if(left->type.compare("string") == 0 && right->type.compare("string") == 0){
            cout << "字符串无法左移运算";
            exit(-1);
        }  
    }
    if(op.compare(">>") == 0){
        if(left->type.compare("int") == 0 && right->type.compare("int") == 0 ){
            return new Constant("int",left->ival>>right->ival);
        }
        if(left->type.compare("long") == 0 && right->type.compare("long") == 0){
            return new Constant("long",left->lval>>right->lval);
        }
        if(left->type.compare("long long") == 0 && right->type.compare("long long") == 0){
            return new Constant("long long",left->llval>>right->llval);
        }

        if(left->type.compare("int") == 0 && right->type.compare("long") == 0){
            return new Constant("long",left->ival>>right->lval);
        }
        if(left->type.compare("long") == 0 && right->type.compare("int") == 0){
            return new Constant("long",left->lval>>right->ival);
        }

        if(left->type.compare("double") == 0 || right->type.compare("double")){
             cout << "浮点数无法右移运算";
             exit(-1);
        }
        if(left->type.compare("float") == 0 || right->type.compare("float")){
            cout << "浮点数无法右移运算";
            exit(-1);
        }

        if(left->type.compare("string") == 0 && right->type.compare("string") == 0){
            cout << "字符串无法右移运算";
            exit(-1);
        }  
    }
    
    //逻辑运算
    if(op.compare("<") == 0){
        if(left->type.compare("int") == 0 && right->type.compare("int") == 0 ){
            if(left->ival < right->ival){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        if(left->type.compare("long") == 0 && right->type.compare("long") == 0){
            if(left->lval < right->lval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        if(left->type.compare("long long") == 0 && right->type.compare("long long") == 0){
           if(left->llval < right->llval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("int") == 0 && right->type.compare("long") == 0){
            if(left->ival < right->lval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("long") == 0 && right->type.compare("int") == 0){
            if(left->lval < right->ival){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("long") == 0 && right->type.compare("long long") == 0){
            if(left->lval < right->llval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("long long") == 0 && right->type.compare("long") == 0){
            if(left->llval < right->lval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("int") == 0 && right->type.compare("long long") == 0){
            if(left->ival < right->llval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        if(left->type.compare("long long") == 0 && right->type.compare("int") == 0){
            if(left->llval < right->ival){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("double") == 0 && right->type.compare("double")){
             if(left->dval < right->dval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("float") == 0 && right->type.compare("float")){
            if(left->fval < right->fval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("string") == 0 || right->type.compare("string") == 0){
            cout << "字符串无法比较";
            exit(-1);
        }
    }
    if(op.compare("<=") == 0){
        if(left->type.compare("int") == 0 && right->type.compare("int") == 0 ){
            if(left->ival <= right->ival){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        if(left->type.compare("long") == 0 && right->type.compare("long") == 0){
            if(left->lval <= right->lval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        if(left->type.compare("long long") == 0 && right->type.compare("long long") == 0){
           if(left->llval <= right->llval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("int") == 0 && right->type.compare("long") == 0){
            if(left->ival <= right->lval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("long") == 0 && right->type.compare("int") == 0){
            if(left->lval <= right->ival){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("long") == 0 && right->type.compare("long long") == 0){
            if(left->lval <= right->llval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("long long") == 0 && right->type.compare("long") == 0){
            if(left->llval <= right->lval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("int") == 0 && right->type.compare("long long") == 0){
            if(left->ival <= right->llval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        if(left->type.compare("long long") == 0 && right->type.compare("int") == 0){
            if(left->llval <= right->ival){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("double") == 0 && right->type.compare("double")){
             if(left->dval <= right->dval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        
        if(left->type.compare("float") == 0 && right->type.compare("float")){
            if(left->fval <= right->fval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("string") == 0 || right->type.compare("string") == 0){
            cout << "字符串无法比较";
            exit(-1);
        }
    }
    if(op.compare(">") == 0){
        if(left->type.compare("int") == 0 && right->type.compare("int") == 0 ){
            if(left->ival > right->ival){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        if(left->type.compare("long") == 0 && right->type.compare("long") == 0){
            if(left->lval > right->lval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        if(left->type.compare("long long") == 0 && right->type.compare("long long") == 0){
           if(left->llval > right->llval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("int") == 0 && right->type.compare("long") == 0){
            if(left->ival > right->lval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("long") == 0 && right->type.compare("int") == 0){
            if(left->lval > right->ival){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("long") == 0 && right->type.compare("long long") == 0){
            if(left->lval > right->llval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("long long") == 0 && right->type.compare("long") == 0){
            if(left->llval > right->lval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("int") == 0 && right->type.compare("long long") == 0){
            if(left->ival > right->llval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        if(left->type.compare("long long") == 0 && right->type.compare("int") == 0){
            if(left->llval > right->ival){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("double") == 0 && right->type.compare("double")){
             if(left->dval > right->dval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        
        if(left->type.compare("float") == 0 && right->type.compare("float")){
            if(left->fval > right->fval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("string") == 0 || right->type.compare("string") == 0){
            cout << "字符串无法比较";
            exit(-1);
        }
    }
    if(op.compare(">=") == 0){
        if(left->type.compare("int") == 0 && right->type.compare("int") == 0 ){
            if(left->ival >= right->ival){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        if(left->type.compare("long") == 0 && right->type.compare("long") == 0){
            if(left->lval >= right->lval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        if(left->type.compare("long long") == 0 && right->type.compare("long long") == 0){
           if(left->llval >= right->llval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("int") == 0 && right->type.compare("long") == 0){
            if(left->ival >= right->lval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("long") == 0 && right->type.compare("int") == 0){
            if(left->lval >= right->ival){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("long") == 0 && right->type.compare("long long") == 0){
            if(left->lval >= right->llval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("long long") == 0 && right->type.compare("long") == 0){
            if(left->llval >= right->lval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("int") == 0 && right->type.compare("long long") == 0){
            if(left->ival >= right->llval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        if(left->type.compare("long long") == 0 && right->type.compare("int") == 0){
            if(left->llval >= right->ival){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("double") == 0 && right->type.compare("double")){
             if(left->dval >= right->dval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        
        if(left->type.compare("float") == 0 && right->type.compare("float")){
            if(left->fval >= right->fval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("string") == 0 || right->type.compare("string") == 0){
            cout << "字符串无法比较";
            exit(-1);
        }
    }
    if(op.compare("==") == 0){
        if(left->type.compare("int") == 0 && right->type.compare("int") == 0 ){
            if(left->ival == right->ival){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        if(left->type.compare("long") == 0 && right->type.compare("long") == 0){
            if(left->lval == right->lval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        if(left->type.compare("long long") == 0 && right->type.compare("long long") == 0){
           if(left->llval == right->llval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("int") == 0 && right->type.compare("long") == 0){
            if(left->ival == right->lval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("long") == 0 && right->type.compare("int") == 0){
            if(left->lval == right->ival){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("long") == 0 && right->type.compare("long long") == 0){
            if(left->lval == right->llval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("long long") == 0 && right->type.compare("long") == 0){
            if(left->llval == right->lval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("int") == 0 && right->type.compare("long long") == 0){
            if(left->ival == right->llval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        if(left->type.compare("long long") == 0 && right->type.compare("int") == 0){
            if(left->llval == right->ival){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("double") == 0 && right->type.compare("double")){
             if(left->dval == right->dval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        
        if(left->type.compare("float") == 0 && right->type.compare("float")){
            if(left->fval == right->fval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("string") == 0 || right->type.compare("string") == 0){
            cout << "字符串无法比较";
            exit(-1);
        }
    }
    if(op.compare("!=") == 0){
        if(left->type.compare("int") == 0 && right->type.compare("int") == 0 ){
            if(left->ival != right->ival){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        if(left->type.compare("long") == 0 && right->type.compare("long") == 0){
            if(left->lval != right->lval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        if(left->type.compare("long long") == 0 && right->type.compare("long long") == 0){
           if(left->llval != right->llval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("int") == 0 && right->type.compare("long") == 0){
            if(left->ival != right->lval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("long") == 0 && right->type.compare("int") == 0){
            if(left->lval != right->ival){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("long") == 0 && right->type.compare("long long") == 0){
            if(left->lval != right->llval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("long long") == 0 && right->type.compare("long") == 0){
            if(left->llval != right->lval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("int") == 0 && right->type.compare("long long") == 0){
            if(left->ival != right->llval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        if(left->type.compare("long long") == 0 && right->type.compare("int") == 0){
            if(left->llval != right->ival){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("double") == 0 && right->type.compare("double")){
             if(left->dval != right->dval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        
        if(left->type.compare("float") == 0 && right->type.compare("float")){
            if(left->fval != right->fval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("string") == 0 || right->type.compare("string") == 0){
            cout << "字符串无法比较";
            exit(-1);
        }
    }
    if(op.compare("&&") == 0){
        if(left->type.compare("int") == 0 && right->type.compare("int") == 0 ){
            if(left->ival && right->ival){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        if(left->type.compare("long") == 0 && right->type.compare("long") == 0){
            if(left->lval && right->lval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        if(left->type.compare("long long") == 0 && right->type.compare("long long") == 0){
           if(left->llval && right->llval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("int") == 0 && right->type.compare("long") == 0){
            if(left->ival && right->lval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("long") == 0 && right->type.compare("int") == 0){
            if(left->lval && right->ival){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("long") == 0 && right->type.compare("long long") == 0){
            if(left->lval && right->llval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("long long") == 0 && right->type.compare("long") == 0){
            if(left->llval && right->lval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("int") == 0 && right->type.compare("long long") == 0){
            if(left->ival && right->llval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        if(left->type.compare("long long") == 0 && right->type.compare("int") == 0){
            if(left->llval && right->ival){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("double") == 0 && right->type.compare("double")){
             if(left->dval && right->dval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        
        if(left->type.compare("float") == 0 && right->type.compare("float")){
            if(left->fval && right->fval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("bool") == 0 && right->type.compare("bool")){
            if(left->bval && right->bval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("int") == 0 && right->type.compare("bool")){
            if(left->ival && right->bval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("bool") == 0 && right->type.compare("int")){
            if(left->bval && right->ival){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("bool") == 0 && right->type.compare("long")){
            if(left->bval && right->ival){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("long") == 0 && right->type.compare("bool")){
            if(left->lval && right->bval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("bool") == 0 && right->type.compare("long long")){
            if(left->bval && right->llval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("long long") == 0 && right->type.compare("bool")){
            if(left->llval && right->bval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("double") == 0 && right->type.compare("bool")){
            if(left->dval && right->bval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("bool") == 0 && right->type.compare("double")){
            if(left->bval && right->dval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("float") == 0 && right->type.compare("bool")){
            if(left->fval && right->bval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("bool") == 0 && right->type.compare("double")){
            if(left->bval && right->dval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("double") == 0 && right->type.compare("bool")){
            if(left->dval && right->bval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("string") == 0 || right->type.compare("string") == 0){
            cout << "字符串无法比较";
            exit(-1);
        }
    }
    if(op.compare("||") == 0){
        if(left->type.compare("int") == 0 && right->type.compare("int") == 0 ){
            if(left->ival || right->ival){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        if(left->type.compare("long") == 0 && right->type.compare("long") == 0){
            if(left->lval || right->lval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        if(left->type.compare("long long") == 0 && right->type.compare("long long") == 0){
           if(left->llval || right->llval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("int") == 0 && right->type.compare("long") == 0){
            if(left->ival || right->lval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("long") == 0 && right->type.compare("int") == 0){
            if(left->lval || right->ival){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("long") == 0 && right->type.compare("long long") == 0){
            if(left->lval || right->llval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("long long") == 0 && right->type.compare("long") == 0){
            if(left->llval || right->lval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("int") == 0 && right->type.compare("long long") == 0){
            if(left->ival || right->llval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        if(left->type.compare("long long") == 0 && right->type.compare("int") == 0){
            if(left->llval || right->ival){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("double") == 0 && right->type.compare("double")){
             if(left->dval || right->dval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }
        
        if(left->type.compare("float") == 0 && right->type.compare("float")){
            if(left->fval || right->fval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("bool") == 0 && right->type.compare("bool")){
            if(left->bval || right->bval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("int") == 0 && right->type.compare("bool")){
            if(left->ival || right->bval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("bool") == 0 && right->type.compare("int")){
            if(left->bval || right->ival){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("bool") == 0 && right->type.compare("long")){
            if(left->bval || right->ival){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("long") == 0 && right->type.compare("bool")){
            if(left->lval || right->bval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("bool") == 0 && right->type.compare("long long")){
            if(left->bval || right->llval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("long long") == 0 && right->type.compare("bool")){
            if(left->llval || right->bval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("double") == 0 && right->type.compare("bool")){
            if(left->dval || right->bval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("bool") == 0 && right->type.compare("double")){
            if(left->bval || right->dval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("float") == 0 && right->type.compare("bool")){
            if(left->fval || right->bval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("bool") == 0 && right->type.compare("double")){
            if(left->bval || right->dval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("double") == 0 && right->type.compare("bool")){
            if(left->dval || right->bval){
                return new Constant("bool",true);
            }else{
                return new Constant("bool",false);
            }
        }

        if(left->type.compare("string") == 0 || right->type.compare("string") == 0){
            cout << "字符串无法比较";
            exit(-1);
        }
    }
    
    }else if(!left && right){ // 先不支持 ++i,仅支持 i++
        if(op.compare("+") == 0){
            if(right){
                if(right->type.compare("int") == 0){
                    return new Constant("int",+right->ival);
                }
                if(right->type.compare("long") == 0){
                    return new Constant("long",+right->lval);
                }
                if(right->type.compare("long long") == 0){
                    return new Constant("long long",+right->llval);
                }
                if(right->type.compare("float") == 0){
                    return new Constant("float",+right->fval);
                }
                if(right->type.compare("double") == 0){
                    return new Constant("double",+right->dval);
                } 
            }
        }
        if(op.compare("-") == 0){
            if(right){
                if(right->type.compare("int") == 0){
                    return new Constant("int",-right->ival);
                }
                if(right->type.compare("long") == 0){
                    return new Constant("long",-right->lval);
                }
                if(right->type.compare("long long") == 0){
                    return new Constant("long long",-right->llval);
                }
                if(right->type.compare("float") == 0){
                    return new Constant("float",-right->fval);
                }
                if(right->type.compare("double") == 0){
                    return new Constant("double",-right->dval);
                } 
            }
        }
        if(op.compare("~") == 0){
            if(right){
                if(right->type.compare("int") == 0){
                    return new Constant("int",~right->ival);
                }
                if(right->type.compare("long") == 0){
                    return new Constant("long",~right->lval);
                }
                if(right->type.compare("double")){
                    cout << "浮点数无法求补码运算";
                    exit(-1);
                }
                if(right->type.compare("float")){
                    cout << "浮点数无法求补码运算";
                    exit(-1);
                }

                if(right->type.compare("string") == 0){
                    cout << "字符串无法求补码运算";
                    exit(-1);
                }  
            }
        }
        if(op.compare("!") == 0){
            if(right){
                if(right->type.compare("int") == 0){
                    return new Constant("bool",!right->ival);
                }
                if(right->type.compare("long") == 0){
                    return new Constant("bool",!right->lval);
                }
                if(right->type.compare("long long") == 0){
                    return new Constant("bool",!right->llval);
                }
                if(right->type.compare("float") == 0){
                    return new Constant("bool",!right->fval);
                }
                if(right->type.compare("double") == 0){
                    return new Constant("bool",!right->dval);
                }
                if(right->type.compare("bool") == 0){
                    return new Constant("bool",!right->bval);
                } 
            }
        }
    

    }else if(left && !right){
        if(op.compare("++") == 0){
            if(left){
                if(left->type.compare("int") == 0){
                    return new Constant("int",left->ival++);
                }
                if(left->type.compare("long") == 0){
                    return new Constant("long",left->lval++);
                }
                if(left->type.compare("long long") == 0){
                    return new Constant("long long",left->llval++);
                }
                if(left->type.compare("float") == 0){
                    return new Constant("float",left->fval++);
                }
                if(left->type.compare("double") == 0){
                    return new Constant("double",left->dval++);
                } 
            }
        }
        if(op.compare("--") == 0){
            if(left->type.compare("int") == 0){
                    return new Constant("int",left->ival--);
                }
                if(left->type.compare("long") == 0){
                    return new Constant("long",left->lval--);
                }
                if(left->type.compare("long long") == 0){
                    return new Constant("long long",left->llval--);
                }
                if(left->type.compare("float") == 0){
                    return new Constant("float",left->fval--);
                }
                if(left->type.compare("double") == 0){
                    return new Constant("double",left->dval--);
                } 
        }
    }

    cout << "运算类型不匹配";
    exit(-1);
}
// 计算表达式结果
Constant* getOperationResult(Node* exp){
    switch (exp->type)
    {
    case Id:
        {
            idNode *id = static_cast<idNode *>(exp);
            if(id->isArray){
                return NULL;
            }
            string name = id->name;
            Variable *val = top->LookupIdentifySymbol(name);
            if(val){
                return val->value;
            }else{
                return NULL;
            }
            
            break;
        }
    case Binop:{
        Node *left = static_cast<binopNode *>(exp)->left;
        Node *right = static_cast<binopNode *>(exp)->right;
        Constant *leftV,*rightV;
        if(left){
            leftV =  getOperationResult(static_cast<binopNode *>(exp)->left);
        }
        if(right){
            rightV = getOperationResult(static_cast<binopNode *>(exp)->right);
        }
        string op = static_cast<binopNode *>(exp)->op;
        if(op.compare(".") == 0) return NULL;
        if(leftV && rightV){
            return getResult(op,leftV,rightV);
        }else{
            return NULL;
        }
        
        break;
    }
    case constant:{
        constantNode *value = static_cast<constantNode *>(exp);
        string type = value->style;
        Constant *constant;
        if(type.compare("int") == 0){
            constant = new Constant("int",value->ival);
        }
        if(type.compare("long") == 0){
            constant = new Constant("long",value->lval);
        }
        if(type.compare("long long") == 0){
            constant = new Constant("long long",value->llval);
        }
        if(type.compare("double") == 0){
            constant = new Constant("double",value->dval);
        }
        if(type.compare("float") == 0){
            constant = new Constant("float",value->dval);
        }
        if(type.compare("string") == 0){
            constant = new Constant("string",value->sval);
        }
        return constant;
        break;
    }
    case Call:{
        callNode* call = static_cast<callNode* >(exp);
        //return new Constant("int",0);
        return NULL;
        //call->arg_list
        break;
    }
    default:
        break;
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
            string op = static_cast<binopNode *>(stmt)->op;
            if(op.compare(".") == 0){
                // 对于 点运算  比如: 取 stream 中的 变量
                //Node* stream_node = checkIdentify(left);
                //checkStreamIdentify(stream_node,right);
            }else{
                genrateStmt(left);
                genrateStmt(right);
            }
            // 常量传播
            if(op.compare("=") == 0){
                Variable* variable;
                if(left->type == Binop){
                    //todo 处理数组
                }else if(left->type == Id && right->type == Binop){
                    if(static_cast<binopNode *>(right)->op.compare(".") == 0){
                        return;
                    }
                    if(ifConstantFlow){
                        variable = top->LookupIdentifySymbol(static_cast<idNode*>(left)->name);
                        variable->value  = getOperationResult(right);
                    }
                    
                }
                
               
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
            EnterScopeFn(stmt);
            generatorOperatorNode(static_cast<operatorNode *>(stmt));  //解析 operator 节点
            ExitScopeFn();
            break;
        }
        case Block:{
            generatorBlcokNode(static_cast<blockNode *>(stmt));
            break;
        }
        case While:{
            EnterScopeFn(stmt);
            genrateStmt(static_cast<whileNode *>(stmt)->exp);
            genrateStmt(static_cast<whileNode *>(stmt)->stmt);
            ExitScopeFn();
            break;
        }
        case Do:{
            EnterScopeFn(stmt);
            genrateStmt(static_cast<doNode *>(stmt)->exp);
            genrateStmt(static_cast<doNode *>(stmt)->stmt);
            ExitScopeFn();
            break;
        }
        case For:{
            EnterScopeFn(stmt);
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
            compositeNode *actual_comp = S.LookupCompositeSymbol(static_cast<compositeCallNode *>(stmt)->compName)->composite;
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

// 解析声明语句中的 常量 初始化部分
Constant* generateInitNode(Node* init_value){
    if(init_value != NULL){
        switch (init_value->type)
        {
            case Initializer:{
                list<Node *> init_values = static_cast<initNode *>(init_value)->value;
                for(auto it = init_values.begin();it!=init_values.end();it++){
                    return generateInitNode(*it);
                }
                break;
            }
            default:{
                genrateStmt(init_value);
                return getOperationResult(init_value);
                break;
            }
                
        }
    }
}

// 解析声明语句中 数组的初始化
list<Constant *> *generateInitArray(Node* init_value){
    idNode *id = (idNode *)init_value;
    //ArrayConstant *array = new ArrayConstant(id->valType);
    list<Constant *> *values = new list<Constant *>();
    if(init_value != NULL){
        switch (init_value->type)
        {
            case Initializer:{
                list<Node *> init_values = static_cast<initNode *>(init_value)->value;
                for(auto it = init_values.begin();it!=init_values.end();it++){
                    values->push_back(generateInitNode(*it));
                }
                break;
            }
            default:{
                genrateStmt(init_value);
                values->push_back(getOperationResult(init_value)); 
                break;
            }
                
        }
    }
    return values;
}
// 解析 Declare 节点
void generateDeclareNode(declareNode* dlcNode){
    list<idNode *> id_list = dlcNode->id_list;
    //generatorIdList(id_list);  
    for(auto it = id_list.begin();it!=id_list.end();it++){
        // 处理初始化值
        Node* init_value = (*it)->init;
        if((*it)->isArray){
            ArrayConstant *array = new ArrayConstant((*it)->valType);
            array->values = *generateInitArray(init_value); // todo
            Variable *variable = new Variable("array",(*it)->name,array);
            top->InsertIdentifySymbol(variable);
        }else{
             Constant *constant = generateInitNode(init_value); // 解析初始化值
             top->InsertIdentifySymbol(*it,constant);
        }
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
        stream_dlc->id =new idNode((*it)->name);
        stream_dlc->type = InOutdcl;
        top->InsertStreamSymbol(stream_dlc);
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
            EnterScopeFn(work);
            isOperatorCheck = true; //判断work中是否用到了 外部定义的变量
            generatorBlcokNode(static_cast<blockNode *>(work));
            ExitScopeFn();
            isOperatorCheck = false;
            // 判断是否为有状态operator结束 初始化状态
            right_opt = NULL;
            right_opt_symboltable = NULL;
            
        }

        //window
        if(win){
            list<Node *> *win_list = win->win_list;
            if(win_list != NULL){
                for(auto it = win_list->begin();it != win_list->end();it++){
                generatorWindow(static_cast<winStmtNode *>(*it));
                }
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

    YYLTYPE *loc = new YYLTYPE();
    loc->first_column = 0;
    loc->last_column = __INT_MAX__;
    loc->first_line = 0;
    loc->last_line = __INT_MAX__;

    S = *new SymbolTable(NULL,loc);
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
                EnterScopeFn(it);/* 进入 composite 块级作用域 */ 
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
            list<Node *> *input_List = inout->input_List; //原始输入流
            list<Node *> *output_List = inout->output_List; //原始输出流
            if(input_List != NULL){
                for(auto it = input_List->begin();it!=input_List->end();it++){
                    inOutdeclNode *copy_inoutdeclNode = new inOutdeclNode();
                    copy_inoutdeclNode->strType = static_cast<inOutdeclNode *>(*it)->strType;
                    idNode *copy_id = new idNode(static_cast<inOutdeclNode *>(*it)->id->name);
                    copy_inoutdeclNode->id = copy_id;
                    copy_inoutdeclNode->isInOut = true;
                    top->InsertStreamSymbol(copy_inoutdeclNode);
                }
            }
            if(output_List != NULL){
                for(auto it = output_List->begin();it!=output_List->end();it++){
                    inOutdeclNode *copy_inoutdeclNode = new inOutdeclNode();
                    copy_inoutdeclNode->strType = static_cast<inOutdeclNode *>(*it)->strType;
                    idNode *copy_id = new idNode(static_cast<inOutdeclNode *>(*it)->id->name);
                    copy_inoutdeclNode->id = copy_id;
                    copy_inoutdeclNode->isInOut = true;
                    top->InsertStreamSymbol(copy_inoutdeclNode);
                } 
            } 
        } 
        /*if(composite->head->originalInOut){

            ComInOutNode *originalInOut = composite->head->originalInOut;
            
            if(inout != NULL){
            list<Node *> *original_input_List = originalInOut->input_List; //原始输入流
            list<Node *> *original_output_List = originalInOut->output_List; //原始输出流

            list<Node *> *input_List = inout->input_List; // 流代替后 输入流 
            list<Node *> *output_List = inout->output_List; //流代替后 输出流

            
            if(input_List != NULL && original_input_List != NULL){
                auto original_input = original_input_List->begin();
                for(auto it = input_List->begin();it!=input_List->end();it++){
                    //top->InsertStreamSymbol(((inOutdeclNode *)*original_input)->id->name ,static_cast<inOutdeclNode *>(*it));
                    original_input++;
                }
            }

            if(output_List != NULL && original_output_List != NULL){
                auto original_output = original_output_List->begin();
                for(auto it = output_List->begin();it!=output_List->end();it++){
                     //top->InsertStreamSymbol(((inOutdeclNode *)*original_output)->id->name ,static_cast<inOutdeclNode *>(*it));
                     original_output++;
                }  
            } 
        } 

        }else{
            
        }
    */

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


SymbolTable* generateCompositeRunningContext(compositeCallNode *call,compositeNode *composite,list<Constant *> paramList,list<Node *> *inputs,list<Node *> *outputs){
    if(call){
        if(call->scope){
            top = new SymbolTable(call->scope,NULL); //call->scope 上一层执行上下文
        }else{
            top = new SymbolTable(&S,NULL);
        }
        top->count = call->count;
    }else{
        top = new SymbolTable(&S,NULL);
        top->count = 0;
    }
    
    generateComposite(composite);
    
    compBodyNode *body = composite->body; //body
    paramNode *param;

    //在符号表中将数据流替换为真实数据流
    if(composite->head->inout){
        list<Node *> *comp_inputs = composite->head->inout->input_List;
        list<Node *> *comp_outputs = composite->head->inout->output_List;
        
        //生成 stream 符号表
        if(comp_inputs){
        auto comp_it = comp_inputs->begin();
        for(auto it : *inputs){
            string comp_name = (((inOutdeclNode *)(*comp_it))->id)->name; //composite中的参数名
            string real_name;
            if(call->isOriginal){
                inOutdeclNode *real_stream = runningTop->LookUpStreamSymbol(((idNode *)it)->name); //compositecall传入的参数名
                real_name = real_stream->id->name;
            }else{
                real_name = ((idNode *)it)->name;
            }
            (top->LookUpStreamSymbol(comp_name))->id->name = real_name;
            comp_it++;
        }
        }
        if(comp_outputs){
        auto comp_it = comp_outputs->begin();
        for(auto it : *outputs){
            string comp_name = (((inOutdeclNode *)(*comp_it))->id)->name;
            string real_name;
            if(call->isOriginal){
                inOutdeclNode *real_stream = runningTop->LookUpStreamSymbol(((idNode *)it)->name); //compositecall传入的参数名
                real_name = real_stream->id->name;
            }else{
                real_name = ((idNode *)it)->name;
            }
            (top->LookUpStreamSymbol(comp_name))->id->name = real_name;
            comp_it++;
        }
        }
    }
   
   //多次调用同一个 composite,其内部声明的数据流通过count来进行区分
   for(auto it : top->getStreamTable()){
        inOutdeclNode *stream = it.second;

        if(!stream->isInOut){
            stream->id->name = stream->id->name + to_string(top->count);
        }
    }

    list<Constant*>::iterator paramValue =  paramList.begin();
    if(body != NULL){
        param = body->param; // param
    // 解析 param
        if(param != NULL){
            list<Node *> param_list = *(param->param_list);
            for(auto it = param_list.begin();it != param_list.end();it++){
                Variable *variable = top->LookupIdentifySymbol(((idNode *)(*it))->name);
                 variable->value = (*paramValue);
                 top->InsertParamSymbol(variable);
                /*if(variable->type.compare((*paramValue)->type) == 0){ //todo 参数类型匹配
                    variable->value = (*paramValue);
                }else{
                    cout<<"参数类型不匹配";
                    exit(-1);
                }*/ 
            }
        }
        //解析window 不能放这里
        /*for (auto it : *body->stmt_List)
        {
            operatorNode *exp = NULL;
            if(it->type == Binop){
                Node *right = ((binopNode *)it)->right;
                if(right->type == Operator_){
                    exp = (operatorNode *)right;
                }
            }
            if (it->type == Operator_)
            {
                exp = ((operatorNode *)it);
            }
            if(exp!=NULL){
                //cout<<"exp->type ="<<exp->type<<endl;
                // 解析window
                
                /* 除了window都可以指向一块内存 对于window动态分配一块内存，替换window中的名字，再函数的结尾将流进行替换*/
                /*operBodyNode *operBody = exp->operBody;
                //paramNode *param = operBody->param;
                list<Node *> stmts = operBody->stmt_list;
                list<Node *> *win_list = new list<Node *>();
                /*动态分配生成新的windowNode*/ // 求出具体window大小
                /*for (auto it : *operBody->win->win_list)
                {
                    Node *winType = ((winStmtNode *)it)->winType;
                    string winName = ((winStmtNode *)it)->winName;
                    winStmtNode *copy_winstmt_node;
                    if(winType->type == Sliding){
                        slidingNode *sliding = (slidingNode *)winType;
                        list<Node *> *sliding_list = sliding->arg_list;
                        list<Node *> *copy_list = new list<Node *>();
                        for (auto it : *sliding_list){
                            constantNode *constant_node;
                            Constant *value = getOperationResult(it);
                            if(value->type.compare("int") == 0){ //todo
                                // constant_node = new constantNode(value->type,value->ival);
                            }
                            if(value->type.compare("long") == 0){
                                constant_node = new constantNode(value->type,value->lval);
                            }
                            if(value->type.compare("long long") == 0){
                                constant_node = new constantNode(value->type,value->llval);
                            }
                            copy_list->push_back(constant_node);
                        }
                        slidingNode *copy_sliding = new slidingNode(copy_list);
                        copy_winstmt_node = new winStmtNode(winName,copy_sliding);
                    }
                    if(winType->type == Tumbling){
                        slidingNode *sliding = (slidingNode *)winType;
                        list<Node *> *sliding_list = sliding->arg_list;
                        list<Node *> *copy_list = new list<Node *>();
                        for (auto it : *sliding_list){
                            constantNode *constant_node;
                            Constant *value = getOperationResult(it);
                            if(value->type.compare("int") == 0){ //todo
                                // constant_node = new constantNode(value->type,value->ival);
                            }
                            if(value->type.compare("long") == 0){
                                // constant_node = new constantNode(value->type,value->lval);
                            }
                            if(value->type.compare("long long") == 0){
                                constant_node = new constantNode(value->type,value->llval);
                            }
                            copy_list->push_back(constant_node);
                        }
                        tumblingNode *copy_sliding = new tumblingNode(copy_list);
                        copy_winstmt_node = new winStmtNode(winName,copy_sliding);
                    }
                    win_list->push_back(copy_winstmt_node);
                }
                operBody->win->win_list = win_list;
                //windowNode *win = new windowNode(win_list);  
            }  
            
        }*/
    }
    top->printSymbolTables();
    return top;
}


list<Constant *> generateStreamList(list<Node *> *stream_List,int target){
    list<Constant *> paramList;
    top = FindRightSymbolTable(target);
    for(auto it = stream_List->begin();it != stream_List->end();it++){
        paramList.push_back(getOperationResult((*it)));
    }
    return paramList;
}

list<Constant *> generateStreamList(list<Node *> *stream_List,SymbolTable *s){
    list<Constant *> paramList;
    top = s;
    for(auto it = stream_List->begin();it != stream_List->end();it++){
        paramList.push_back(getOperationResult((*it)));
    }
    return paramList;
}