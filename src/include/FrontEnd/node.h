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
    short parenthesized;
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
        type = primary;
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
        type = constant;
    }
    constantNode(string type, long long l, Loc *loc) : name(type), llval(l)
    {
        setLoc(loc);
        type = constant;
    }
    constantNode(string type, double d, Loc *loc) : name(type), dval(d)
    {
        setLoc(loc);
        type = constant;
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
        type = Id;
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
        type = Initializer;
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
    int size;
    expNode *dim;
    NodeType valType;
    /* 默认1维 */
    adclNode( NodeType valType,expNode * eNode, Loc *loc)
    {
        type = Adcl;
        this->setLoc(loc);
        this->dim=eNode;
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

#endif