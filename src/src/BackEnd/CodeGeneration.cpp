#include "CodeGeneration.h"
#include "x86CodeGenaration.h"
#include "GPUCodeGenerate.h"
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
//CodeGeneration 入口. 其中 path 为 NULL,未生效,实际上是通过 chdir 来控制的.
void CodeGeneration(int CpuCoreNum, SchedulerSSG *sssg, string path, StageAssignment *psa, Partition *mp)
{
    //取得命令行指定的place个数，若无指定则设置成与程序actor个数一致
    int nCpucore = CpuCoreNum > 0 ? CpuCoreNum : sssg->GetFlatNodes().size();
    X86CodeGeneration *X86Code = new X86CodeGeneration(nCpucore, sssg, path.c_str(), psa, mp);
    X86Code->CGMakefile();        //生成Makefile文件
    X86Code->CGGlobalvar();       //生成流程序引入的全局变量定义文件 GlobalVar.cpp
    X86Code->CGGlobalvarHeader(); //生成流程序引入的全局变量的声明文件 GlobalVar.h
    X86Code->CGGlobal();          //生成流程序的所有缓冲区信息Global.cpp
    X86Code->CGGlobalHeader();    //生成流程序的所有缓冲区声明Global.h
    X86Code->CGactors();          //生成以类表示的计算单元actor
    X86Code->CGAllActorHeader();  //生成所有actor节点头文件
    X86Code->CGThreads();         //生成所有线程
    X86Code->CGMain();            //生成线程启动的main文件
    X86Code->CGFunctionHeader();  //生成function头文件
    X86Code->CGFunction();        //生成function定义
    /* 拷贝程序运行所需要的库文件 */
    string command = "cp " +  origin_path+"/lib/* .";
    system(command.c_str());
}

void GPUCodeGeneration(int CpuCoreNum, SchedulerSSG *sssg, string path, StageAssignment *psa, GPULBPartition *hlbp)
{
    cout<<"-----------生成GPU(OpenCL)代码-----------"<<endl;
    GPUCodeGenerate *GPUCode = new GPUCodeGenerate(sssg,"",psa,hlbp);
    GPUCode->CGGlobalvar();//生成全局变量
    GPUCode->CGGlobalvarextern();
    GPUCode->CGThreads();
    GPUCode->CGglobalHeader();
    GPUCode->CGactors();
    GPUCode->CGAllKernel();
    GPUCode->CGglobalCpp();
    GPUCode->CGMain();//生成启动线程的main文件
    /* 拷贝程序运行所需要的库文件 */
    string command = "cp " +  origin_path+"/gpulib/* .";
    system(command.c_str());
}