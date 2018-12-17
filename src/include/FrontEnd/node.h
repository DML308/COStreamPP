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
    /* parenthesized is set on expressions which were parenthesized
	   in the original source:  e.g., (x+y)*(w+z) would have
	   parenthesized==TRUE on both PLUS nodes, and parenthesized==FALSE
	   on both MULT nodes. */
    bool parenthesized;
    Node() {
        loc = new YYLTYPE();
    }
    virtual ~Node() {
        delete loc;
    }
    void setLoc(YYLTYPE loc);
    virtual void print() = 0;
    virtual string toString() = 0;
};

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
    string name;
    string sval;
    double dval;
    long long llval;
    constantNode(string type, string str, YYLTYPE loc) : name(type), sval(str)
    {
        setLoc(loc);
        this->type = constant;
    }
    constantNode(string type, long long l, YYLTYPE loc) : name(type), llval(l)
    {
        setLoc(loc);
        this->type = constant;
    }
    constantNode(string type, double d, YYLTYPE loc) : name(type), dval(d)
    {
        setLoc(loc);
        this->type = constant;
    }
    ~constantNode() {}
    void print() { cout << "constant :" << type << endl; }
    string toString();
};

class idNode : public Node
{
  public:
    string name;
    int level;
    int version;
    idNode(string name, YYLTYPE loc) : name(name)
    {
        this->type = Id;
        setLoc(loc);
        this->level = Level;
        this->version = current_version[Level];
    }
    ~idNode() {}
    void print() {}
    string toString()
    {
        string str = "name:" + name;
        str += "  level:" + to_string(level);
        str += "  version:" + to_string(version);
        return str.c_str();
    }
};

