#include<iostream>
#include "node.h"
#include <list>
#include "token.h"
#include "handle_options.h"
#include "symbol.h"
#include "global.h"

extern FILE *yyin;      // flex uses yyin as input file's pointer
extern int  yyparse();   // parser.cc provides yyparse()
string PhaseName = "undefined"; //阶段名
SymbolTable S;

//===----------------------------------------------------------------------===//
// Main
//===----------------------------------------------------------------------===//
int main(int argc, char *argv[])
{
    //===----------------------------------------------------------------------===//
    // 编译前端 bgein
    //===----------------------------------------------------------------------===//

    // (0) 对命令行输入预处理 ---- 对应文件 FrontEnd/ ( handle_options.h, handle_options.c )
    if (handle_options(argc, argv) == false)
        return 0;
    yyin = infp; // infp is initialized in handle_opt() , default is stdin

    // (1) 初始化环境（类型、符号表、操作符表）
    //S=new SymbolTable();

    //（2）文法建立和语法树生成，
    PhaseName = "Parsing";
    yyparse();
    std::cout<<std::endl<<"[main.cpp] yyparse() end"<<std::endl;
    return 0;
}
