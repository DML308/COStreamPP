#include<iostream>
#include "token.hh"
extern int yyparse();   // parser.cc provides yyparse()

int main(){
    yyparse();
    std::cout<<std::endl<<"[main.cpp] yyparse() end"<<std::endl;
    return 0;
}
