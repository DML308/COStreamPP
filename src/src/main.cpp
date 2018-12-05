#include<iostream>
#include "node.h"
#include <list>
#include "token.h"
#include "handle_options.h"
#include "global.h"

extern FILE *yyin;      // flex uses yyin as input file's pointer
extern int  yyparse();   // parser.cc provides yyparse()
string PhaseName = "undefined"; //阶段名

//===----------------------------------------------------------------------===//
// Main
//===----------------------------------------------------------------------===//
int main(int argc, char *argv[])
{
    //===----------------------------------------------------------------------===//
    // 编译前端 begin
    //===----------------------------------------------------------------------===//

    // (0) 对命令行输入预处理,同时做第一遍扫描 ---- 对应文件 FrontEnd/ ( handle_options.h, handle_options.c )
    if (handle_options(argc, argv) == false)
        return 0;
    yyin = infp; // infp is initialized in handle_opt() , default is stdin

    // (1) 初始化环境（类型、符号表、操作符表）
    //InitTypes();
    //init_symbol_tables(TRUE);
    //InitOperatorTable();

    // (2)文法建立和语法树生成，
    PhaseName = "Parsing";
    yyparse();

    //===----------------------------------------------------------------------===//
    // 编译前端 end
    //===----------------------------------------------------------------------===//
    //===----------------------------------------------------------------------===//
    // 编译后端 begin
    //===----------------------------------------------------------------------===//
    //===----------------------------------------------------------------------===//
    // 编译后端 end
    //===----------------------------------------------------------------------===//

    // (last) 全局垃圾回收
    PhaseName = "Recycling";
    //removeTempFile(); //语法树使用完毕后删除临时文件.该 temp 文件用于输出报错行的具体内容.
    return 0;
}
