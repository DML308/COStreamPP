#ifndef _NODE_H_
#define _NODE_H_
#include "nodetype.h"

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
    Node()  { loc = new Loc; }
    virtual ~Node() { delete loc; }
    void setLoc(Loc *loc);
    virtual void print();
};

#endif