class initNode : public Node
{
  public:
    list<Node *> value;
    initNode(Node *node,YYLTYPE loc)
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

/* expNode向前声明 */
class expNode;
class adclNode : public Node
{
  public:
    string name;
    int size; // 还未用到
    expNode *dim;
    NodeType valType;
    /* 默认1维 */
    adclNode(NodeType valType, expNode *eNode, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = Adcl;
        this->dim = eNode;
        this->valType = valType;
    }
    ~adclNode() {}
    void print() {}
    string toString() {}
};

class expNode : public Node
{
  public:
    expNode() {}
    ~expNode() {}
    void print() {}
    string toString() {}
};

class declareNode : public Node
{
  public:
    primNode *prim;
    list<idNode *> id_List;
    list<adclNode *> adcl_List;
    list<initNode *> init_List;
    declareNode(primNode *prim, idNode *id, adclNode *adcl, initNode *init, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = Decl;
        this->prim = prim;
        this->append(id, adcl, init);
    }
    void append(idNode *id, adclNode *adcl, initNode *init)
    {
        id_List.push_back(id);
        adcl_List.push_back(adcl);
        init_List.push_back(init);
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
    string toString() {}
};
/*  这里pointNode可以和binopnode可以合成一个，但是需要将binopNode的expnode改为node
    因此在这里单独处理
*/
class pointNode : public Node
{
  public:
    Node *assign;
    Node *id;
    pointNode(Node *assign, Node *id, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = Point;
        this->assign = assign;
        this->id = id;
    }
    ~pointNode() {}
    void print() {}
    string toString() {}
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
        op = op;
    }
    ~binopNode() {}
    void print() {}
    string toString() {}
};

class ternaryNode : public Node
{
  public:
    expNode *first;
    expNode *second;
    expNode *third;
    ternaryNode(expNode *first, expNode *second, expNode *thrid, YYLTYPE loc)
    {
        setLoc(loc);
        this->type = Ternary;
        this->first = first;
        this->second = second;
        this->third = third;
    }
    ~ternaryNode() {}
    void print() {}
    string toString() {}
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
    string toString() {}
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
    list<Node *> *stmt_List;
    YYLTYPE right;
    blockNode(list<Node *> *stmt_List, YYLTYPE left, YYLTYPE right)
    {
        this->setLoc(left);
        this->right = right;
        this->type = Block;
        this->stmt_List = stmt_List;
    }
    ~blockNode() {}
    void print() {}
    string toString() {}
};

class pipelineNode : public Node
{
  public:
    list<Node *> *split_pipe_stmt_List;
    pipelineNode(list<Node *> *split_pipe_stmt_List, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = Pipeline;
        this->split_pipe_stmt_List = split_pipe_stmt_List;
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
        this->dup_round=dup_round;
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
    list<Node *> *split_pipe_stmt_List;
    splitjoinNode(splitNode *split, list<Node *> *stmt_list, list<Node *> *split_pipe_stmt_List, joinNode *join, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = SplitJoin;
        this->split = split;
        this->join = join;
        this->stmt_list = stmt_list;
        this->split_pipe_stmt_List = split_pipe_stmt_List;
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
    list<Node *> *arg_List;
    OperdclNode(string name, list<Node *> *arg_List, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = Operdcl;
        this->name = name;
        this->arg_List = arg_List;
    }
    ~OperdclNode() {}
    void print() {}
    string toString() {}
};

class strdclNode : public Node
{
  public:
    list<primNode *> prim_List;
    list<idNode *> id_List;
    list<adclNode *> adcl_List;
    list<idNode *> decl_List;
    strdclNode(primNode *prim, idNode *id, adclNode *adcl, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = StrDcl;
        prim_List.push_back(prim);
        id_List.push_back(id);
        adcl_List.push_back(adcl);
    }
    void append(primNode *prim, idNode *id, adclNode *adcl)
    {
        prim_List.push_back(prim);
        id_List.push_back(id);
        adcl_List.push_back(adcl);
    }
    void insert(idNode *decl)
    {
        decl_List.push_back(decl);
    }
    ~strdclNode() {}
    void print() {}
    string toString() {}
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
    list<Node *> *winStmt_List;
    windowNode(list<Node *> *winStmt_List)
    {
        this->type = Window;
        this->winStmt_List = winStmt_List;
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
    list<Node *> *stmt_List;
    Node *init;
    Node *work;
    windowNode *win;
    operBodyNode(list<Node *> *stmt_List, Node *init, Node *work, windowNode *win)
    {
        this->type = OperBody;
        this->stmt_List = stmt_List;
        this->init = init;
        this->work = work;
        this->win = win;
    }
    ~operBodyNode() {}
    void print() {}
    string toString() {}
};

class callNode : public Node
{
  public:
    string name;
    list<Node *> *arg_List;
    callNode(string name, list<Node *> *arg_List, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = Call;
        this->name = name;
        this->arg_List = arg_List;
    }
    ~callNode() {}
    void print() {}
    string toString() {}
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
    list<Node *> *input_List;
    list<Node *> *output_List;
    ComInOutNode(list<Node *> *input_List, list<Node *> *output_List, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = ComInOut;
        this->input_List = input_List;
        this->output_List = output_List;
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
    adclNode *adcl;
    paramDeclNode(primNode *prim, idNode *id, adclNode *adcl, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = ParamDcl;
        this->prim = prim;
        this->id = id;
        this->adcl = adcl;
    }
    ~paramDeclNode() {}
    void print() {}
    string toString() {}
};
class paramNode : public Node
{
  public:
    list<Node *> *param_List;
    paramNode(list<Node *> *param_List)
    {
        this->type = Param;
        this->param_List = param_List;
    }
    ~paramNode() {}
    void print() {}
    string toString() {}
};

class funcBodyNode : public Node
{
  public:
    list<Node *> *stmt_List;
    funcBodyNode(list<Node *> *stmt_List)
    {
        this->stmt_List = stmt_List;
    }
    ~funcBodyNode() {}
    void print() {}
    string toString() {}
};

class compBodyNode : public Node
{
  public:
    paramNode *param;
    list<Node *> *stmt_List;
    compBodyNode(paramNode *param, list<Node *> *stmt_List)
    {
        this->type = CompBody;
        this->param = param;
        this->stmt_List = stmt_List;
    }
    ~compBodyNode() {}
    void print() {}
    string toString() {}
};

class funcDclNode : public Node
{
  public:
    primNode *prim;
    idNode *id;
    list<Node *> *param_List;
    funcBodyNode *funcBody;
    funcDclNode(primNode *prim, idNode *id, list<Node *> *param_List, funcBodyNode *funcBody)
    {
        this->type = FuncDcl;
        this->prim = prim;
        this->id = id;
        this->param_List = param_List;
        this->funcBody = funcBody;
    }
    ~funcDclNode() {}
    void print() {}
    string toString() {}
};

class compositeNode;
class compsiteCallNode : public Node
{
  public:
    string compName;
    list<Node *> *stream_List;
    list<Node *> *param_List;
    compositeNode *actual_composite;    //保存composite展开节点
    compsiteCallNode(string compName, list<Node *> *stream_List, list<Node *> *param_List, YYLTYPE loc)
    {
        this->setLoc(loc);
        this->type = CompositeCall;
        this->compName = compName;
        this->param_List = param_List;
    }
    ~compsiteCallNode() {}
    void print() {}
    string toString() {}
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
        this->compName=id->name;
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
        this->compName=head->compName;
    }
    ~compositeNode() {}
    void print() {}
    string toString() {}
};

class operatorNode : public Node
{
  public:
    string operName;
    list<Node *> *arg_List;
    operBodyNode *operBody;
    operatorNode(string operName, list<Node *> *arg_List, operBodyNode *operBody)
    {
        this->type = Operator_;
        this->operName = operName;
        this->arg_List = arg_List;
        this->operBody = operBody;
    }
    ~operatorNode() {}
    void print() {}
    string toString() {}
};
#endif
