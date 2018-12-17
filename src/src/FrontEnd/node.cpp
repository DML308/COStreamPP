#include "node.h"
void Node::setLoc(YYLTYPE loc)
{
    this->loc->first_line = loc.first_line;
    this->loc->first_column = loc.first_column;
    this->loc->last_line = loc.last_line;
    this->loc->last_column = loc.last_column;
}

string primNode::toString()
{
    string str = "";
    if (isConst)
        str += "const ";
    return str + name;
}

string constantNode::toString()
{
    if (name == "double")
        return to_string(dval);
    else if (name == "interger")
        return to_string(llval);
    else
        return sval;
    ;
}

string initNode::toString()
{
    string str = "";
    for (auto i : value)
        str = str + i->toString()+',';
    return str;
}
