#ifndef _NODE_H_
#define _NODE_H_
#include "global.h"
#include "nodetype.h"
#include "defines.h"
#include <list>
#include <vector>

typedef struct
{
    int first_line;
    int first_column;
    int last_line;
    int last_column;
} Loc;

class Node
{
  public:
    NodeType type;
    Loc *loc;
    short pass;
    /* parenthesized is set on expressions which were parenthesized
	   in the original source:  e.g., (x+y)*(w+z) would have
	   parenthesized==TRUE on both PLUS nodes, and parenthesized==FALSE
	   on both MULT nodes. */
    bool parenthesized;
    Node() {}
    virtual ~Node() {}
    void setLoc(Loc *loc)
    {
        loc = loc;
    }
    virtual void print() = 0;
    virtual const char *toString() = 0;
};

class primaryNode : public Node
{
  public:
    string name;
    bool isConst;
    primaryNode(string str, Loc *loc) : name(str), isConst(false)
    {
        this->type = primary;
        setLoc(loc);
    }
    ~primaryNode() {}
    void print() { cout << "primNodeType :" << name << endl; }
    const char *toString();
};

class constantNode : public Node
{
  public:
    /* 判断是double/long或者string类型 */
    string name;
    string sval;
    double dval;
    long long llval;
    constantNode(string type, string str, Loc *loc) : name(type), sval(str)
    {
        setLoc(loc);
        this->type = constant;
    }
    constantNode(string type, long long l, Loc *loc) : name(type), llval(l)
    {
        setLoc(loc);
        this->type = constant;
    }
    constantNode(string type, double d, Loc *loc) : name(type), dval(d)
    {
        setLoc(loc);
        this->type = constant;
    }
    ~constantNode() {}
    void print() { cout << "constant :" << type << endl; }
    const char *toString();
};

class identifierNode : public Node
{
  public:
    string name;
    int level;
    int version;
    identifierNode(string name, Loc *loc) : name(name)
    {
        this->type = Id;
        setLoc(loc);
        this->level = Level;
        this->version = current_version[Level];
    }
    ~identifierNode() {}
    void print() {}
    const char *toString()
    {
        string str = "name:" + name;
        str += "  level:" + to_string(level);
        str += "  version:" + to_string(version);
        return str.c_str();
    }
};

class initializerNode : public Node
{
  public:
    list<Node *> value;
    initializerNode(Loc *loc)
    {
        this->type = Initializer;
        setLoc(loc);
    }
    ~initializerNode() {}
    void print() {}
    const char *toString() {}
};

class functionNode : public Node
{
  public:
    functionNode() {}
    ~functionNode() {}
};

class compositeNode : public Node
{
  public:
    compositeNode() {}
    ~compositeNode() {}
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
    adclNode(NodeType valType, expNode *eNode, Loc *loc)
    {
        this->setLoc(loc);
        this->type = Adcl;
        this->dim = eNode;
        this->valType = valType;
    }
    ~adclNode() {}
    void print() {}
    const char *toString() {}
};

class expNode : public Node
{
  public:
    expNode() {}
    ~expNode() {}
    void print() {}
    const char *toString() {}
};

class declareNode : public Node
{
  public:
    primaryNode *primNode;
    identifierNode *idenNode;
    initializerNode *initNode;
    declareNode(primaryNode *pnode, identifierNode *idnode, initializerNode *innode)
    {
        this->primNode = pnode;
        this->idenNode = idnode;
        this->initNode = innode;
    }
    ~declareNode() {}
    void print() {}
    const char *toString() {}
};
class unaryNode : public Node
{
  public:
    expNode *exp;
    string op;
    unaryNode(string op, expNode *exp, Loc *loc)
    {
        setLoc(loc);
        this->exp = exp;
        this->op = op;
    }
    ~unaryNode() {}
    void print() {}
    const char *toString() {}
};
class binopNode : public Node
{
  public:
    expNode *left;
    expNode *right;
    string op;
    binopNode(expNode *left, string op, expNode *right, Loc *loc)
    {
        this->type = Binop;
        setLoc(loc);
        this->left = left;
        this->right = right;
        op = op;
    }
    ~binopNode() {}
    void print() {}
    const char *toString() {}
};

class ternaryNode : public Node
{
  public:
    expNode *first;
    expNode *second;
    expNode *third;
    ternaryNode(expNode *first, expNode *second, expNode *thrid, Loc *loc)
    {
        setLoc(loc);
        this->type = Ternary;
        this->first = first;
        this->second = second;
        this->third = third;
    }
    ~ternaryNode() {}
    void print() {}
    const char *toString() {}
};

class castNode : public Node
{
  public:
    primaryNode *prim;
    expNode *exp;
    castNode(primaryNode *prim, expNode *exp, Loc *loc)
    {
        setLoc(loc);
        this->type = Cast;
        this->prim = prim;
        this->exp = exp;
    }
    ~castNode() {}
    void print() {}
    const char *toString() {}
};

/* switch() case: */
class statNode;
class caseNode : public Node
{
  public:
    expNode *exp;
    statNode *stmt;
    caseNode(expNode *exp, statNode *stmt, Loc *loc)
    {
        setLoc(loc);
        this->type = Case;
        this->exp = exp;
        this->stmt = stmt;
    }
    ~caseNode() {}
    void print() {}
    const char *toString() {}
};

class defaultNode : public Node
{
  public:
    statNode *stmt;
    defaultNode(statNode *stmt, Loc *loc)
    {
        setLoc(loc);
        this->type = Default;
        this->stmt = stmt;
    }
    ~defaultNode() {}
    void print() {}
    const char *toString() {}
};

class continueNode : public Node
{
  public:
    continueNode(Loc *loc)
    {
        this->setLoc(loc);
        this->type = Continue;
    }
    ~continueNode() {}
    void print() {}
    const char *toString() {}
};

class breakNode : public Node
{
  public:
    breakNode(Loc *loc)
    {
        this->setLoc(loc);
        this->type = Break;
    }
    ~breakNode() {}
    void print() {}
    const char *toString() {}
};

class returnNode : public Node
{
  public:
    expNode *exp;
    returnNode(expNode *exp, Loc *loc)
    {
        this->setLoc(loc);
        this->type = Return;
        this->exp = exp;
    }
    ~returnNode() {}
    void print() {}
    const char *toString() {}
};

class ifNode:public Node{
    public:
    expNode *exp;
    ifNode(){}
    ~ifNode(){}
    void print() {}
    const char *toString() {}
};
#endif