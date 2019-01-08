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

string declareNode::toString()
{
    string str = "";
    str += prim->toString() + " ";
    str += id_list.front()->toString();
    if (id_list.front()->init != NULL)
        str += "=" + id_list.front()->init->toString();
    for (auto iter = ++id_list.begin(); iter != id_list.end(); ++iter)
    {
        str += ",";
        str += (*iter)->toString();
        if ((*iter)->init != NULL)
            str += "=" + (*iter)->init->toString();
    }
    str += ";";
    return str;
}

string constantNode::toString()
{
    if (style == "double")
        return to_string(dval);
    else if (style == "integer")
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
    if (op == "=" || op == "+=" || op == "-=" || op == "*=" || op == "/=")
        return left->toString() + op + right->toString() + ";";
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
    auto top = id_list.front();
    string str = top->valType + ' ' + top->toString();
    for (auto iter = ++id_list.begin(); iter != id_list.end(); ++iter)
    {
        str += (*iter)->valType + ' ' + (*iter)->toString() + ',';
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

string caseNode::toString()
{
    string str = "case " + exp->toString() + ":\n";
    str += "\t" + stmt->toString();
    return str;
}
string defaultNode::toString()
{
    string str = "default:\n\t";
    str += stmt->toString();
    return str;
}

string compositeCallNode::toString()
{
    return "compositeCallNode";
}

string ifNode::toString()
{
    string str = "if(";
    str += exp->toString();
    str += ")";
    str += "\t" + stmt->toString();
    return str;
}
string ifElseNode::toString()
{
    string str = "if(";
    str += exp->toString();
    str += ")";
    str += "\t" + stmt1->toString();
    str += "else";
    str += "\t" + stmt2->toString();
    return str;
}
string forNode::toString()
{
    string str = "for(";
    str += init->toString();
    str += cond->toString();
    str += ";";
    str += next->toString();
    str += ")";
    str += "\t\t" + stmt->toString();
    return str;
}

string blockNode::toString()
{
    string str = "\t{\n";
    if (stmt_list != NULL)
    {
        for (auto stmt : *stmt_list)
        {
            str += "\t\t" + stmt->toString();
            str += "\n";
        }
    }
    str += "\t}";
    return str;
}