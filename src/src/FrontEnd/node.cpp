#include"node.h"
void Node::setLoc(YYLTYPE loc)
{
    this->loc->first_line = loc.first_line;
    this->loc->first_column = loc.first_column;
    this->loc->last_line = loc.last_line;
    this->loc->last_column = loc.last_column;
}

const char * primNode::toString(){
        warning("prinNode::toString");
        string str = "" ;
        if(isConst) str += "const ";
        return (str+name).c_str();
}


const char * constantNode::toString(){
        if(name == "double")
            return (to_string(dval)).c_str();
        else if(name=="interger")
            return (to_string(llval)).c_str();
        else
            return (sval).c_str();
       ;
}
