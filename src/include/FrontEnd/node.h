#ifndef _NODE_H_
#define _NODE_H_
#include "global.h"
#include "nodetype.h"
#include "defines.h"
#include <list>
#include <vector>

class Node
{
  public:
    NodeType type;
    YYLTYPE *loc;
    short pass;
    Node()
    {
        loc = new YYLTYPE();
    }
    virtual ~Node()
    {
        delete loc;
    }
    void setLoc(YYLTYPE loc);
    virtual void print() = 0;
    virtual string toString() = 0;
};

string listToString(list<Node *> list);

class primNode : public Node
{
  public:
    string name;
    bool isConst;
    primNode(string str, YYLTYPE loc) : name(str), isConst(false)
    {
        this->type = primary;
        setLoc(loc);
    }
    ~primNode() {}
    void print() { cout << "primNodeType :" << name << endl; }
    string toString();
};

class constantNode : public Node
{
  public:
    /* 判断是double/long或者string类型 */
    string valueType;
    string sval;
    double dval;
    long long llval;
    constantNode(string type, string str, YYLTYPE loc) : valueType(type), sval(str)
    {
        setLoc(loc);
        this->type = constant;
    }
    constantNode(string type, long long l, YYLTYPE loc) : valueType(type), llval(l)
    {
        setLoc(loc);
        this->type = constant;
    }
    constantNode(string type, double d, YYLTYPE loc) : valueType(type), dval(d)
    {
        setLoc(loc);
        this->type = constant;
    }
    ~constantNode() {}
    void print() { cout << "constant :" << type << endl; }
    string toString();
};
/* expNode向前声明 */
class expNode;

class idNode : public Node
{
  public:
    string name;
    string valType;
    list<Node *> arg_list;
    Node *init;
    int level;
    int version;
    int isArray;  //是否为数组, 主要用于 int a[] 这种 arg_list 为空的场景
    int isStream; //是否为 Stream 复杂类型.后续待处理
    int isParam;  //是否为function 或 composite 的输入参数
    idNode(string name, YYLTYPE loc) : name(name), isArray(0), isStream(0), isParam(0)
    {
        this->type = Id;
        setLoc(loc);
        this->level = Level;
        this->version = current_version[Level];
        this->valType = "int";
    }
    idNode(string *name, YYLTYPE loc)
    {
        new (this) idNode(*name, loc);
    }
    ~idNode() {}
    void print() {}
    string toString();
};

class initNode : public Node
{
  public:
    list<Node *> value;
    initNode(Node *node, YYLTYPE loc)
    {
        value.push_back(node);
        this->type = Initializer;
        setLoc(loc);
    }
    ~initNode() {}
    void print() {}
    string toString();
};

class functionNode : public Node
{
  public:
    functionNode() {}
    ~functionNode() {}
};

class expNode : public Node
{
  public:
    expNode() {}
    ~expNode() {}
    void print() {}
    string toString() {}
};
class arrayNode : public Node
{
  public:
    list<Node *> arg_list;
    arrayNode(expNode *exp, YYLTYPE loc)
    {
        if (exp)
            arg_list.push_back(exp);
        setLoc(loc);
    }
    ~arrayNode() {}
    void print() {}
    string toString() { return string("arrayNode"); }
};

class declareNode : public Node
{
  public:
    primNode *prim;
    list<idNode *> id_list;
    declareNode(primNode *prim, idNode *id, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = Decl;
        this->prim = prim;
        if (id)
            this->id_list.push_back(id);
    }
    ~declareNode() {}
    void print() {}
    string toString() {}
};

class unaryNode : public Node
{
  public:
    expNode *exp;
    string op;
    unaryNode(string op, expNode *exp, YYLTYPE loc)
    {
        setLoc(loc);
        this->exp = exp;
        this->op = op;
    }
    ~unaryNode() {}
    void print() {}
    string toString();
};

class binopNode : public Node
{
  public:
    expNode *left;
    expNode *right;
    string op;
    binopNode(expNode *left, string op, expNode *right, YYLTYPE loc)
    {
        this->type = Binop;
        setLoc(loc);
        this->left = left;
        this->right = right;
        this->op = op;
    }
    ~binopNode() {}
    void print() {}
    string toString();
};

