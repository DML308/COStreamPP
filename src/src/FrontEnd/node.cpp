#include"node.h"
const char * primNode::toString(){
        string str = "" ;
        if(isConst) str += "const ";
        return (str+name).c_str();
} 


const char * constantNode::toString(){
        if(style == "double") 
            return (to_string(dval)).c_str();
        else if(style=="interger")
            return (to_string(llval)).c_str();
        else 
            return (sval).c_str();
       ;
} 