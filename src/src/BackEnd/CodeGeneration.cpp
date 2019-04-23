#include "CodeGeneration.h"
#include "x86CodeGenaration.h"
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
void CodeGeneration(int CpuCoreNum, SchedulerSSG *sssg, string fileName, StageAssignment *psa, Partition *mp)
{
    /* 获取当前文件目录 用于拼接文件目录，暂时未实现*/
    char buf[1024];
    char *path = getcwd(buf, 1024);
    chdir("../");
    strcat(buf, "\/StaticDistCode_Linux");
    mkdir("StaticDistCode", 777);
    //更改文件目录
    chdir("StaticDistCode");
    mkdir(fileName.c_str(), 777);
    chdir(fileName.c_str());
    //取得命令行指定的place个数，若无指定则设置成与程序actor个数一致
    int nCpucore = CpuCoreNum > 0 ? CpuCoreNum : sssg->GetFlatNodes().size();
    X86CodeGeneration *X86Code = new X86CodeGeneration(nCpucore, sssg, path, psa, mp);
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
    const char *command = "cp ./src/lib/* .";
    const char *command2 = "pwd";
    system(command2);
    system(command);
}