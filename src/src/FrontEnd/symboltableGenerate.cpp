#include "symboltableGenerate.h"
#include "makeFileReader.h"
extern SymbolTable *symboltables[MAX_SCOPE_DEPTH][MAX_SCOPE_DEPTH];
extern SymbolTable *runningTop;
extern bool ifConstantFlow;
SymbolTable S;             //全局
SymbolTable *top;          //当前作用域
list<SymbolTable *> saved; //作用域栈

operatorNode *right_opt;                     // 正在判断是否为有状态的 operator节点
SymbolTable *right_opt_symboltable;          // 正在判断是否为有状态的operator节点的 作用域
map<string, Node *> operator_state_identify; // operator 中 init work 外定义的 变量


vector<Node *> *right_compositecall_list = new vector<Node *>();
vector<vector<Node *>> compositecall_list_stack;


bool isOperatorState = false; //是否进行 变量收集
bool isOperatorCheck = false; //是否进行 状态判断

void EnterScopeFn(Node *node)
{
    EnterScope(); /* 进入 composite 块级作用域 */
    saved.push_back(top);
    //saved=top;
    top = new SymbolTable(top, node->loc);
}

void ExitScopeFn()
{
    ExitScope(); /* 退出 composite 块级作用域 */
    top = saved.back();
    saved.pop_back();
}

// todo 检查 变量 是否已经定义
Node *checkIdentify(Node *node)
{
    string name;
    switch (node->type)
    {
    case Id:
    {
        name = static_cast<idNode *>(node)->name;
        break;
    }
    case InOutdcl:
    {
        name = static_cast<inOutdeclNode *>(node)->id->name;
        break;
    }
    case WindowStmt:
    {
        name = static_cast<winStmtNode *>(node)->winName;
        break;
    }
    }

    Variable *act_node = top->LookupIdentifySymbol(name);
}
bool checkStreamIdentify(Node *stream, Node *node)
{
    string name = static_cast<idNode *>(node)->name;
    list<idNode *> id_list = static_cast<strdclNode *>(static_cast<inOutdeclNode *>(stream)->strType)->id_list;
    for (auto it = id_list.begin(); it != id_list.end(); it++)
    {
        if ((*it)->name.compare(name) == 0)
        {
            return true;
        }
    }
    cout << name << " is undefined in stream " << static_cast<inOutdeclNode *>(stream)->id->name << endl;
    return false;
}

// 前置声明
void generatorOperatorNode(operatorNode *optNode);
void generatorSplitjoinNode(splitjoinNode *splitjoinNode);
void generatorPipelineNode(pipelineNode *pipelineNode);
void generateComposite(compositeNode *composite);
void generatorBlcokNode(blockNode *blockNode);
void generateDeclareNode(declareNode *dlcNode);
void generateStrDlcNode(strdclNode *streamDeclearNode);
void genrateStmt(Node *stmt);

// 解析 NodeList
void generateNodeList(list<Node *> id_list)
{
    for (auto it = id_list.begin(); it != id_list.end(); it++)
    {
        top->InsertIdentifySymbol(static_cast<idNode *>(*it));
        static_cast<idNode *>(*it)->level = Level;
        static_cast<idNode *>(*it)->version = current_version[Level];
    }
}

