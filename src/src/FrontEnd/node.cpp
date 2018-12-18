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
        str = str + i->toString() + ',';
    return str;
}

string binopNode::toString()
{
    return left->toString() + op + right->toString();
}

string ternaryNode::toString()
{
    assert(first && second && third);
    return first->toString() + "?" + second->toString() + ":" + third->toString();
}

string unaryNode::toString()
{
    if (op == "PREINC")
        return "++" + exp->toString();
    else if (op == "PREDEC")
        return "--" + exp->toString();
    else if (op == "POSTINC")
        return exp->toString() + "++";
    else if (op == "POSTDEC")
        return exp->toString() + "++";
    else
        return op + exp->toString();
}

string parenNode::toString()
{
    return "(" + exp->toString() + ")";
}

string castNode::toString()
{
    return "(" + prim->toString() + ")" + exp->toString();
}
