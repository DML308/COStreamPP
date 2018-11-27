#ifndef _NODE_H_
#define _NODE_H_

typedef enum
{
    /* expression nodes */
    Const,
    Id
} NodeType;

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
    Node()  { loc = new Loc; }
    virtual ~Node() { delete loc; }
    void setLoc(Loc *loc);
    virtual void print();
};

#endif
