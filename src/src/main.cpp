#include<iostream>
#include "node.h"
#include <list>
#include "token.h"
using namespace std;

extern int yyparse();   // parser.cc provides yyparse()

int main(){
    yyparse();
    std::cout<<std::endl<<"[main.cpp] yyparse() end"<<std::endl;
    return 0;
}
