#include <iostream>
#include "node.h"
#include <list>
#include "token.h"
#include "0.handle_options.h"
#include "1.firstScan.h"
#include "2.semCheck.h"
#include "symbol.h"
#include "global.h"
#include "unfoldComposite.h"
#include "staticStreamGragh.h"
#include "schedulerSSG.h"
#include "GreedyPartition.h"

extern FILE *yyin;                               // flex uses yyin as input file's pointer
extern int yyparse();                            // parser.cc provides yyparse()
string PhaseName = "undefined";                  //阶段名
UnfoldComposite *unfold = new UnfoldComposite(); //用于展开splitjoin，pipeline节点
list<Node *> *Program = NULL;                    //用于存储语法树节点
compositeNode *gMainComposite = NULL;            //compositeMain
StaticStreamGraph *SSG = NULL;
SchedulerSSG *SSSG = NULL;
SymbolTable S;

//===----------------------------------------------------------------------===//
// Main
//===----------------------------------------------------------------------===//
int main(int argc, char *argv[])
{
    Partition *mp = NULL;
    int CpuCoreNum = 4;/*默认初始化为1一台机器中核的数目*/
    //===----------------------------------------------------------------------===//
    // 编译前端 begin
    //===----------------------------------------------------------------------===//

    // (0) 对命令行输入预处理,同时
    if (handle_options(argc, argv) == false)
        return 0;

    // (1) 做第一遍扫描(当输入文件存在时)(函数和 composite 变量名存入符号表 S)
    if (infile_name.size() == 0)
    {
        infp = stdin;
        infile_name = "stdin";
    }
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
    SSG = AST2FlatStaticStreamGraph(gMainComposite);

    // (6) 对静态数据流图各节点进行工作量估计
    PhaseName = "WorkEstimate";
    WorkEstimate(SSG);
    /* 
    打印初态和稳态工作量
    for(auto it:SSG->mapInitWork2FlatNode){
        cout<<it.second<<endl;
    }
    cout<<"-------------------"<<endl;
    for(auto it:SSG->mapSteadyWork2FlatNode){
        cout<<it.second<<endl;
    }
    */
    //===----------------------------------------------------------------------===//
    // 编译前端 end
    //===----------------------------------------------------------------------===//
    //===----------------------------------------------------------------------===//
    // 编译后端 begin
    //===----------------------------------------------------------------------===//
    //  (1) 对静态数据流图进行初态和稳态调度
    PhaseName = "schedulerSSG";
    SSSG = SchedulingSSG(SSG);

    // （2）用XML文本的形式描述SDF图
    PhaseName = "SSG2Graph";
    DumpStreamGraph(SSSG, "flatgraph.dot");

    // （3）对节点进行调度划分
    PhaseName = "Partition";
    mp = new GreedyPartition(SSSG);
    /* CpuCoreNum需要从argv中读取然后赋值，暂时未做，采用初始值 */
    mp->setCpuCoreNum(CpuCoreNum, SSSG);
    mp->SssgPartition(SSSG, 1);
    
    //===----------------------------------------------------------------------===//
    // 编译后端 end
    //===----------------------------------------------------------------------===//

    // (last) 全局垃圾回收
    PhaseName = "Recycling";
    //removeTempFile(); //语法树使用完毕后删除临时文件.该 temp 文件用于输出报错行的具体内容.
    return 0;
}