class ternaryNode : public Node
{
  public:
    expNode *first;
    expNode *second;
    expNode *third;
    ternaryNode(expNode *first, expNode *second, expNode *third, YYLTYPE loc)
    {
        setLoc(loc);
        this->type = Ternary;
        this->first = first;
        this->second = second;
        this->third = third;
    }
    ~ternaryNode() {}
    void print() {}
    string toString();
};
class parenNode : public Node
{
  public:
    expNode *exp;
    parenNode(expNode *exp, YYLTYPE loc)
    {
        setLoc(loc);
        this->exp = exp;
    }
    ~parenNode() {}
    void print() {}
    string toString();
};

class castNode : public Node
{
  public:
    primNode *prim;
    expNode *exp;
    castNode(primNode *prim, expNode *exp, YYLTYPE loc)
    {
        setLoc(loc);
        this->type = Cast;
        this->prim = prim;
        this->exp = exp;
    }
    ~castNode() {}
    void print() {}
    string toString();
};

/* switch() case: */
class statNode;
class caseNode : public Node
{
  public:
    expNode *exp;
    statNode *stmt;
    caseNode(expNode *exp, statNode *stmt, YYLTYPE loc)
    {
        setLoc(loc);
        this->type = Case;
        this->exp = exp;
        this->stmt = stmt;
    }
    ~caseNode() {}
    void print() {}
    string toString() {}
};

class defaultNode : public Node
{
  public:
    statNode *stmt;
    defaultNode(statNode *stmt, YYLTYPE loc)
    {
        setLoc(loc);
        this->type = Default;
        this->stmt = stmt;
    }
    ~defaultNode() {}
    void print() {}
    string toString() {}
};

class continueNode : public Node
{
  public:
    continueNode(YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = Continue;
    }
    ~continueNode() {}
    void print() {}
    string toString() {}
};

class breakNode : public Node
{
  public:
    breakNode(YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = Break;
    }
    ~breakNode() {}
    void print() {}
    string toString() {}
};

class returnNode : public Node
{
  public:
    expNode *exp;
    returnNode(expNode *exp, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = Return;
        this->exp = exp;
    }
    ~returnNode() {}
    void print() {}
    string toString() {}
};

class ifNode : public Node
{
  public:
    expNode *exp;
    Node *stmt;
    ifNode(expNode *exp, Node *stmt, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = If;
        this->exp = exp;
        this->stmt = stmt;
    }
    ~ifNode() {}
    void print() {}
    string toString() {}
};

class ifElseNode : public Node
{
  public:
    expNode *exp;
    Node *stmt1;
    Node *stmt2;
    ifElseNode(expNode *exp, Node *stmt1, Node *stmt2, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = IfElse;
        this->exp = exp;
        this->stmt1 = stmt1;
        this->stmt2 = stmt2;
    }
    ~ifElseNode() {}
    void print() {}
    string toString() {}
};

class switchNode : public Node
{
  public:
    expNode *exp;
    statNode *stat;
    switchNode(expNode *exp, statNode *stat, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = Switch;
        this->exp = exp;
        this->stat = stat;
    }
    ~switchNode() {}
    void print() {}
    string toString() {}
};

class whileNode : public Node
{
  public:
    expNode *exp;
    Node *stmt;
    whileNode(expNode *exp, Node *stmt, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = While;
        this->exp = exp;
        this->stmt = stmt;
    }
    ~whileNode() {}
    void print() {}
    string toString() {}
};

class doNode : public Node
{
  public:
    expNode *exp;
    Node *stmt;
    doNode(Node *stmt, expNode *exp, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = Do;
        this->exp = exp;
        this->stmt = stmt;
    }
    ~doNode() {}
    void print() {}
    string toString() {}
};

class forNode : public Node
{
  public:
    Node *init;
    expNode *cond;
    expNode *next;
    Node *stmt;
    forNode(Node *init, expNode *cond, expNode *next, Node *stmt, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = For;
        this->init = init;
        this->cond = cond;
        this->next = next;
        this->stmt = stmt;
    }
    void print() {}
    string toString() {}
};

class blockNode : public Node
{
  public:
    list<Node *> *stmt_list;
    YYLTYPE right;
    blockNode(list<Node *> *stmt_list, YYLTYPE left, YYLTYPE right)
    {
        this->setLoc(left);
        this->right = right;
        this->type = Block;
        this->stmt_list = stmt_list;
    }
    ~blockNode() {}
    void print() {}
    string toString() {}
};

class pipelineNode : public Node
{
  public:
    list<Node *> *split_pipe_stmt_list;
    pipelineNode(list<Node *> *split_pipe_stmt_list, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = Pipeline;
        this->split_pipe_stmt_list = split_pipe_stmt_list;
    }
    ~pipelineNode() {}
    void print() {}
    string toString() {}
};

