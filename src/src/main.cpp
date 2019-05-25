#include <iostream>
#include "node.h"
#include <list>
#include "token.h"
#include "0.handle_options.h"
#include "1.firstScan.h"
#include "2.semCheck.h"
#include "symbol.h"
#include "global.h"
#include "speedup.h"
#include "unfoldComposite.h"
#include "staticStreamGragh.h"
#include "6.schedulerSSG.h"
#include "ActorStageAssignment.h"
#include "GreedyPartition.h"
#include "HeuristicGreedyPartition.h"
#include "CodeGeneration.h"

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
    StageAssignment *pSA = NULL;
    int CpuCoreNum = 4; /*默认初始化为1一台机器中核的数目*/
    //===----------------------------------------------------------------------===//
    // 编译前端 begin
    //===----------------------------------------------------------------------===//

    // (0) 对命令行输入预处理
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
        //设置输出文件路径
        setOutputPath();
    }
    
    // (2) 文法建立和语法树生成
    PhaseName = "Parsing";
    yyin = infp;
    yyparse();

    // (3) 语义检查
    PhaseName = "SemCheck";
    /* 找到Main composite */
    SemCheck::findMainComposite(Program);
    // (4) 打印抽象语法树
    PhaseName = "PrintAstTree";

    // (5)语法树到平面图 SSG 是 StaticStreamGraph 对象
    PhaseName = "AST2FlatSSG";
    SSG = AST2FlatStaticStreamGraph(gMainComposite);
    // (6) 对静态数据流图各节点进行工作量估计
    PhaseName = "WorkEstimate";
    WorkEstimate(SSG);
    //打印初态和稳态工作量
    cout << "--------- 执行WorkEstimate后, 查看静态数据流图中的全部 FlatNode ---------------\n";
#if 0
    for (auto it : SSG->flatNodes)
    {
        cout << it->name << ":\t" << it->toString();
        cout << ", initWork:" << SSG->mapInitWork2FlatNode[it];
        cout << ", steadyWork:" << SSG->mapSteadyWork2FlatNode[it] << endl;
        if (it != SSG->flatNodes.back())
            cout << "    ↓" << endl;
    }
#endif

    //===----------------------------------------------------------------------===//
    // 编译前端 end
    //===----------------------------------------------------------------------===//
    //===----------------------------------------------------------------------===//
    // 编译后端 begin
    //===----------------------------------------------------------------------===//
    // (1) 对静态数据流图进行初态和稳态调度
    PhaseName = "schedulerSSG";
    cout << "--------- 对静态数据流图进行初态和稳态调度 ---------------\n";
    SSSG = SchedulingSSG(SSG);

    // (2)用XML文本的形式描述SDF图
    PhaseName = "SSG2Graph";
    DumpStreamGraph(SSSG, NULL, "flatgraph.dot");

    // (3)对节点进行调度划分
    PhaseName = "Partition";
    #if 0
    mp = new GreedyPartition(SSSG);
    #else
    mp = new HeuristicGreedyPartition(SSSG);
    #endif
    /* CpuCoreNum需要从argv中读取然后赋值，暂时未做，采用初始值 */
    mp->setCpuCoreNum(CpuCoreNum, SSSG);
    mp->SssgPartition(SSSG);
    /* dot出划分后的图 */
    DumpStreamGraph(SSSG, mp, "PartitionGraph.dot");

    // (5)打印理论加速比
    PhaseName = "Speedup";
    ComputeSpeedup(SSSG, mp, infile_name, "workEstimate.txt", "GAPartition");

    // (6) 阶段赋值
    PhaseName = "StageAssignment";
    //存储阶段赋值的结果
    pSA = new StageAssignment();
    //第一步首先根据SDF图的输入边得到拓扑序列，并打印输出
    pSA->actorTopologicalorder(SSSG->GetFlatNodes());
    //第二步根据以上步骤的节点划分结果，得到阶段赋值结果
    pSA->actorStageMap(mp->FlatNode2PartitionNum);

    // (7) 输入为SDF图，输出为目标代码
    CodeGeneration(CpuCoreNum, SSSG, "", pSA, mp);

    //===----------------------------------------------------------------------===//
    // 编译后端 end
    //===----------------------------------------------------------------------===//
    // (last) 全局垃圾回收
    PhaseName = "Recycling";
    removeTempFile(); //语法树使用完毕后删除临时文件.该 temp 文件用于输出报错行的具体内容.
    return 0;
}
