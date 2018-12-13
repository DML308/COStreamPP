#include <iostream>
#include "node.h"
#include <list>
#include "token.h"
#include "0.handle_options.h"
#include "1.firstScan.h"
#include "2.semCheck.h"
#include "symbol.h"
#include "global.h"

extern FILE *yyin;                    // flex uses yyin as input file's pointer
extern int yyparse();                 // parser.cc provides yyparse()
string PhaseName = "undefined";       //阶段名
list<Node *> *Program = NULL;         //用于存储语法树节点
compositeNode *gMainComposite = NULL; //compositeMain
SymbolTable S;

//===----------------------------------------------------------------------===//
// Main
//===----------------------------------------------------------------------===//
int main(int argc, char *argv[])
{
    //===----------------------------------------------------------------------===//
    // 编译前端 begin
    //===----------------------------------------------------------------------===//

    // (0) 对命令行输入预处理,同时
    if (handle_options(argc, argv) == false)
        return 0;

    // (1) 做第一遍扫描(当输入文件存在时)(函数和 composite 变量名存入符号表 S)
    if (infile_name == NULL)
        infp = stdin;
    else
    {
        infp = changeTabToSpace();
        infp = recordFunctionAndCompositeName();
    }

    // (2) 文法建立和语法树生成
    PhaseName = "Parsing";
    yyin = infp;
    yyparse();

    // (3) 语义检查
    PhaseName = "SemCheck";
    SemCheck::findMainComposite(Program);

    // (4) 打印抽象语法树
    PhaseName = "PrintAstTree";

    //（5）语法树到平面图 SSG 是 StaticStreamGraph 对象
    PhaseName = "AST2FlatSSG";
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