class roundrobinNode : public Node
{
  public:
    list<Node *> *arg_list;
    roundrobinNode(list<Node *> *arg_list, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = RoundRobin;
        this->arg_list = arg_list;
    }
    ~roundrobinNode() {}
    void print() {}
    string toString() {}
};

class duplicateNode : public Node
{
  public:
    expNode *exp;
    duplicateNode(expNode *exp, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = Duplicate;
        this->exp = exp;
    }
    ~duplicateNode() {}
    void print() {}
    string toString() {}
};

class splitNode : public Node
{
  public:
    Node *dup_round;
    splitNode(Node *dup_round, YYLTYPE loc)
    {
        this->type = Split;
        this->setLoc(loc);
        this->dup_round = dup_round;
    }
    ~splitNode() {}
    void print() {}
    string toString() {}
};

class joinNode : public Node
{
  public:
    roundrobinNode *rdb;
    joinNode(roundrobinNode *rdb, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = Join;
        this->rdb = rdb;
    }
    ~joinNode() {}
    void print() {}
    string toString() {}
};

class splitjoinNode : public Node
{
  public:
    splitNode *split;
    joinNode *join;
    list<Node *> *stmt_list;
    list<Node *> *split_pipe_stmt_list;
    splitjoinNode(splitNode *split, list<Node *> *stmt_list, list<Node *> *split_pipe_stmt_list, joinNode *join, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = SplitJoin;
        this->split = split;
        this->join = join;
        this->stmt_list = stmt_list;
        this->split_pipe_stmt_list = split_pipe_stmt_list;
    }
    ~splitjoinNode() {}
    void print() {}
    string toString() {}
};

class addNode : public Node
{
  public:
    /* content可以为pipeline，splitjoin或者composite调用 */
    Node *content;
    addNode(Node *content, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = Add;
        this->content = content;
    }
    ~addNode() {}
    void print() {}
    string toString() {}
};

class slidingNode : public Node
{
  public:
    list<Node *> *arg_list;
    slidingNode(list<Node *> *arg_list, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = Sliding;
        this->arg_list = arg_list;
    }
    ~slidingNode() {}
    void print() {}
    string toString() {}
};

class tumblingNode : public Node
{
  public:
    list<Node *> *arg_list;
    tumblingNode(list<Node *> *arg_list, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = Tumbling;
        this->arg_list = arg_list;
    }
    ~tumblingNode() {}
    void print() {}
    string toString() {}
};

class OperdclNode : public Node
{
  public:
    string name;
    list<Node *> *arg_list;
    OperdclNode(string name, list<Node *> *arg_list, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = Operdcl;
        this->name = name;
        this->arg_list = arg_list;
    }
    OperdclNode(string *name, list<Node *> *arg_list, YYLTYPE loc)
    {
        new (this) OperdclNode(*name, arg_list, loc);
    }
    ~OperdclNode() {}
    void print() {}
    string toString() {}
};

class strdclNode : public Node
{
  public:
    list<idNode *> id_list;
    strdclNode(idNode *id, YYLTYPE loc)
    {
        this->setLoc(loc);
        if (id)
            id_list.push_back(id);
    }
    ~strdclNode() {}
    void print() {}
    string toString();
};

class winStmtNode : public Node
{
  public:
    Node *winType;
    string winName;
    winStmtNode(string winName, Node *winType, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = WindowStmt;
        this->winName = winName;
        this->winType = winType;
    }

    ~winStmtNode() {}
    void print() {}
    string toString() {}
};

class windowNode : public Node
{
  public:
    list<Node *> *winStmt_list;
    windowNode(list<Node *> *winStmt_list)
    {
        this->type = Window;
        this->winStmt_list = winStmt_list;
    }
    ~windowNode() {}
    void print() {}
    string toString() {}
};

class paramNode;
class operBodyNode : public Node
{
  public:
    paramNode *param;
    list<Node *> *stmt_list;
    Node *init;
    Node *work;
    windowNode *win;
    operBodyNode(list<Node *> *stmt_list, Node *init, Node *work, windowNode *win)
    {
        this->type = OperBody;
        this->stmt_list = stmt_list;
        this->init = init;
        this->work = work;
        this->win = win;
    }
    ~operBodyNode() {}
    void print() {}
    string toString() { return "operBodyNode"; }
};

