#include"node.h"
const char * primaryNode::toString(){
        string str = "" ;
        if(isConst) str += "const ";
        return (str+name).c_str();
    } 
};

const char * constantNode::toString(){
        if(name == "double") 
            return (to_string(dval)).c_str();
        else if(name=="interger")
            return (to_string(llval)).c_str();
        else 
            return (sval).c_str();
       ;
    } 