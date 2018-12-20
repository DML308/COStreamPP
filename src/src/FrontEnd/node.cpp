#include "node.h"
string listToString(list<Node *> list)
{
    int k = 0;
    string str = "";
    for (auto i : list)
        str += (k++ > 0 ? "," : "") + i->toString();
    return str;
}

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
    if (style == "double")
        return to_string(dval);
    else if (style == "interger")
        return to_string(llval);
    else
        return sval;
    ;
}

string initNode::toString()
{
    string str = "{";
    return str + listToString(value) + "}";
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

string callNode::toString()
{
    string str = name + '(';
    return str + listToString(arg_list) + ')';
}

string operatorNode::toString()
{
    return "operatorNode";
}
string idNode::toString()
{
    string str = name;
    if (isArray)
    {
        if (arg_list.size() == 0) // int sum(int a[])特殊情况,是数组但是无 arg_list
            str += "[]";
        else
        {
            for (auto i : arg_list)
            {
                str += '[' + i->toString() + ']';
            }
        }
    }
    return str;
}

string strdclNode::toString()
{
    string str = "";
    for (auto i : id_list)
    {
        str += i->valType + ' ' + i->toString() + ',';
    }
    return str;
}

string funcDclNode::toString()
{
    string str = "type.specifier(" + prim->name + ")";
    str += " IDENTIFIER(" + name + ")";
    str += " parameter.list(";
    for (auto i : param_list)
    {
        assert(i);
        str += ((idNode *)i)->valType + " " + ((idNode *)i)->toString() + ",";
    }
    str += ") function.body";
    return str;
}

string compositeCallNode::toString()
{
    return "compositeCallNode";
}