class callNode : public Node
{
  public:
    string name;
    list<Node *> arg_list;
    callNode(string name, list<Node *> *arg_list, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = Call;
        this->name = name;
        if (arg_list)
            this->arg_list = *arg_list;
    }
    callNode(string *name, list<Node *> *arg_list, YYLTYPE loc)
    {
        new (this) callNode(*name, arg_list, loc);
    }
    ~callNode() {}
    void print() {}
    string toString();
};

class inOutdeclNode : public Node
{
  public:
    Node *strType;
    idNode *id;
    inOutdeclNode(Node *strType, idNode *id, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = InOutdcl;
        this->strType = strType;
    }
    ~inOutdeclNode() {}
    void print() {}
    string toString() {}
};

class ComInOutNode : public Node
{
  public:
    list<Node *> *input_list;
    list<Node *> *output_list;
    ComInOutNode(list<Node *> *input_list, list<Node *> *output_list, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = ComInOut;
        this->input_list = input_list;
        this->output_list = output_list;
    }
    ~ComInOutNode() {}
    void print() {}
    string toString() {}
};

class paramDeclNode : public Node
{
  public:
    primNode *prim;
    idNode *id;
    paramDeclNode(idNode *id, arrayNode *adcl, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = ParamDcl;
        this->prim = prim;
        this->id = id;
    }
    ~paramDeclNode() {}
    void print() {}
    string toString() {}
};
class paramNode : public Node
{
  public:
    list<Node *> *param_list;
    paramNode(list<Node *> *param_list)
    {
        this->type = Param;
        this->param_list = param_list;
    }
    ~paramNode() {}
    void print() {}
    string toString() {}
};

class funcBodyNode : public Node
{
  public:
    list<Node *> *stmt_list;
    funcBodyNode(list<Node *> *stmt_list)
    {
        this->stmt_list = stmt_list;
    }
    ~funcBodyNode() {}
    void print() {}
    string toString() {}
};

class compBodyNode : public Node
{
  public:
    paramNode *param;
    list<Node *> *stmt_list;
    compBodyNode(paramNode *param, list<Node *> *stmt_list)
    {
        this->type = CompBody;
        this->param = param;
        this->stmt_list = stmt_list;
    }
    ~compBodyNode() {}
    void print() {}
    string toString() {}
};

class funcDclNode : public Node
{
  public:
    primNode *prim;
    string name;
    list<Node *> param_list;
    funcBodyNode *funcBody;
    funcDclNode(primNode *prim, string *name, list<Node *> *param_list, funcBodyNode *funcBody)
    {
        this->type = FuncDcl;
        this->prim = prim;
        this->name = *name;
        if (param_list)
            this->param_list = *param_list;
        this->funcBody = funcBody;
    }
    ~funcDclNode() {}
    void print() {}
    string toString();
};

class compositeNode;
class compositeCallNode : public Node
{
  public:
    string name;
    list<Node *> stream_list;
    list<Node *> param_list;
    compositeNode *actual_composite; //保存composite展开节点
    compositeCallNode(string *name, list<Node *> *stream_list, list<Node *> *param_list, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = CompositeCall;
        this->name = *name;
        if (stream_list)
            this->stream_list = *stream_list;
        if (param_list)
            this->param_list = *param_list;
    }
    ~compositeCallNode() {}
    void print() {}
    string toString();
};

class compHeadNode : public Node
{
  public:
    idNode *id;
    string compName;
    ComInOutNode *inout;
    compHeadNode(idNode *id, ComInOutNode *inout)
    {
        this->type = CompHead;
        this->id = id;
        this->inout = inout;
        this->compName = id->name;
    }
    ~compHeadNode() {}
    void print() {}
    string toString() {}
};

class compositeNode : public Node
{
  public:
    compHeadNode *head;
    compBodyNode *body;
    string compName;
    compositeNode(compHeadNode *head, compBodyNode *body)
    {
        this->type = Composite;
        this->head = head;
        this->body = body;
        this->compName = head->compName;
    }
    ~compositeNode() {}
    void print() {}
    string toString() {}
};

class operatorNode : public Node
{
  public:
    string operName;
    list<Node *> arg_list;
    operBodyNode *operBody;
    operatorNode(string operName, list<Node *> *arg_list, operBodyNode *operBody)
    {
        this->type = Operator_;
        this->operName = operName;
        if (arg_list)
            this->arg_list = *arg_list;
        this->operBody = operBody;
    }
    ~operatorNode() {}
    void print() {}
    string toString();
};
#endif