Constant *getResult(string op, Constant *left, Constant *right)
{
    if (left && right)
    {
        if (op.compare("+") == 0)
        {
            if (left->type.compare("int") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->ival + right->ival);
            }
            if (left->type.compare("int") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->ival + right->lval);
            }
            if (left->type.compare("int") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->ival + right->llval);
            }
            if (left->type.compare("int") == 0 && right->type.compare("float") == 0)
            {
                return new Constant(left->ival + right->fval);
            }
            if (left->type.compare("int") == 0 && right->type.compare("double") == 0)
            {
                return new Constant(left->ival + right->dval);
            }

            if (left->type.compare("long") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->lval + right->ival);
            }
            if (left->type.compare("long") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->lval + right->lval);
            }
            if (left->type.compare("long") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->lval + right->llval);
            }
            if (left->type.compare("long") == 0 && right->type.compare("float") == 0)
            {
                return new Constant(left->lval + right->fval);
            }
            if (left->type.compare("long") == 0 && right->type.compare("double") == 0)
            {
                return new Constant(left->lval + right->dval);
            }
            
            
            if (left->type.compare("long long") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->llval + right->ival);
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->llval + right->lval);
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->llval + right->llval);
            }
            if (left->type.compare("long long") == 0 && right->type.compare("float") == 0)
            {
                return new Constant(left->llval + right->fval);
            }
            if (left->type.compare("long long") == 0 && right->type.compare("double") == 0)
            {
                return new Constant(left->llval + right->dval);
            }

            if (left->type.compare("float") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->fval + right->ival);
            }
            if (left->type.compare("float") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->fval + right->lval);
            }
            if (left->type.compare("float") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->fval + right->llval);
            }
            if (left->type.compare("float") == 0 && right->type.compare("float") == 0)
            {
                return new Constant(left->fval + right->fval);
            }
            if (left->type.compare("float") == 0 && right->type.compare("double") == 0)
            {
                return new Constant(left->fval + right->dval);
            }

            
            if (left->type.compare("double") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->dval + right->ival);
            }
            if (left->type.compare("double") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->dval + right->lval);
            }
            if (left->type.compare("double") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->dval + right->llval);
            }
            if (left->type.compare("double") == 0 && right->type.compare("float") == 0)
            {
                return new Constant(left->dval + right->fval);
            }
            if (left->type.compare("double") == 0 && right->type.compare("double") == 0)
            {
                return new Constant(left->dval + right->dval);
            }

            if (left->type.compare("string") == 0 && right->type.compare("string") == 0)
            {
                return new Constant("string", left->sval + right->sval);
            }
        }
        if (op.compare("-") == 0)
        {
            if (left->type.compare("int") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->ival - right->ival);
            }
            if (left->type.compare("int") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->ival - right->lval);
            }
            if (left->type.compare("int") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->ival - right->llval);
            }
            if (left->type.compare("int") == 0 && right->type.compare("float") == 0)
            {
                return new Constant(left->ival - right->fval);
            }
            if (left->type.compare("int") == 0 && right->type.compare("double") == 0)
            {
                return new Constant(left->ival - right->dval);
            }

            if (left->type.compare("long") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->lval - right->ival);
            }
            if (left->type.compare("long") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->lval - right->lval);
            }
            if (left->type.compare("long") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->lval - right->llval);
            }
            if (left->type.compare("long") == 0 && right->type.compare("float") == 0)
            {
                return new Constant(left->lval - right->fval);
            }
            if (left->type.compare("long") == 0 && right->type.compare("double") == 0)
            {
                return new Constant(left->lval - right->dval);
            }
            
            
            if (left->type.compare("long long") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->llval - right->ival);
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->llval - right->lval);
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->llval - right->llval);
            }
            if (left->type.compare("long long") == 0 && right->type.compare("float") == 0)
            {
                return new Constant(left->llval - right->fval);
            }
            if (left->type.compare("long long") == 0 && right->type.compare("double") == 0)
            {
                return new Constant(left->llval - right->dval);
            }

            if (left->type.compare("float") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->fval - right->ival);
            }
            if (left->type.compare("float") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->fval - right->lval);
            }
            if (left->type.compare("float") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->fval - right->llval);
            }
            if (left->type.compare("float") == 0 && right->type.compare("float") == 0)
            {
                return new Constant(left->fval - right->fval);
            }
            if (left->type.compare("float") == 0 && right->type.compare("double") == 0)
            {
                return new Constant(left->fval - right->dval);
            }

            
            if (left->type.compare("double") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->dval - right->ival);
            }
            if (left->type.compare("double") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->dval - right->lval);
            }
            if (left->type.compare("double") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->dval - right->llval);
            }
            if (left->type.compare("double") == 0 && right->type.compare("float") == 0)
            {
                return new Constant(left->dval - right->fval);
            }
            if (left->type.compare("double") == 0 && right->type.compare("double") == 0)
            {
                return new Constant(left->dval - right->dval);
            }

            if (left->type.compare("string") == 0 && right->type.compare("string") == 0)
            {
                cout << "字符串无法相减";
                exit(-1);
            }
        }
        if (op.compare("*") == 0)
        {
            if (left->type.compare("int") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->ival * right->ival);
            }
            if (left->type.compare("int") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->ival * right->lval);
            }
            if (left->type.compare("int") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->ival * right->llval);
            }
            if (left->type.compare("int") == 0 && right->type.compare("float") == 0)
            {
                return new Constant(left->ival * right->fval);
            }
            if (left->type.compare("int") == 0 && right->type.compare("double") == 0)
            {
                return new Constant(left->ival * right->dval);
            }

            if (left->type.compare("long") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->lval * right->ival);
            }
            if (left->type.compare("long") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->lval * right->lval);
            }
            if (left->type.compare("long") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->lval * right->llval);
            }
            if (left->type.compare("long") == 0 && right->type.compare("float") == 0)
            {
                return new Constant(left->lval * right->fval);
            }
            if (left->type.compare("long") == 0 && right->type.compare("double") == 0)
            {
                return new Constant(left->lval * right->dval);
            }
            
            
            if (left->type.compare("long long") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->llval * right->ival);
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->llval * right->lval);
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->llval * right->llval);
            }
            if (left->type.compare("long long") == 0 && right->type.compare("float") == 0)
            {
                return new Constant(left->llval * right->fval);
            }
            if (left->type.compare("long long") == 0 && right->type.compare("double") == 0)
            {
                return new Constant(left->llval * right->dval);
            }

            if (left->type.compare("float") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->fval * right->ival);
            }
            if (left->type.compare("float") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->fval * right->lval);
            }
            if (left->type.compare("float") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->fval * right->llval);
            }
            if (left->type.compare("float") == 0 && right->type.compare("float") == 0)
            {
                return new Constant(left->fval * right->fval);
            }
            if (left->type.compare("float") == 0 && right->type.compare("double") == 0)
            {
                return new Constant(left->fval * right->dval);
            }

            
            if (left->type.compare("double") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->dval * right->ival);
            }
            if (left->type.compare("double") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->dval * right->lval);
            }
            if (left->type.compare("double") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->dval * right->llval);
            }
            if (left->type.compare("double") == 0 && right->type.compare("float") == 0)
            {
                return new Constant(left->dval * right->fval);
            }
            if (left->type.compare("double") == 0 && right->type.compare("double") == 0)
            {
                return new Constant(left->dval * right->dval);
            }

            if (left->type.compare("string") == 0 && right->type.compare("string") == 0)
            {
                cout << "字符串无法相乘";
                exit(-1);
            }
        }
        if (op.compare("/") == 0)
        {
            if (left->type.compare("int") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->ival / right->ival);
            }
            if (left->type.compare("int") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->ival / right->lval);
            }
            if (left->type.compare("int") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->ival / right->llval);
            }
            if (left->type.compare("int") == 0 && right->type.compare("float") == 0)
            {
                return new Constant(left->ival / right->fval);
            }
            if (left->type.compare("int") == 0 && right->type.compare("double") == 0)
            {
                return new Constant(left->ival / right->dval);
            }

            if (left->type.compare("long") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->lval / right->ival);
            }
            if (left->type.compare("long") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->lval / right->lval);
            }
            if (left->type.compare("long") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->lval / right->llval);
            }
            if (left->type.compare("long") == 0 && right->type.compare("float") == 0)
            {
                return new Constant(left->lval / right->fval);
            }
            if (left->type.compare("long") == 0 && right->type.compare("double") == 0)
            {
                return new Constant(left->lval / right->dval);
            }
            
            
            if (left->type.compare("long long") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->llval / right->ival);
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->llval / right->lval);
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->llval / right->llval);
            }
            if (left->type.compare("long long") == 0 && right->type.compare("float") == 0)
            {
                return new Constant(left->llval / right->fval);
            }
            if (left->type.compare("long long") == 0 && right->type.compare("double") == 0)
            {
                return new Constant(left->llval / right->dval);
            }

            if (left->type.compare("float") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->fval / right->ival);
            }
            if (left->type.compare("float") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->fval / right->lval);
            }
            if (left->type.compare("float") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->fval / right->llval);
            }
            if (left->type.compare("float") == 0 && right->type.compare("float") == 0)
            {
                return new Constant(left->fval / right->fval);
            }
            if (left->type.compare("float") == 0 && right->type.compare("double") == 0)
            {
                return new Constant(left->fval / right->dval);
            }

            
            if (left->type.compare("double") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->dval / right->ival);
            }
            if (left->type.compare("double") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->dval / right->lval);
            }
            if (left->type.compare("double") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->dval / right->llval);
            }
            if (left->type.compare("double") == 0 && right->type.compare("float") == 0)
            {
                return new Constant(left->dval / right->fval);
            }
            if (left->type.compare("double") == 0 && right->type.compare("double") == 0)
            {
                return new Constant(left->dval / right->dval);
            }

            if (left->type.compare("string") == 0 && right->type.compare("string") == 0)
            {
                cout << "字符串无法相除";
                exit(-1);
            }
        }
        if (op.compare("%") == 0)
        {
            if (left->type.compare("int") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->ival % right->ival);
            }
            if (left->type.compare("int") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->ival % right->lval);
            }
            if (left->type.compare("int") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->ival % right->llval);
            }

            if (left->type.compare("long") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->lval % right->ival);
            }
            if (left->type.compare("long") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->lval % right->lval);
            }
            if (left->type.compare("long") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->lval % right->llval);
            }

            if (left->type.compare("long long") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->llval % right->ival);
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->llval % right->lval);
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->llval % right->llval);
            }
            
            
            if (left->type.compare("double") == 0 || right->type.compare("double") == 0)
            {
                cout << "浮点数无法取余";
                exit(-1);
            }
            if (left->type.compare("float") == 0 || right->type.compare("float") == 0)
            {
                cout << "浮点数无法取余";
                exit(-1);
            }
            if (left->type.compare("string") == 0 || right->type.compare("string") == 0)
            {
                cout << "字符串无法取余";
                exit(-1);
            }
        }
        if (op.compare("|") == 0)
        {
            if (left->type.compare("int") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->ival | right->ival);
            }
            if (left->type.compare("int") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->ival | right->lval);
            }
            if (left->type.compare("int") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->ival | right->llval);
            }

            if (left->type.compare("long") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->lval | right->ival);
            }
            if (left->type.compare("long") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->lval | right->lval);
            }
            if (left->type.compare("long") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->lval | right->llval);
            }

            if (left->type.compare("long long") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->llval | right->ival);
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->llval | right->lval);
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->llval | right->llval);
            }
            if (left->type.compare("double") == 0 || right->type.compare("double"))
            {
                cout << "浮点数无法或运算";
                exit(-1);
            }
            if (left->type.compare("float") == 0 || right->type.compare("float"))
            {
                cout << "浮点数无法或运算";
                exit(-1);
            }

            if (left->type.compare("string") == 0 && right->type.compare("string") == 0)
            {
                cout << "字符串无法或运算";
                exit(-1);
            }
        }
        if (op.compare("&") == 0)
        {
            if (left->type.compare("int") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->ival & right->ival);
            }
            if (left->type.compare("int") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->ival & right->lval);
            }
            if (left->type.compare("int") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->ival & right->llval);
            }

            if (left->type.compare("long") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->lval & right->ival);
            }
            if (left->type.compare("long") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->lval & right->lval);
            }
            if (left->type.compare("long") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->lval & right->llval);
            }

            if (left->type.compare("long long") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->llval & right->ival);
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->llval & right->lval);
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->llval & right->llval);
            }

            if (left->type.compare("double") == 0 || right->type.compare("double"))
            {
                cout << "浮点数无法且运算";
                exit(-1);
            }
            if (left->type.compare("float") == 0 || right->type.compare("float"))
            {
                cout << "浮点数无法且运算";
                exit(-1);
            }

            if (left->type.compare("string") == 0 && right->type.compare("string") == 0)
            {
                cout << "字符串无法且运算";
                exit(-1);
            }
        }
        if (op.compare("^") == 0)
        {
            if (left->type.compare("int") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->ival ^ right->ival);
            }
            if (left->type.compare("int") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->ival ^ right->lval);
            }
            if (left->type.compare("int") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->ival ^ right->llval);
            }

            if (left->type.compare("long") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->lval ^ right->ival);
            }
            if (left->type.compare("long") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->lval ^ right->lval);
            }
            if (left->type.compare("long") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->lval ^ right->llval);
            }

            if (left->type.compare("long long") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->llval ^ right->ival);
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->llval ^ right->lval);
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->llval ^ right->llval);
            }

            if (left->type.compare("double") == 0 || right->type.compare("double"))
            {
                cout << "浮点数无法异或运算";
                exit(-1);
            }
            if (left->type.compare("float") == 0 || right->type.compare("float"))
            {
                cout << "浮点数无法异或运算";
                exit(-1);
            }

            if (left->type.compare("string") == 0 && right->type.compare("string") == 0)
            {
                cout << "字符串无法异或运算";
                exit(-1);
            }
        }
        if (op.compare("<<") == 0)
        {
            if (left->type.compare("int") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->ival << right->ival);
            }
            if (left->type.compare("int") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->ival << right->lval);
            }
            if (left->type.compare("int") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->ival << right->llval);
            }

            if (left->type.compare("long") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->lval << right->ival);
            }
            if (left->type.compare("long") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->lval << right->lval);
            }
            if (left->type.compare("long") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->lval << right->llval);
            }

            if (left->type.compare("long long") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->llval << right->ival);
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->llval << right->lval);
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->llval << right->llval);
            }

            if (left->type.compare("double") == 0 || right->type.compare("double"))
            {
                cout << "浮点数无法左移运算";
                exit(-1);
            }
            if (left->type.compare("float") == 0 || right->type.compare("float"))
            {
                cout << "浮点数无法左移运算";
                exit(-1);
            }

            if (left->type.compare("string") == 0 && right->type.compare("string") == 0)
            {
                cout << "字符串无法左移运算";
                exit(-1);
            }
        }
        if (op.compare(">>") == 0)
        {
            if (left->type.compare("int") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->ival >> right->ival);
            }
            if (left->type.compare("int") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->ival >> right->lval);
            }
            if (left->type.compare("int") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->ival >> right->llval);
            }

            if (left->type.compare("long") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->lval >> right->ival);
            }
            if (left->type.compare("long") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->lval >> right->lval);
            }
            if (left->type.compare("long") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->lval >> right->llval);
            }

            if (left->type.compare("long long") == 0 && right->type.compare("int") == 0)
            {
                return new Constant(left->llval >> right->ival);
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long") == 0)
            {
                return new Constant(left->llval >> right->lval);
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long long") == 0)
            {
                return new Constant(left->llval >> right->llval);
            }

            if (left->type.compare("double") == 0 || right->type.compare("double"))
            {
                cout << "浮点数无法右移运算";
                exit(-1);
            }
            if (left->type.compare("float") == 0 || right->type.compare("float"))
            {
                cout << "浮点数无法右移运算";
                exit(-1);
            }

            if (left->type.compare("string") == 0 && right->type.compare("string") == 0)
            {
                cout << "字符串无法右移运算";
                exit(-1);
            }
        }

        //逻辑运算
        if (op.compare("<") == 0)
        {
            if (left->type.compare("int") == 0 && right->type.compare("int") == 0)
            {
                if (left->ival < right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("long") == 0)
            {
                if (left->ival < right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("long long") == 0)
            {
                if (left->ival < right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("float") == 0)
            {
                if (left->ival < right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("double") == 0)
            {
                if (left->ival < right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("long") == 0 && right->type.compare("int") == 0)
            {
                if (left->lval < right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("long") == 0)
            {
                if (left->lval < right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("long long") == 0)
            {
                if (left->lval < right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("float") == 0)
            {
                if (left->lval < right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("double") == 0)
            {
                if (left->lval < right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("long long") == 0 && right->type.compare("int") == 0)
            {
                if (left->llval < right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long") == 0)
            {
                if (left->llval < right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long long") == 0)
            {
                if (left->llval < right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("float") == 0)
            {
                if (left->llval < right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("double") == 0)
            {
                if (left->llval < right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("float") == 0 && right->type.compare("int") == 0)
            {
                if (left->fval < right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("long") == 0)
            {
                if (left->fval < right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("long long") == 0)
            {
                if (left->fval < right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("float") == 0)
            {
                if (left->fval < right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("double") == 0)
            {
                if (left->fval < right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("double") == 0 && right->type.compare("int") == 0)
            {
                if (left->dval < right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("long") == 0)
            {
                if (left->dval < right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("long long") == 0)
            {
                if (left->dval < right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("float") == 0)
            {
                if (left->dval < right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("double") == 0)
            {
                if (left->dval < right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("string") == 0 || right->type.compare("string") == 0)
            {
                cout << "字符串无法比较";
                exit(-1);
            }
        }
        if (op.compare("<=") == 0)
        {
            if (left->type.compare("int") == 0 && right->type.compare("int") == 0)
            {
                if (left->ival <= right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("long") == 0)
            {
                if (left->ival <= right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("long long") == 0)
            {
                if (left->ival <= right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("float") == 0)
            {
                if (left->ival <= right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("double") == 0)
            {
                if (left->ival <= right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("long") == 0 && right->type.compare("int") == 0)
            {
                if (left->lval <= right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("long") == 0)
            {
                if (left->lval <= right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("long long") == 0)
            {
                if (left->lval <= right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("float") == 0)
            {
                if (left->lval <= right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("double") == 0)
            {
                if (left->lval <= right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("long long") == 0 && right->type.compare("int") == 0)
            {
                if (left->llval <= right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long") == 0)
            {
                if (left->llval <= right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long long") == 0)
            {
                if (left->llval <= right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("float") == 0)
            {
                if (left->llval <= right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("double") == 0)
            {
                if (left->llval <= right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("float") == 0 && right->type.compare("int") == 0)
            {
                if (left->fval <= right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("long") == 0)
            {
                if (left->fval <= right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("long long") == 0)
            {
                if (left->fval <= right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("float") == 0)
            {
                if (left->fval <= right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("double") == 0)
            {
                if (left->fval <= right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("double") == 0 && right->type.compare("int") == 0)
            {
                if (left->dval <= right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("long") == 0)
            {
                if (left->dval <= right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("long long") == 0)
            {
                if (left->dval <= right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("float") == 0)
            {
                if (left->dval <= right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("double") == 0)
            {
                if (left->dval <= right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("string") == 0 || right->type.compare("string") == 0)
            {
                cout << "字符串无法比较";
                exit(-1);
            }
        }
        if (op.compare(">") == 0)
        {
            if (left->type.compare("int") == 0 && right->type.compare("int") == 0)
            {
                if (left->ival > right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("long") == 0)
            {
                if (left->ival > right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("long long") == 0)
            {
                if (left->ival > right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("float") == 0)
            {
                if (left->ival > right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("double") == 0)
            {
                if (left->ival > right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("long") == 0 && right->type.compare("int") == 0)
            {
                if (left->lval > right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("long") == 0)
            {
                if (left->lval > right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("long long") == 0)
            {
                if (left->lval > right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("float") == 0)
            {
                if (left->lval > right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("double") == 0)
            {
                if (left->lval > right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("long long") == 0 && right->type.compare("int") == 0)
            {
                if (left->llval > right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long") == 0)
            {
                if (left->llval > right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long long") == 0)
            {
                if (left->llval > right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("float") == 0)
            {
                if (left->llval > right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("double") == 0)
            {
                if (left->llval > right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("float") == 0 && right->type.compare("int") == 0)
            {
                if (left->fval > right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("long") == 0)
            {
                if (left->fval > right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("long long") == 0)
            {
                if (left->fval > right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("float") == 0)
            {
                if (left->fval > right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("double") == 0)
            {
                if (left->fval > right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("double") == 0 && right->type.compare("int") == 0)
            {
                if (left->dval > right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("long") == 0)
            {
                if (left->dval > right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("long long") == 0)
            {
                if (left->dval > right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("float") == 0)
            {
                if (left->dval > right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("double") == 0)
            {
                if (left->dval > right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("string") == 0 || right->type.compare("string") == 0)
            {
                cout << "字符串无法比较";
                exit(-1);
            }
        }
        if (op.compare(">=") == 0)
        {
            if (left->type.compare("int") == 0 && right->type.compare("int") == 0)
            {
                if (left->ival >= right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("long") == 0)
            {
                if (left->ival >= right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("long long") == 0)
            { 
                if (left->ival >= right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("float") == 0)
            {
                if (left->ival >= right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("double") == 0)
            {
                if (left->ival >= right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("long") == 0 && right->type.compare("int") == 0)
            {
                if (left->lval >= right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("long") == 0)
            {
                if (left->lval >= right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("long long") == 0)
            {
                if (left->lval >= right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("float") == 0)
            {
                if (left->lval >= right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("double") == 0)
            {
                if (left->lval >= right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("long long") == 0 && right->type.compare("int") == 0)
            {
                if (left->llval >= right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long") == 0)
            {
                if (left->llval >= right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long long") == 0)
            {
                if (left->llval >= right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("float") == 0)
            {
                if (left->llval >= right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("double") == 0)
            {
                if (left->llval >= right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("float") == 0 && right->type.compare("int") == 0)
            {
                if (left->fval >= right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("long") == 0)
            {
                if (left->fval >= right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("long long") == 0)
            {
                if (left->fval >= right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("float") == 0)
            {
                if (left->fval >= right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("double") == 0)
            {
                if (left->fval >= right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("double") == 0 && right->type.compare("int") == 0)
            {
                if (left->dval >= right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("long") == 0)
            {
                if (left->dval >= right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("long long") == 0)
            {
                if (left->dval >= right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("float") == 0)
            {
                if (left->dval >= right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("double") == 0)
            {
                if (left->dval >= right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("string") == 0 || right->type.compare("string") == 0)
            {
                cout << "字符串无法比较";
                exit(-1);
            }
        }
        if (op.compare("==") == 0)
        {
            if (left->type.compare("int") == 0 && right->type.compare("int") == 0)
            {
                if (left->ival == right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("long") == 0)
            {
                if (left->ival == right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("long long") == 0)
            {
                if (left->ival == right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("float") == 0)
            {
                if (left->ival == right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("double") == 0)
            {
                if (left->ival == right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("long") == 0 && right->type.compare("int") == 0)
            {
                if (left->lval == right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("long") == 0)
            {
                if (left->lval == right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("long long") == 0)
            {
                if (left->lval == right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("float") == 0)
            {
                if (left->lval == right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("double") == 0)
            {
                if (left->lval == right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("long long") == 0 && right->type.compare("int") == 0)
            {
                if (left->llval == right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long") == 0)
            {
                if (left->llval == right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long long") == 0)
            {
                if (left->llval == right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("float") == 0)
            {
                if (left->llval == right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("double") == 0)
            {
                if (left->llval == right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("float") == 0 && right->type.compare("int") == 0)
            {
                if (left->fval == right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("long") == 0)
            {
                if (left->fval == right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("long long") == 0)
            {
                if (left->fval == right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("float") == 0)
            {
                if (left->fval == right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("double") == 0)
            {
                if (left->fval == right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("double") == 0 && right->type.compare("int") == 0)
            {
                if (left->dval == right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("long") == 0)
            {
                if (left->dval == right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("long long") == 0)
            {
                if (left->dval == right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("float") == 0)
            {
                if (left->dval == right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("double") == 0)
            {
                if (left->dval == right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("string") == 0 || right->type.compare("string") == 0)
            {
                cout << "字符串无法比较";
                exit(-1);
            }
        }
        if (op.compare("!=") == 0)
        {
            if (left->type.compare("int") == 0 && right->type.compare("int") == 0)
            {
                if (left->ival != right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("long") == 0)
            {
                if (left->ival != right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("long long") == 0)
            {
                if (left->ival != right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("float") == 0)
            {
                if (left->ival != right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("double") == 0)
            {
                if (left->ival != right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("long") == 0 && right->type.compare("int") == 0)
            {
                if (left->lval != right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("long") == 0)
            {
                if (left->lval != right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("long long") == 0)
            {
                if (left->lval != right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("float") == 0)
            {
                if (left->lval != right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("double") == 0)
            {
                if (left->lval != right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("long long") == 0 && right->type.compare("int") == 0)
            {
                if (left->llval != right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long") == 0)
            {
                if (left->llval != right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long long") == 0)
            {
                if (left->llval != right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("float") == 0)
            {
                if (left->llval != right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("double") == 0)
            {
                if (left->llval != right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("float") == 0 && right->type.compare("int") == 0)
            {
                if (left->fval != right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("long") == 0)
            {
                if (left->fval != right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("long long") == 0)
            {
                if (left->fval != right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("float") == 0)
            {
                if (left->fval != right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("double") == 0)
            {
                if (left->fval != right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("double") == 0 && right->type.compare("int") == 0)
            {
                if (left->dval != right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("long") == 0)
            {
                if (left->dval != right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("long long") == 0)
            {
                if (left->dval != right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("float") == 0)
            {
                if (left->dval != right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("double") == 0)
            {
                if (left->dval != right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("string") == 0 || right->type.compare("string") == 0)
            {
                cout << "字符串无法比较";
                exit(-1);
            }
        }
        if (op.compare("&&") == 0)
        {
            if (left->type.compare("int") == 0 && right->type.compare("int") == 0)
            {
                if (left->ival && right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("long") == 0)
            {
                if (left->ival && right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("long long") == 0)
            {
                if (left->ival && right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("float") == 0)
            {
                if (left->ival && right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("double") == 0)
            {
                if (left->ival && right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("bool") == 0)
            {
                if (left->ival && right->bval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("long") == 0 && right->type.compare("int") == 0)
            {
                if (left->lval && right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("long") == 0)
            {
                if (left->lval && right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("long long") == 0)
            {
                if (left->lval && right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("float") == 0)
            {
                if (left->lval && right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("double") == 0)
            {
                if (left->lval && right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("bool") == 0)
            {
                if (left->lval && right->bval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }


            if (left->type.compare("long long") == 0 && right->type.compare("int") == 0)
            {
                if (left->llval && right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long") == 0)
            {
                if (left->llval && right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long long") == 0)
            {
                if (left->llval && right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("float") == 0)
            {
                if (left->llval && right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("double") == 0)
            {
                if (left->llval && right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("bool") == 0)
            {
                if (left->llval && right->bval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("float") == 0 && right->type.compare("int") == 0)
            {
                if (left->fval && right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("long") == 0)
            {
                if (left->fval && right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("long long") == 0)
            {
                if (left->fval && right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("float") == 0)
            {
                if (left->fval && right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("double") == 0)
            {
                if (left->fval && right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("bool") == 0)
            {
                if (left->fval && right->bval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("double") == 0 && right->type.compare("int") == 0)
            {
                if (left->dval && right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("long") == 0)
            {
                if (left->dval && right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("long long") == 0)
            {
                if (left->dval && right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("float") == 0)
            {
                if (left->dval && right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("double") == 0)
            {
                if (left->dval && right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("bool") == 0)
            {
                if (left->dval && right->bval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("bool") == 0 && right->type.compare("int") == 0)
            {
                if (left->bval && right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("bool") == 0 && right->type.compare("long") == 0)
            {
                if (left->bval && right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("bool") == 0 && right->type.compare("long long") == 0)
            {
                if (left->bval && right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("bool") == 0 && right->type.compare("float") == 0)
            {
                if (left->bval && right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("bool") == 0 && right->type.compare("double") == 0)
            {
                if (left->bval && right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("bool") == 0 && right->type.compare("bool") == 0)
            {
                if (left->bval && right->bval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("string") == 0 || right->type.compare("string") == 0)
            {
                cout << "字符串无法比较";
                exit(-1);
            }
        }
        if (op.compare("||") == 0)
        {
            if (left->type.compare("int") == 0 && right->type.compare("int") == 0)
            {
                if (left->ival || right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("long") == 0)
            {
                if (left->ival || right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("long long") == 0)
            {
                if (left->ival || right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("float") == 0)
            {
                if (left->ival || right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("double") == 0)
            {
                if (left->ival || right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("int") == 0 && right->type.compare("bool") == 0)
            {
                if (left->ival || right->bval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("long") == 0 && right->type.compare("int") == 0)
            {
                if (left->lval || right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("long") == 0)
            {
                if (left->lval || right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("long long") == 0)
            {
                if (left->lval || right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("float") == 0)
            {
                if (left->lval || right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("double") == 0)
            {
                if (left->lval || right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long") == 0 && right->type.compare("bool") == 0)
            {
                if (left->lval || right->bval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }


            if (left->type.compare("long long") == 0 && right->type.compare("int") == 0)
            {
                if (left->llval || right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long") == 0)
            {
                if (left->llval || right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("long long") == 0)
            {
                if (left->llval || right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("float") == 0)
            {
                if (left->llval || right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("double") == 0)
            {
                if (left->llval || right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("long long") == 0 && right->type.compare("bool") == 0)
            {
                if (left->llval || right->bval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("float") == 0 && right->type.compare("int") == 0)
            {
                if (left->fval || right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("long") == 0)
            {
                if (left->fval || right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("long long") == 0)
            {
                if (left->fval || right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("float") == 0)
            {
                if (left->fval || right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("double") == 0)
            {
                if (left->fval || right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("float") == 0 && right->type.compare("bool") == 0)
            {
                if (left->fval || right->bval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("double") == 0 && right->type.compare("int") == 0)
            {
                if (left->dval || right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("long") == 0)
            {
                if (left->dval || right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("long long") == 0)
            {
                if (left->dval || right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("float") == 0)
            {
                if (left->dval || right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("double") == 0)
            {
                if (left->dval || right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("double") == 0 && right->type.compare("bool") == 0)
            {
                if (left->dval || right->bval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("bool") == 0 && right->type.compare("int") == 0)
            {
                if (left->bval || right->ival)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("bool") == 0 && right->type.compare("long") == 0)
            {
                if (left->bval || right->lval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("bool") == 0 && right->type.compare("long long") == 0)
            {
                if (left->bval || right->llval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("bool") == 0 && right->type.compare("float") == 0)
            {
                if (left->bval || right->fval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("bool") == 0 && right->type.compare("double") == 0)
            {
                if (left->bval || right->dval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }
            if (left->type.compare("bool") == 0 && right->type.compare("bool") == 0)
            {
                if (left->bval || right->bval)
                    return new Constant("bool", true);
                else
                    return new Constant("bool", false);
                
            }

            if (left->type.compare("string") == 0 || right->type.compare("string") == 0)
            {
                cout << "字符串无法比较";
                exit(-1);
            }
        }
    }
    else if (!left && right)
    { // 先不支持 ++i,仅支持 i++
        if (op.compare("+") == 0)
        {
            if (right)
            {
                if (right->type.compare("int") == 0)
                {
                    return new Constant("int", +right->ival);
                }
                if (right->type.compare("long") == 0)
                {
                    return new Constant("long", +right->lval);
                }
                if (right->type.compare("long long") == 0)
                {
                    return new Constant("long long", +right->llval);
                }
                if (right->type.compare("float") == 0)
                {
                    return new Constant("float", +right->fval);
                }
                if (right->type.compare("double") == 0)
                {
                    return new Constant("double", +right->dval);
                }
            }
        }
        if (op.compare("-") == 0)
        {
            if (right)
            {
                if (right->type.compare("int") == 0)
                {
                    return new Constant("int", -right->ival);
                }
                if (right->type.compare("long") == 0)
                {
                    return new Constant("long", -right->lval);
                }
                if (right->type.compare("long long") == 0)
                {
                    return new Constant("long long", -right->llval);
                }
                if (right->type.compare("float") == 0)
                {
                    return new Constant("float", -right->fval);
                }
                if (right->type.compare("double") == 0)
                {
                    return new Constant("double", -right->dval);
                }
            }
        }
        if (op.compare("~") == 0)
        {
            if (right)
            {
                if (right->type.compare("int") == 0)
                {
                    return new Constant("int", ~right->ival);
                }
                if (right->type.compare("long") == 0)
                {
                    return new Constant("long", ~right->lval);
                }
                if (right->type.compare("double"))
                {
                    cout << "浮点数无法求补码运算";
                    exit(-1);
                }
                if (right->type.compare("float"))
                {
                    cout << "浮点数无法求补码运算";
                    exit(-1);
                }

                if (right->type.compare("string") == 0)
                {
                    cout << "字符串无法求补码运算";
                    exit(-1);
                }
            }
        }
        if (op.compare("!") == 0)
        {
            if (right)
            {
                if (right->type.compare("int") == 0)
                {
                    return new Constant("bool", !right->ival);
                }
                if (right->type.compare("long") == 0)
                {
                    return new Constant("bool", !right->lval);
                }
                if (right->type.compare("long long") == 0)
                {
                    return new Constant("bool", !right->llval);
                }
                if (right->type.compare("float") == 0)
                {
                    return new Constant("bool", !right->fval);
                }
                if (right->type.compare("double") == 0)
                {
                    return new Constant("bool", !right->dval);
                }
                if (right->type.compare("bool") == 0)
                {
                    return new Constant("bool", !right->bval);
                }
            }
        }
    }
    else if (left && !right)
    {
        if (op.compare("++") == 0)
        {
            if (left)
            {
                if (left->type.compare("int") == 0)
                {
                    return new Constant("int", left->ival++);
                }
                if (left->type.compare("long") == 0)
                {
                    return new Constant("long", left->lval++);
                }
                if (left->type.compare("long long") == 0)
                {
                    return new Constant("long long", left->llval++);
                }
                if (left->type.compare("float") == 0)
                {
                    return new Constant("float", left->fval++);
                }
                if (left->type.compare("double") == 0)
                {
                    return new Constant("double", left->dval++);
                }
            }
        }
        if (op.compare("--") == 0)
        {
            if (left->type.compare("int") == 0)
            {
                return new Constant("int", left->ival--);
            }
            if (left->type.compare("long") == 0)
            {
                return new Constant("long", left->lval--);
            }
            if (left->type.compare("long long") == 0)
            {
                return new Constant("long long", left->llval--);
            }
            if (left->type.compare("float") == 0)
            {
                return new Constant("float", left->fval--);
            }
            if (left->type.compare("double") == 0)
            {
                return new Constant("double", left->dval--);
            }
        }
    }

    cout << "运算类型不匹配";
    exit(-1);
}

//类型的强制转换
void changeValueType(string type, Constant *value)
{
    
    string val_type = value->type;
    if (type.compare("int") == 0)
    {
        if (val_type.compare("long") == 0)
        {
            value->ival = (int)value->lval;
        }
        if (val_type.compare("long long") == 0)
        {
            value->ival = (int)value->llval;
        }
        if (val_type.compare("double") == 0)
        {
            value->ival = (int)value->dval;
        }
        if (val_type.compare("float") == 0)
        {
            value->ival = (int)value->fval;
        }
    }
    if (type.compare("long") == 0)
    {
        if (val_type.compare("int") == 0)
        {
            value->lval = (long)value->ival;
        }
        if (val_type.compare("long long") == 0)
        {
            value->lval = (long)value->llval;
        }
        if (val_type.compare("double") == 0)
        {
            value->lval = (long)value->dval;
        }
        if (val_type.compare("float") == 0)
        {
            value->lval = (long)value->fval;
        }
    }
    if (type.compare("long long") == 0)
    {
        if (val_type.compare("int") == 0)
        {
            value->llval = (long long)value->ival;
        }
        if (val_type.compare("long") == 0)
        {
            value->llval = (long long)value->lval;
        }
        if (val_type.compare("double") == 0)
        {
            value->llval = (long long)value->dval;
        }
        if (val_type.compare("float") == 0)
        {
            value->llval = (long long)value->fval;
        }
    }
    if (type.compare("double") == 0)
    {
        if (val_type.compare("int") == 0)
        {
            value->dval = (double)value->ival;
        }
        if (val_type.compare("long") == 0)
        {
            value->dval = (double)value->lval;
        }
        if (val_type.compare("long long") == 0)
        {
            value->dval = (double)value->llval;
        }
        if (val_type.compare("float") == 0)
        {
            value->dval = (double)value->fval;
        }
    }
    if (type.compare("float") == 0)
    {
        if (val_type.compare("int") == 0)
        {
            value->fval = (float)value->ival;
        }
        if (val_type.compare("long") == 0)
        {
            value->fval = (float)value->lval;
        }
        if (val_type.compare("long long") == 0)
        {
            value->fval = (float)value->llval;
        }
        if (val_type.compare("double") == 0)
        {
            value->fval = (float)value->dval;
        }
    }
    value->type = type;
}

//数学函数调用 计算结果为 double 单参数
Constant *getCallValue(string name, list<Constant *> params)
{
    Constant *result = new Constant("double");
    result->type = "double";
    char *fooName[] = {"sin", "cos", "tan", "exp", "log", "sqrt"};
    double (*foo[])(double) = {sin, cos, tan, exp, log, sqrt};
    int num = sizeof(foo) / sizeof(double);
    int i;
    for (i = 0; i < num; ++i)
    {
        string call_name = fooName[i];
        if (call_name.compare(name) == 0)
            break;
    }

    if(i == num){
        return NULL;
    }
    //assert(i != num);

    Constant *param = params.front();
    if (param->type.compare("int") == 0)
    {
        result->dval = foo[i](param->ival);
    }
    if (param->type.compare("long") == 0)
    {
        result->dval = foo[i](param->lval);
    }
    if (param->type.compare("long long") == 0)
    {
        result->dval = foo[i](param->llval);
    }
    if (param->type.compare("float") == 0)
    {
        result->dval = foo[i](param->fval);
    }
    if (param->type.compare("double") == 0)
    {
        result->dval = foo[i](param->dval);
    }
    return result;
}
// 计算表达式结果
Constant *getOperationResult(Node *exp)
{
    switch (exp->type)
    {
    case Id:
    {
        idNode *id = static_cast<idNode *>(exp);

        string name = id->name;
        Variable *variable = top->LookupIdentifySymbol(name);

        if (id->isArray)
        {
            variable->isArray = true;
            if (id->arg_list.size())
            {
                int index = 0;
                bool canGetIndex = true;
                //处理数组下标
                vector<int> arg_size = ((ArrayConstant *)variable->value)->arg_size;
                vector<int> each_size;
                for (int i = arg_size.size(); i >= 1; i--)
                {
                    if (each_size.size())
                    {
                        each_size.push_back(arg_size[i] * each_size.back());
                    }
                    else
                    {
                        each_size.push_back(1);
                    }
                }

                int array_size = arg_size.size() - 1;
                int level = 0;
                for (auto i : id->arg_list)
                {
                    int size;
                    Constant *value = getOperationResult(i);
                    if (value)
                    {
                        size = value->llval;
                    }
                    else
                    {
                        canGetIndex = false;
                        break;
                    }
                    index += each_size[array_size-level] * size;
                    level++;
                }
                if (canGetIndex)
                {
                    if(index > (((ArrayConstant *)(variable->value))->values).size()-1){
                                cout<<"数组越界";
                                exit(-1);
                            }
                    return ((ArrayConstant *)variable->value)->values[index];
                }
                else
                {
                    return NULL;
                }
            }
            return variable->value;
        }

        if (variable)
        {
            return variable->value;
        }
        else
        {
            return NULL;
        }

        break;
    }
    case Binop:
    {
        Node *left = static_cast<binopNode *>(exp)->left;
        Node *right = static_cast<binopNode *>(exp)->right;
        Constant *leftV, *rightV;
        string op = static_cast<binopNode *>(exp)->op;
        if (op.compare(".") == 0)
            return NULL;
        if (left)
        {
            leftV = getOperationResult(static_cast<binopNode *>(exp)->left);
        }
        if (right)
        {
            rightV = getOperationResult(static_cast<binopNode *>(exp)->right);
        }

        if (leftV && rightV)
        {
            return getResult(op, leftV, rightV);
        }
        else
        {
            return NULL;
        }

        break;
    }
    case Unary:{
        unaryNode *unary_node = (unaryNode *)exp;
        Constant *right = getOperationResult(unary_node->exp);
        string op = unary_node->op;
        if(op.compare("POSTINC") == 0){
            op = "++";
            return getResult(op,right,NULL); //i++
        }else if(op.compare("PREDEC") == 0){
            op = "--";
            return getResult(op,right,NULL); //i--
        }
        if(right){
            return getResult(op,NULL,right);
        }else{
            return NULL;
        }
        
        break;
    }
    case Paren:
    {
        return getOperationResult(static_cast<parenNode *>(exp)->exp);
        break;
    }
    case Cast:
    {
        castNode *cast_node = (castNode *)(exp);
        string type = cast_node->prim->name;
        Constant *value = getOperationResult(cast_node->exp);
        if(value){
            changeValueType(type, value);
        }
        return value;
        break;
    }
    case constant:
    {
        constantNode *value = static_cast<constantNode *>(exp);
        string type = value->style;
        Constant *constant;
        if(value->value){
            return value->value;
        }
        if (type.compare("int") == 0)
        {
            constant = new Constant("int", value->ival);
        }
        if (type.compare("long") == 0)
        {
            constant = new Constant("long", value->lval);
        }
        if (type.compare("long long") == 0)
        {
            constant = new Constant("long long", value->llval);
        }
        if (type.compare("double") == 0)
        {
            constant = new Constant("double", value->dval);
        }
        if (type.compare("float") == 0)
        {
            constant = new Constant("float", value->fval);
        }
        if (type.compare("string") == 0)
        {
            constant = new Constant("string", value->sval);
        }
        return constant;
        break;
    }
    case Call:
    {
        callNode *call = static_cast<callNode *>(exp);
        string name = call->name;
        list<Node *> params = call->arg_list;
        list<Constant *> param_values;
        for (auto it : params)
        {
            Constant *value = getOperationResult(it);
            if(value){
                param_values.push_back(value);
            }
            
        }
        if(param_values.size() == params.size()){
            return getCallValue(name, param_values);
        }else{
            return NULL;
        }
        
        break;
    }
    case Ternary:{
        ternaryNode *ternary_node = (ternaryNode *)exp;
        expNode *first = ternary_node->first;
        expNode *second = ternary_node->second;
        expNode *third = ternary_node->third;
        if(getOperationResult(first)->bval){
            return getOperationResult(second);
        }else{
            return getOperationResult(third);
        }
        break;
    }
    default:
        return NULL;
        break;
    }
}

Node *workNodeCopy(Node *u)
{
    if (!u)
    {
        return NULL;
    }
    switch (u->type)
    {
    case CompositeCall:
    {

        compositeCallNode *comCallNode = static_cast<compositeCallNode *>(u);
        compositeNode *actual_composite = comCallNode->actual_composite;
        list<Node *> *outputs = comCallNode->outputs;
        list<Node *> *inputs = comCallNode->inputs;
        list<Node *> *stream_List = comCallNode->stream_List;
        list<Node *> *copy_stream_List;
        if (stream_List)
        {
            int length = stream_List->size();
            copy_stream_List = new list<Node *>(length);
            std::copy(stream_List->begin(), stream_List->end(), copy_stream_List->begin());
        }
        else
        {
            copy_stream_List = stream_List;
        }

        string compName = comCallNode->compName;
        compositeCallNode *tmp = new compositeCallNode(outputs, compName, copy_stream_List, inputs, actual_composite);
        tmp->loc = u->loc;
        return tmp;
        break;
    }
    case Split:
    {
        Node *dup_round = workNodeCopy(static_cast<splitNode *>(u)->dup_round);
        splitNode *tmp = new splitNode(dup_round);
        tmp->loc = u->loc;
        return tmp;
    }
    case RoundRobin:
    {
        list<Node *> *arg_list = new list<Node *>();
        if (static_cast<roundrobinNode *>(u)->arg_list != NULL)
            for (auto it : *static_cast<roundrobinNode *>(u)->arg_list)
                arg_list->push_back(workNodeCopy(it));
        roundrobinNode *tmp = new roundrobinNode(arg_list);
        tmp->loc = u->loc;
        return tmp;
    }
    case Duplicate:
    {
        Node *exp = workNodeCopy(static_cast<duplicateNode *>(u)->exp);
        duplicateNode *tmp = new duplicateNode((expNode *)exp);
        tmp->loc = u->loc;
        return tmp;
    }
    case Join:
    {
        Node *rdb = workNodeCopy(static_cast<joinNode *>(u)->rdb);
        joinNode *tmp = new joinNode((roundrobinNode *)rdb);
        tmp->loc = u->loc;
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
        tmp->loc = u->loc;
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
        tmp->loc = u->loc;
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
        tmp->loc = u->loc;
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
        tmp->loc = u->loc;
        return tmp;
        break;
    }
    case Binop:
    {
        Node *left = workNodeCopy(static_cast<binopNode *>(u)->left);
        Node *right = workNodeCopy(static_cast<binopNode *>(u)->right);
        binopNode *tmp = new binopNode((expNode *)left, static_cast<binopNode *>(u)->op, (expNode *)right);
        tmp->loc = u->loc;
        return tmp;
        break;
    }
    case Paren:
    {
        Node *exp = workNodeCopy(static_cast<parenNode *>(u)->exp);
        parenNode *tmp = new parenNode((expNode *)exp);
        tmp->loc = u->loc;
        return tmp;
        break;
    }
    case Unary:
    {
        Node *exp = workNodeCopy(static_cast<unaryNode *>(u)->exp);
        unaryNode *tmp = new unaryNode(static_cast<unaryNode *>(u)->op, (expNode *)exp);
        tmp->loc = u->loc;
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
        tmp->loc = u->loc;
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
        tmp->loc = u->loc;
        return tmp;
        break;
    }
    case Case:
    {
        Node *exp = workNodeCopy(static_cast<caseNode *>(u)->exp);
        Node *stmt = workNodeCopy(static_cast<caseNode *>(u)->stmt);
        caseNode *tmp = new caseNode((expNode *)exp, (expNode *)stmt);
        tmp->loc = u->loc;
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
        tmp->loc = u->loc;
        return tmp;
        break;
    }
    case IfElse:
    {
        Node *exp = workNodeCopy(static_cast<ifElseNode *>(u)->exp);
        Node *stmt1 = workNodeCopy(static_cast<ifElseNode *>(u)->stmt1);
        Node *stmt2 = workNodeCopy(static_cast<ifElseNode *>(u)->stmt2);
        ifElseNode *tmp = new ifElseNode((expNode *)exp, (expNode *)stmt1, (expNode *)stmt2);
        tmp->loc = u->loc;
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
        tmp->loc = u->loc;
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
        tmp->loc = u->loc;
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
        block->loc = u->loc;
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
        tmp->loc = u->loc;
        return tmp;
        break;
    }
    default:
        break;
    }
    return u;
}

//替换 compositecall 传入的参数 => 常量
void compositeVariableReplace(Node *node)
{
    if (node->type == CompositeCall)
    {
        compositeCallNode *call_composite = (compositeCallNode *)node;
        list<Node *> *params = call_composite->stream_List;
        list<Node *> *actual_params = new list<Node *>();
        
        if (params)
        {
            for (auto param : *params)
            {
                actual_params->push_back(copyConstantNode(getOperationResult(param)));
            }    
            call_composite->stream_List = actual_params;
        }
    }
    if (node->type == SplitJoin)
    {
        //todo 找到for中的变量,然后替换,不支持for中变量的赋值
    }
    if (node->type == Pipeline)
    {
    }
}

/*
    compositeflow 只会对 splitjoin pipiline内的composite调用进行分析,
    其他地方的composite调用不会添加到 comositecall_flow 中
*/
//模拟if执行过程
void generateIfConstant(Node *nd)
{
    if (nd->type == IfElse)
    {

        list<Node *> *ifelse_list = new list<Node *>();
        ifElseNode *if_node = (ifElseNode *)nd;
        Constant *bool_result = getOperationResult(if_node->exp);
        if(!bool_result){
            return;
        }
        if (bool_result->bval)
        {
            ifelse_list->push_back(if_node->stmt1);
        }
        else
        {
            ifelse_list->push_back(if_node->stmt2);
        }
        for (auto stmt : *ifelse_list)
        {
            EnterScopeFn(stmt); //if 内层作用域
            genrateStmt(stmt);
            ExitScopeFn();
        }
    }
    else if (nd->type == If)
    {
        top = new SymbolTable(top, NULL); // 新作用域
        //top = runningTop; // test

        list<Node *> *ifelse_list = new list<Node *>();
        ifNode *if_node = (ifNode *)nd;
        Constant *bool_result = getOperationResult(if_node->exp);
        if(!bool_result){
            return;
        }
        if (bool_result->bval)
        {
            ifelse_list->push_back(if_node->stmt);
        }
        for (auto stmt : *ifelse_list)
        {
            EnterScopeFn(stmt); //if 内层作用域
            genrateStmt(stmt);
            ExitScopeFn();
        }
    }
}
//模拟for循环执行过程
void generateForConstant(forNode *for_nd)
{

    //top = new SymbolTable(top,NULL);
    //top = runningTop; // test
    /*获得for循环中的init，cond和next值 目前只处理for循环中数据是 整型 的情况 */
    Constant *initial = NULL;// = MAX_INF;
    Constant *condition = NULL; //= MAX_INF;
    //forNode *for_nd = (forNode *)nd;
    Node *init = for_nd->init;
    expNode *cond = for_nd->cond;
    expNode *next = for_nd->next;
    string con_op;
    string con_id;
    list<Node *> *stmts = NULL;

    Variable *init_v;

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
        //assert(con_init->type == constant && ((constantNode *)con_init)->style == "long long");
        initial = getOperationResult(con_init); // ((constantNode *)con_init)->llval; //todo 支持浮点数
        con_id = id_nd->name;
        init_v = new Variable("long long", con_id, initial);
    }
    else if (init->type == Binop)
    {
        binopNode *init_b = (binopNode *)(init);
        Node *left = init_b->left;
        if (init_b->op.compare("=") == 0)
        {
            con_id = ((idNode *)left)->name;
        }
        init_v = top->LookupIdentifySymbol(con_id);
        Constant *con_init = getOperationResult(init_b->right);
        //binopNode *init_b = (binopNode *)init;
        //assert(init_b->right->type == constant);
        //constantNode *con_init = (constantNode *)(init_b->right);
        //assert(con_init->style == "integer");
        initial = con_init;
        init_v->value = con_init;
    }
    else if (init->type == Id)
    {
        init_v = top->LookupIdentifySymbol(((idNode *)init)->name);
        initial = init_v->value;
    }

    /* 获取cond值 */
    if (cond->type == Binop)
    {
        binopNode *cond_b = (binopNode *)cond;
        //assert(cond_b->right->type == constant); todo
        //assert(con_cond->style == "integer");
        Constant *con_cond = getOperationResult(cond_b->right);
        condition = con_cond;
        
        con_op = cond_b->op;
        if (cond_b->op == "<=" || cond_b->op == ">=")
            condition = getResult("+",condition,new Constant(1));
    }
    //cout << "init= " << initial << " cond= " << condition << endl;
    if (!initial || !condition)
    {
        cout << "init or condition is not defined !";
        exit(-1);
    }
    /* 获取next值 */
    vector<Constant *> step_value; //存储每次循环变量的值,(maybe不支持循环体内部改变该值)
    if (next->type == Binop)
    {
        binopNode *next_b = ((binopNode *)next);
        Node *right = next_b->right;
        string op;
        Constant *step;

        if (right->type == constant)
        {
            step = getOperationResult(right);
            
            op = next_b->op;
        }
        else if (right->type == Binop)
        { // 解析 i = i + x;
            Node *next_left = ((binopNode *)right)->left;
            Node *next_right = ((binopNode *)right)->right;
            Constant *step_v;
            if (((idNode *)next_left)->name.compare(con_id) != 0)
            {
                step = getOperationResult(next_left);
            }
            else
            {
                step = getOperationResult(next_right);
            }
            op = ((binopNode *)right)->op;
        }
        if (con_op.compare("<") == 0 || con_op.compare("<=") == 0)
        {
            if (op == "*=" || op == "*")
            {
                int cnt = 0;
                if (getResult("<",initial,condition)->bval)
                {
                    while(getResult("<",initial,condition)->bval){
                        step_value.push_back(copyConstant(initial));
                        initial = getResult("*",initial,step);
                    }
                }
            }
            else if (op == "/=" || op == "/")
            {
                if (getResult("<",initial,condition)->bval && getResult("<",step,new Constant(1))->bval)
                {
                    while(getResult("<",initial,condition)->bval){
                        step_value.push_back(copyConstant(initial));
                        initial = getResult("/",initial,step);
                    }
                }
                else
                {
                    cout << " infinite loop " << endl;
                }
            }
            else if (op == "+=" || op == "+")
            {
                if (getResult("<",initial,condition)->bval && getResult(">",step,new Constant(0))->bval)
                {
                    while(getResult("<",initial,condition)->bval){
                        step_value.push_back(copyConstant(initial));
                        initial = getResult("+",initial,step);
                    }
                }
                else
                {
                    cout << " infinite loop " << endl;
                }
            }
            else if (op == "-=" || op == "-")
            {
                if (getResult("<",initial,condition)->bval && getResult("<",step,new Constant(0))->bval)
                {
                    while(getResult("<",initial,condition)->bval){
                        step_value.push_back(copyConstant(initial));
                        initial = getResult("-",initial,step);
                    }
                }
                else
                {
                    cout << " infinite loop " << endl;
                }
            }
        }
        else
        {
            if (op == "*=" || op == "*")
            {
                if (getResult(">",initial,condition)->bval && getResult("<",step,new Constant(1))->bval)
                {
                    while(getResult(">",initial,condition)->bval){
                        step_value.push_back(copyConstant(initial));
                        initial = getResult("*",initial,step);
                    }
                }
                else
                {
                    cout << " infinite loop " << endl;
                }
            }
            else if (op == "/=" || op == "/")
            {
                if (getResult(">",initial,condition)->bval)
                {
                    while(getResult(">",initial,condition)->bval){
                        step_value.push_back(copyConstant(initial));
                        initial = getResult("/",initial,step);
                    }
                }
                else
                {
                    cout << " infinite loop " << endl;
                }
            }
            else if (op == "+=" || op == "+")
            {
                if (getResult(">",initial,condition)->bval && getResult("<",step,new Constant(0))->bval)
                {
                    while(getResult(">",initial,condition)->bval){
                        step_value.push_back(copyConstant(initial));
                        initial = getResult("+",initial,step);
                    }
                }
                else
                {
                    cout << " infinite loop " << endl;
                }
            }
            else if (op == "-=" || op == "-")
            {
                if (getResult(">",initial,condition)->bval && getResult(">",step,new Constant(1))->bval)
                {
                    while(getResult(">",initial,condition)->bval){
                        step_value.push_back(copyConstant(initial));
                        initial = getResult("-",initial,step);
                    }
                }
                else
                {
                    cout << " infinite loop " << endl;
                }
            }
        }
    }
    else if (next->type == Unary)
    {
        unaryNode *next_u = (unaryNode *)(next);
        int cnt = 0;
        int i;
        Constant *step = new Constant(1);
        if (next_u->op.compare("POSTINC") == 0)
        { //++
            if (con_op.compare("<") == 0 || con_op.compare("<=") == 0)
            {
                if (getResult("<",initial,condition)->bval)
                {
                    while(getResult("<",initial,condition)->bval){
                        step_value.push_back(copyConstant(initial));
                        initial = getResult("+",initial,step);
                    }
                }
                else
                {
                    cout << " infinite loop " << endl;
                }
            }
            else
            {
                cout << " infinite loop " << endl;
                exit(-1);
            }
        }
        else if (next_u->op.compare("POSTDEC") == 0)
        { //--
            if (con_op.compare(">") == 0 || con_op.compare(">=") == 0)
            {
                if (getResult(">",initial,condition)->bval)
                {
                    while(getResult(">",initial,condition)->bval){
                        step_value.push_back(copyConstant(initial));
                        initial = getResult("-",initial,step);
                    }
                }
                else
                {
                    cout << " infinite loop " << endl;
                }
            }
            else
            {
                cout << " infinite loop " << endl;
                exit(-1);
            }
        }
    }
    for (int i = 0; i < step_value.size(); i++)
    {
        list<Node *> stmt;
        init_v->value = step_value[i];
        if(i == 84){
            int j = 1;
        }
        EnterScopeFn(for_nd->stmt); // 每一次for循环是一个新的作用域
        genrateStmt(for_nd->stmt);
        ExitScopeFn();
        //stmt.push_back(for_nd->stmt);
        //addComposite(stmt);
    }
}

// 解析 语句
void genrateStmt(Node *stmt)
{
    if (stmt == NULL)
        return;
    switch (stmt->type)
    {
    // exp 节点
    case Binop:
    {
        Node *left = static_cast<binopNode *>(stmt)->left;
        Node *right = static_cast<binopNode *>(stmt)->right;
        string op = static_cast<binopNode *>(stmt)->op;
        if (op.compare(".") == 0)
        {
            // 对于 点运算  比如: 取 stream 中的 变量
            //Node* stream_node = checkIdentify(left);
            //checkStreamIdentify(stream_node,right);
        }
        else
        {
            genrateStmt(left);
            genrateStmt(right);
        }
        // 常量传播
        if (ifConstantFlow)
        {
            if (op.compare("=") == 0)
            {
                Variable *variable;
                if (left->type == Binop)
                {
                    //todo 处理 . 运算
                    //expNode *array_left = static_cast<binopNode *>(left)->left;
                    //expNode *array_right = static_cast<binopNode *>(left)->right;
                }
                else if (left->type == Id)
                {
                    if (right->type == Binop)
                    {
                        if (static_cast<binopNode *>(right)->op.compare(".") == 0)
                        {
                            return;
                        }
                    }

                    variable = top->LookupIdentifySymbol(static_cast<idNode *>(left)->name);
                    if (variable == NULL)
                    {
                        break;
                    }
                    bool isArray = false;
                    int index = 0;
                    bool canGetIndex = true;
                    if (((idNode *)left)->arg_list.size())
                    {
                        isArray = true;
                        //处理数组下标
                        vector<int> arg_size = ((ArrayConstant *)variable->value)->arg_size;
                        vector<int> each_size;

                        for (int i = arg_size.size(); i >= 1; i--)
                        {
                            if (each_size.size())
                            {
                                each_size.push_back(arg_size[i] * each_size.back());
                            }
                            else
                            {
                                each_size.push_back(1);
                            }
                        }

                        int array_size = arg_size.size() - 1;
                        int level = 0;
                        for (auto i : ((idNode *)left)->arg_list)
                        {
                            int size;
                            Constant *value = getOperationResult(i);
                            if (value)
                            {
                                size = value->llval;
                            }
                            else
                            {
                                canGetIndex = false;
                                break;
                            }
                            index += each_size[array_size - level] * size;
                            level++;
                        }
                    }

                    //类型隐式转换
                    Constant *value_constant = getOperationResult(right);
                    if (value_constant)
                    {
                        if (variable->type.compare("int") == 0 || variable->type.compare("long") == 0 || variable->type.compare("long long") == 0)
                        {
                            if (value_constant->type.compare("double") == 0)
                            {
                                value_constant->type = "long long";
                                value_constant->llval = (int)value_constant->dval;
                            }
                            if (value_constant->type.compare("float") == 0)
                            {
                                value_constant->type = "long long";
                                value_constant->llval = (int)value_constant->fval;
                            }
                        }
                    }
                    if (isArray)
                    {
                        if (canGetIndex)
                        {
                            if(index > (((ArrayConstant *)(variable->value))->values).size()-1){
                                cout<<"数组越界";
                                exit(-1);
                            }
                            ((ArrayConstant *)variable->value)->values[index] = value_constant;
                        }
                    }
                    else
                    {
                        variable->value = value_constant;
                    }
                }
            }
        }
        break;
    }
    case Unary:
    {

        break;
    }
    case Ternary:
    {
        ternaryNode *ternary_node  = (ternaryNode *)stmt;
        expNode *first = ternary_node->first;
        expNode *second = ternary_node->second;
        expNode *third = ternary_node->third;
        break;
    }
    case Paren:
    {
        break;
    }
    case Cast:
    {
        break;
    }
    case Id:
    {
        checkIdentify(stmt);
        break;
    }
    case constant:
    {
        break;
    }
    case Decl:
    {
        generateDeclareNode(static_cast<declareNode *>(stmt));
        break;
    }
    case StrDcl:
    {
        generateStrDlcNode(static_cast<strdclNode *>(stmt));
        break;
    }
    case Operator_:
    {
        right_opt = static_cast<operatorNode *>(stmt);
        top->InsertOperatorSymbol(static_cast<operatorNode *>(stmt)->operName, static_cast<operatorNode *>(stmt));
        EnterScopeFn(stmt);
        bool storage_ifconstantflow = ifConstantFlow;
        ifConstantFlow = false;
        generatorOperatorNode(static_cast<operatorNode *>(stmt)); //解析 operator 节点
        ifConstantFlow = storage_ifconstantflow;
        ExitScopeFn();
        break;
    }
    case Block:
    {
        generatorBlcokNode(static_cast<blockNode *>(stmt));
        break;
    }
    case While:
    {
        EnterScopeFn(stmt);
        genrateStmt(static_cast<whileNode *>(stmt)->exp);
        genrateStmt(static_cast<whileNode *>(stmt)->stmt);
        ExitScopeFn();
        break;
    }
    case Do:
    {
        EnterScopeFn(stmt);
        genrateStmt(static_cast<doNode *>(stmt)->exp);
        genrateStmt(static_cast<doNode *>(stmt)->stmt);
        ExitScopeFn();
        break;
    }
    case For:
    {
        EnterScopeFn(stmt);
        forNode *for_nd = static_cast<forNode *>(stmt);
        if (ifConstantFlow)
        {
            generateForConstant(for_nd);
        }
        else
        {
            genrateStmt(for_nd->init);
            genrateStmt(for_nd->cond);
            genrateStmt(for_nd->next);
            genrateStmt(for_nd->stmt);
        }
        ExitScopeFn();
        break;
    }
    case If:
    {
        ifNode *if_node = (ifNode *)stmt;
        if (ifConstantFlow)
        {
            generateIfConstant(stmt);
        }
        else
        {
            genrateStmt(if_node->exp);
            genrateStmt(if_node->stmt);
        }
        break;
    }
    case IfElse:
    {
        ifElseNode *if_node = (ifElseNode *)stmt;
        if (ifConstantFlow)
        {
            generateIfConstant(stmt);
        }
        else
        {
            genrateStmt(if_node->exp);
            genrateStmt(if_node->stmt1);
            genrateStmt(if_node->stmt2);
        }
        break;
    }
    case Add:
    {
        addNode *add = static_cast<addNode *>(stmt);
        genrateStmt(add->content);
        break;
    }
    case Call:
    {
        funcDclNode *func = top->LookupFunctionSymbol(static_cast<callNode *>(stmt)->name);
        // print pow 等函数调用 如何与自定义函数 区分
        //func != NULL;
        static_cast<callNode *>(stmt)->actual_callnode = func;
        if (ifConstantFlow)
        {
            //jiaru
        }
        // 检查传入的参数是否存在
        break;
    }
    case CompositeCall:
    {
        if (static_cast<callNode *>(stmt)->name == "FileReader") {
            compositeNode *comp = makeFileReader((compositeCallNode *)stmt);
            S.InsertCompositeSymbol(static_cast<callNode *>(stmt)->name,comp);
            ((compositeCallNode *)stmt) -> actual_composite = comp;
        }
        compositeNode *actual_comp = S.LookupCompositeSymbol(static_cast<compositeCallNode *>(stmt)->compName)->composite;
        static_cast<compositeCallNode *>(stmt)->actual_composite = actual_comp;
        if (ifConstantFlow)
        {
            
            if(right_compositecall_list){
                Node *copy = workNodeCopy(stmt);
                compositeVariableReplace(copy);
                right_compositecall_list->push_back(copy);
            }
            
        }
        // 检查传入的参数是否存在 以及 获得参数值
        break;
    }
    case SplitJoin:
    {
        if(((splitjoinNode*)stmt)->compositeCall_list.size() != 0){
            break;
        }
        if (ifConstantFlow)
        {
            vector<Node *> compositeCall_list;
            compositecall_list_stack.push_back(compositeCall_list);
            right_compositecall_list = &(compositecall_list_stack.back());//&compositeCall_list;
            Node *copy = workNodeCopy(stmt);

            generatorSplitjoinNode(static_cast<splitjoinNode *>(copy));
            if (compositecall_list_stack.size() > 2)
            {
                ((splitjoinNode *)copy)->compositeCall_list = *right_compositecall_list;
            }
            else
            {
                ((splitjoinNode *)stmt)->compositeCall_list = *right_compositecall_list;
            }

            compositecall_list_stack.pop_back();
            if (compositecall_list_stack.size())
            {
                right_compositecall_list = &(compositecall_list_stack.back());
                right_compositecall_list->push_back(copy);
            }
        }
        else
        {
            generatorSplitjoinNode(static_cast<splitjoinNode *>(stmt));
        }
        break;
    }
    case Pipeline:
    {
        if(((pipelineNode*)stmt)->compositeCall_list.size() != 0){
            break;
        }
        if (ifConstantFlow)
        {
            vector<Node *> compositeCall_list;
            compositecall_list_stack.push_back(compositeCall_list);
            right_compositecall_list = &(compositecall_list_stack.back());//&compositeCall_list;
            Node *copy = workNodeCopy(stmt);

            generatorPipelineNode(static_cast<pipelineNode *>(copy));
            if (compositecall_list_stack.size() > 2)
            {
                ((pipelineNode *)copy)->compositeCall_list = *right_compositecall_list;
            }
            else
            {
                ((pipelineNode *)stmt)->compositeCall_list = *right_compositecall_list;
            }

            compositecall_list_stack.pop_back();
            if (compositecall_list_stack.size())
            {
                right_compositecall_list = &(compositecall_list_stack.back());
                right_compositecall_list->push_back(copy);
            }
        }
        else
        {
            generatorPipelineNode(static_cast<pipelineNode *>(stmt));
        }
        break;
    }
    default:
        break;
    }
}

// 解析声明语句中的 常量 初始化部分
Constant *generateInitNode(Node *init_value)
{
    if (init_value != NULL)
    {
        switch (init_value->type)
        {
        case Initializer:
        {
            list<Node *> init_values = static_cast<initNode *>(init_value)->value;
            for (auto it = init_values.begin(); it != init_values.end(); it++)
            {
                return generateInitNode(*it);
            }
            break;
        }
        default:
        {
            genrateStmt(init_value);
            return getOperationResult(init_value);
            break;
        }
        }
    }
}

// 解析声明语句中 数组的初始化
void generateInitArray(Node *init_value, vector<Constant *> &values)
{
    //ArrayConstant *array = new ArrayConstant(id->valType);
    //vector<Constant *> *values = new vetor<Constant *>();
    if (init_value != NULL)
    {
        switch (init_value->type)
        {
        case Initializer:
        {
            list<Node *> init_values = static_cast<initNode *>(init_value)->value;
            for (auto it = init_values.begin(); it != init_values.end(); it++)
            {
                if ((*it)->type == Initializer)
                { //多维数组
                    generateInitArray(*it, values);
                }
                else
                {
                    values.push_back(generateInitNode(*it));
                }
            }
            break;
        }
        default:
        {
            genrateStmt(init_value);
            values.push_back(getOperationResult(init_value));
            break;
        }
        }
    }
    //return values;
}
// 解析 Declare 节点
void generateDeclareNode(declareNode *dlcNode)
{
    list<idNode *> id_list = dlcNode->id_list;
    primNode *prim_node = dlcNode->prim;
    string val_type = prim_node->name;
    //generatorIdList(id_list);
    for (auto it = id_list.begin(); it != id_list.end(); it++)
    {
        // 处理初始化值
        Node *init_value = (*it)->init;
        if ((*it)->isArray)
        {
            ArrayConstant *array = new ArrayConstant((*it)->valType);
            int array_size = 1;
            for (auto arg : (*it)->arg_list)
            {
                Constant *arg_value = getOperationResult(arg);
                if (arg_value)
                {
                    array->arg_size.push_back(arg_value->llval);
                    array_size *= arg_value->llval;
                }
            }
            vector<Constant *> array_values;
            if (array_size)
            {
                if (init_value)
                {
                    generateInitArray(init_value, array_values);
                }
                else
                {
                    array_values.resize(array_size);
                }
            }
            array->values = array_values;
            Variable *variable = new Variable(val_type, (*it)->name, array);
            top->InsertIdentifySymbol(variable);
        }
        else
        {
            Constant *value_constant = generateInitNode(init_value); // 解析初始化值
                                                                     //todo 类型隐式转换
            string val_type = dlcNode->prim->name;
            if (value_constant)
            {
                if (val_type.compare("int") == 0 || val_type.compare("long") == 0 || val_type.compare("long long") == 0)
                {
                    if (value_constant->type.compare("double") == 0)
                    {
                        value_constant->type = "long long";
                        value_constant->llval = (int)value_constant->dval;
                    }
                    if (value_constant->type.compare("float") == 0)
                    {
                        value_constant->type = "long long";
                        value_constant->llval = (int)value_constant->fval;
                    }
                }
                if (val_type.compare("float") == 0)
                {
                    if (value_constant->type.compare("double") == 0)
                    {
                        value_constant->type = "float";
                        value_constant->fval = (float)value_constant->dval;
                    }
                    if (value_constant->type.compare("int") == 0)
                    {
                        value_constant->type = "float";
                        value_constant->fval = (float)value_constant->ival;
                    }
                    if (value_constant->type.compare("long long") == 0)
                    {
                        value_constant->type = "float";
                        value_constant->fval = (float)value_constant->llval;
                    }
                    if (value_constant->type.compare("long") == 0)
                    {
                        value_constant->type = "float";
                        value_constant->fval = (float)value_constant->lval;
                    }
                }
                if (val_type.compare("double") == 0)
                {
                    if (value_constant->type.compare("float") == 0)
                    {
                        value_constant->type = "double";
                        value_constant->dval = (double)value_constant->fval;
                    }
                    if (value_constant->type.compare("int") == 0)
                    {
                        value_constant->type = "double";
                        value_constant->dval = (double)value_constant->ival;
                    }
                    if (value_constant->type.compare("long long") == 0)
                    {
                        value_constant->type = "double";
                        value_constant->dval = (double)value_constant->llval;
                    }
                    if (value_constant->type.compare("long") == 0)
                    {
                        value_constant->type = "double";
                        value_constant->dval = (double)value_constant->lval;
                    }
                }
            }
            Variable *variable = new Variable(val_type, (*it)->name, value_constant);
            top->InsertIdentifySymbol(variable);
        }
        if (isOperatorState)
        {
            operator_state_identify.insert(make_pair((*it)->name, *it));
        }
    }
}

// 处理 stream 声明变量的语句
void generateStrDlcNode(strdclNode *streamDeclearNode)
{ //stream "<int x,int y>" 这部分
    list<idNode *> id_list = streamDeclearNode->declare_stream_id;
    for (auto it = id_list.begin(); it != id_list.end(); it++)
    {
        // 创建一个 stream 声明节点
        inOutdeclNode *stream_dlc = new inOutdeclNode();
        stream_dlc->strType = streamDeclearNode;
        stream_dlc->id = new idNode((*it)->name);
        stream_dlc->type = InOutdcl;
        top->InsertStreamSymbol(stream_dlc);
    }
}
void generatorBlcokNode(blockNode *blockNode)
{
    list<Node *> stmt_list = blockNode->stmt_list;
    if (&stmt_list != NULL)
    {
        for (auto it = stmt_list.begin(); it != stmt_list.end(); it++)
        {
            genrateStmt(*it);
        }
    }
}

void genertatorExpList(list<Node *> *exp_list)
{
    for (auto it = exp_list->begin(); it != exp_list->end(); it++)
    {
        // 需要实现 值的获取
        genrateStmt(*it);
    }
}
// 解析 window
void generatorWindow(winStmtNode *winstmtNode)
{
    checkIdentify(winstmtNode);
    Node *winType = winstmtNode->winType;
    switch (winType->type)
    {
    case Sliding:
    {
        list<Node *> *exp_list = static_cast<slidingNode *>(winType)->arg_list;
        if (exp_list != NULL)
        {
            genertatorExpList(exp_list);
        }
        break;
    }
    case Tumbling:
    {
        list<Node *> *exp_list = static_cast<tumblingNode *>(winType)->arg_list;
        if (exp_list != NULL)
        {
            genertatorExpList(exp_list);
        }
        break;
    }
    }
}

// 解析 operator 节点
void generatorOperatorNode(operatorNode *optNode)
{
    list<Node *> *input_List = optNode->inputs;   //输入输出参数
    list<Node *> *output_List = optNode->outputs; //
    operBodyNode *body = optNode->operBody;       //body

    right_opt_symboltable = top; // 保存oprator的作用域,因为 work 外部变量 都保存在此作用域下

    if (input_List != NULL)
    { //检查
        for (auto it = input_List->begin(); it != input_List->end(); it++)
        {
            checkIdentify(*it);
        }
    }
    if (output_List != NULL)
    { //检查
        for (auto it = output_List->begin(); it != output_List->end(); it++)
        {
            checkIdentify(*it);
        }
    }

    if (body != NULL)
    {
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
        if (&stmt_list != NULL)
        {
            for (auto it = stmt_list.begin(); it != stmt_list.end(); it++)
            {
                genrateStmt(*it);
            }
        }
        isOperatorState = false;

        if (init != NULL)
        {
            generatorBlcokNode(static_cast<blockNode *>(init));
        }

        if (work != NULL)
        {
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
        if (win)
        {
            list<Node *> *win_list = win->win_list;
            if (win_list != NULL)
            {
                for (auto it = win_list->begin(); it != win_list->end(); it++)
                {
                    generatorWindow(static_cast<winStmtNode *>(*it));
                }
            }
        }
    }
}

void generatorSplitjoinNode(splitjoinNode *splitjoinNode)
{
    list<Node *> *outputs = splitjoinNode->outputs;
    list<Node *> *inputs = splitjoinNode->inputs;
    splitNode *split = splitjoinNode->split;
    joinNode *join = splitjoinNode->join;
    list<Node *> *stmt_list = splitjoinNode->stmt_list;
    list<Node *> *body_stmts = splitjoinNode->body_stmts;

    //检查 输入输出
    /*  1.argument.expression.list是一个identifier
        2.查找符号表 identifier是否出现过 */
    if (outputs != NULL)
    {
        for (auto it = outputs->begin(); it != outputs->end(); it++)
        {
            checkIdentify(*it);
        }
    }
    if (inputs != NULL)
    {
        for (auto it = inputs->begin(); it != inputs->end(); it++)
        {
            checkIdentify(*it);
        }
    }

    if (split != NULL)
    {
        Node *node = split->dup_round;
        if (node->type == RoundRobin)
        {
            list<Node *> *exp_list = static_cast<roundrobinNode *>(node)->arg_list;
            if (exp_list != NULL)
            {
                genertatorExpList(exp_list);
            }
        }
        else if (node->type == Duplicate)
        {
            // 需要实现 值的获取
            if (static_cast<duplicateNode *>(node)->exp != NULL)
            {
                genrateStmt(static_cast<duplicateNode *>(node)->exp);
            }
        }
    }
    if (stmt_list != NULL)
    {
        for (auto it = stmt_list->begin(); it != stmt_list->end(); it++)
        {
            genrateStmt(*it);
        }
    }

    if (body_stmts != NULL)
    {
        for (auto it = body_stmts->begin(); it != body_stmts->end(); it++)
        {
            genrateStmt(*it);
        }
    }

    if (join != NULL)
    {
        list<Node *> *exp_list = join->rdb->arg_list;
        if (exp_list != NULL)
        {
            genertatorExpList(exp_list);
        }
    }
}

// 解析 pipeline 节点
void generatorPipelineNode(pipelineNode *pipelineNode)
{
    list<Node *> *outputs = pipelineNode->outputs;
    list<Node *> *inputs = pipelineNode->inputs;
    list<Node *> *body_stmts = pipelineNode->body_stmts;

    if (outputs != NULL)
    {
        for (auto it = outputs->begin(); it != outputs->end(); it++)
        {
            checkIdentify(*it);
        }
    }
    if (inputs != NULL)
    {
        for (auto it = inputs->begin(); it != inputs->end(); it++)
        {
            checkIdentify(*it);
        }
    }

    if (body_stmts != NULL)
    {
        for (auto it = body_stmts->begin(); it != body_stmts->end(); it++)
        {
            genrateStmt(*it);
        }
    }
}

//入口 传入整棵 AST树
void generateSymbolTable(list<Node *> *program, SymbolTable *symbol_tables[][MAX_SCOPE_DEPTH])
{
    for (int i = 0; i < MAX_SCOPE_DEPTH; i++)
    {
        for (int j = 0; j < MAX_SCOPE_DEPTH; j++)
        {
            symbol_tables[i][j] = NULL;
        }
    }

    YYLTYPE *loc = new YYLTYPE();
    loc->first_column = 0;
    loc->last_column = __INT_MAX__;
    loc->first_line = 0;
    loc->last_line = __INT_MAX__;

    S = *new SymbolTable(NULL, loc);
    symbol_tables[0][0] = &S;
    top = &S;
    program != NULL;
    for (auto it : *(program))
    {
        switch (it->type)
        {
        case Decl:
        {
            generateDeclareNode(static_cast<declareNode *>(it));
            break;
        }
        case Composite:
        {
            top->InsertCompositeSymbol(static_cast<compositeNode *>(it)->compName, static_cast<compositeNode *>(it));
            //EnterScopeFn(it);/* 进入 composite 块级作用域 */
            //generateComposite(static_cast<compositeNode *>(it));
            //ExitScopeFn(); /* 退出 composite 块级作用域 */
            break;
        }
        case FuncDcl:
        { // 仅支持全局作用域下的函数声明
            top->InsertFunctionSymbol(static_cast<funcDclNode *>(it));
            //以下实现函数内部解析

            //
        }
        }
    }
}

// 解析 Composite 节点
void generateComposite(compositeNode *composite)
{
    ComInOutNode *inout = composite->head->inout; //输入输出参数
    compBodyNode *body = composite->body;         //body
    paramNode *param;
    list<Node *> *body_stmt;

    if (body != NULL)
    {
        param = body->param; // param
        body_stmt = body->stmt_List;

        //解析 输入输出流 stream 作为参数加入符号表
        if (inout != NULL)
        {
            list<Node *> *input_List = inout->input_List;   //原始输入流
            list<Node *> *output_List = inout->output_List; //原始输出流
            if (input_List != NULL)
            {
                for (auto it = input_List->begin(); it != input_List->end(); it++)
                {
                    inOutdeclNode *copy_inoutdeclNode = new inOutdeclNode();
                    copy_inoutdeclNode->strType = static_cast<inOutdeclNode *>(*it)->strType;
                    idNode *copy_id = new idNode(static_cast<inOutdeclNode *>(*it)->id->name);
                    copy_inoutdeclNode->id = copy_id;
                    copy_inoutdeclNode->isInOut = true;
                    top->InsertStreamSymbol(copy_inoutdeclNode);
                }
            }
            if (output_List != NULL)
            {
                for (auto it = output_List->begin(); it != output_List->end(); it++)
                {
                    inOutdeclNode *copy_inoutdeclNode = new inOutdeclNode();
                    copy_inoutdeclNode->strType = static_cast<inOutdeclNode *>(*it)->strType;
                    idNode *copy_id = new idNode(static_cast<inOutdeclNode *>(*it)->id->name);
                    copy_inoutdeclNode->id = copy_id;
                    copy_inoutdeclNode->isInOut = true;
                    top->InsertStreamSymbol(copy_inoutdeclNode);
                }
            }
        }

        // 解析 param
        if (param != NULL)
        {
            generateNodeList(*(param->param_list));
        }

        // 解析 body
        if (body_stmt != NULL)
        {
            for (auto it = body_stmt->begin(); it != body_stmt->end(); it++)
            {
                genrateStmt(*it);
            }
        }
    }
}

void printSymbolTable(SymbolTable *symbol_tables[][MAX_SCOPE_DEPTH])
{
    for (int i = 0; i < MAX_SCOPE_DEPTH; i++)
    {
        for (int j = 0; j < MAX_SCOPE_DEPTH; j++)
        {
            if (symbol_tables[i][j] != NULL)
            {
                cout << "[Symbol Table] -- Level -- " << i << " Version -- " << j << "--------" << endl;
                symbol_tables[i][j]->printSymbolTables();
                cout << endl;
            }
        }
    }
}

SymbolTable *generateCompositeRunningContext(compositeCallNode *call, compositeNode *composite, list<Constant *> paramList, list<Node *> *inputs, list<Node *> *outputs)
{
    if(compositecall_list_stack.size() == 0){
        compositecall_list_stack.push_back(*right_compositecall_list);
    }
    if (call)
    {
        if (call->scope)
        {
            top = new SymbolTable(call->scope, NULL); //call->scope 上一层执行上下文
        }
        else
        {
            top = new SymbolTable(&S, NULL);
        }
        top->count = call->count;
    }
    else
    {
        top = new SymbolTable(&S, NULL);
        top->count = 0;
    }

    ComInOutNode *inout = composite->head->inout; //输入输出参数
    compBodyNode *body = composite->body;         //body
    paramNode *param;
    list<Node *> *body_stmt;

    if (body != NULL)
    {
        param = body->param; // param
        body_stmt = body->stmt_List;

        //解析 输入输出流 stream 作为参数加入符号表
        if (inout != NULL)
        {
            list<Node *> *input_List = inout->input_List;   //原始输入流
            list<Node *> *output_List = inout->output_List; //原始输出流
            if (input_List != NULL)
            {
                for (auto it = input_List->begin(); it != input_List->end(); it++)
                {
                    inOutdeclNode *copy_inoutdeclNode = new inOutdeclNode();
                    copy_inoutdeclNode->strType = static_cast<inOutdeclNode *>(*it)->strType;
                    idNode *copy_id = new idNode(static_cast<inOutdeclNode *>(*it)->id->name);
                    copy_inoutdeclNode->id = copy_id;
                    copy_inoutdeclNode->isInOut = true;
                    top->InsertStreamSymbol(copy_inoutdeclNode);
                }
            }
            if (output_List != NULL)
            {
                for (auto it = output_List->begin(); it != output_List->end(); it++)
                {
                    inOutdeclNode *copy_inoutdeclNode = new inOutdeclNode();
                    copy_inoutdeclNode->strType = static_cast<inOutdeclNode *>(*it)->strType;
                    idNode *copy_id = new idNode(static_cast<inOutdeclNode *>(*it)->id->name);
                    copy_inoutdeclNode->id = copy_id;
                    copy_inoutdeclNode->isInOut = true;
                    top->InsertStreamSymbol(copy_inoutdeclNode);
                }
            }
        }
        // 解析 param
        if (param != NULL)
        {
            generateNodeList(*(param->param_list));
        }
    }

    //generateComposite(composite);//进行常量传播

    //compBodyNode *body = composite->body; //body
    //paramNode *param;

    //在符号表中将数据流替换为真实数据流
    if (composite->head->inout)
    {
        list<Node *> *comp_inputs = composite->head->inout->input_List;
        list<Node *> *comp_outputs = composite->head->inout->output_List;

        //生成 stream 符号表
        if (comp_inputs)
        {
            auto comp_it = comp_inputs->begin();
            for (auto it : *inputs)
            {
                string comp_name = (((inOutdeclNode *)(*comp_it))->id)->name; //composite中的参数名
                string real_name;

                inOutdeclNode *real_stream = runningTop->LookUpStreamSymbol(((idNode *)it)->name); //compositecall传入的参数名
                if (real_stream)
                {
                    real_name = real_stream->id->name;
                }
                else
                {
                    real_name = ((idNode *)it)->name;
                }

                (top->LookUpStreamSymbol(comp_name))->id->name = real_name;
                comp_it++;
            }
        }
        if (comp_outputs)
        {
            auto comp_it = comp_outputs->begin();
            for (auto it : *outputs)
            {
                string comp_name = (((inOutdeclNode *)(*comp_it))->id)->name;
                string real_name;
                inOutdeclNode *real_stream = runningTop->LookUpStreamSymbol(((idNode *)it)->name); //compositecall传入的参数名
                if (real_stream)
                {
                    real_name = real_stream->id->name;
                }
                else
                {
                    real_name = ((idNode *)it)->name;
                }
                (top->LookUpStreamSymbol(comp_name))->id->name = real_name;
                comp_it++;
            }
        }
    }

    // 传入compositecall参数到param
    list<Constant *>::iterator paramValue = paramList.begin();
    if (body != NULL)
    {
        param = body->param; // param

        if (param != NULL)
        {
            list<Node *> param_list = *(param->param_list);
            for (auto it = param_list.begin(); it != param_list.end(); it++)
            {
                Variable *variable = top->LookupIdentifySymbol(((idNode *)(*it))->name);
                if ((*paramValue)->isArray)
                {
                    variable->isArray = true;
                }
                variable->value = (*paramValue);
                top->InsertParamSymbol(variable);
                paramValue++;
                /*if(variable->type.compare((*paramValue)->type) == 0){ //todo 参数类型匹配
                    variable->value = (*paramValue);
                }else{
                    cout<<"参数类型不匹配";
                    exit(-1);
                }*/
            }
        }
    }

    //先传递composite参数和数据流，再进行body解析，进行常量传播
    if (body_stmt != NULL)
    {
        for (auto it = body_stmt->begin(); it != body_stmt->end(); it++)
        {
            genrateStmt(*it);
        }
    }

    //多次调用同一个 composite,其内部声明的数据流通过count来进行区分
    for (auto it : top->getStreamTable())
    {
        inOutdeclNode *stream = it.second;

        if (!stream->isInOut)
        {
            stream->id->name = composite->compName + stream->id->name + to_string(top->count);
        }
    }
    //top->printSymbolTables();
    return top;
}

list<Constant *> generateStreamList(list<Node *> *stream_List, int target)
{
    list<Constant *> paramList;
    top = FindRightSymbolTable(target);
    for (auto it = stream_List->begin(); it != stream_List->end(); it++)
    {
        paramList.push_back(getOperationResult((*it)));
    }
    return paramList;
}

list<Constant *> generateStreamList(list<Node *> *stream_List, SymbolTable *s)
{
    list<Constant *> paramList;
    top = s;
    for (auto it = stream_List->begin(); it != stream_List->end(); it++)
    {
        paramList.push_back(getOperationResult((*it)));
    }
    return paramList;
}