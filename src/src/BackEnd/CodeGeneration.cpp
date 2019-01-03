#include "CodeGeneration.h"
#include "x86CodeGenaration.h"
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
void CodeGeneration(int CpuCoreNum,SchedulerSSG *sssg, string filePath, StageAssignment *psa, Partition *mp){
    /* 获取当前文件目录 用于拼接文件目录，暂时未实现*/
    char buf[1024];
    char *path=getcwd(buf,1024);
    strcat(buf,"\/StaticDistCode_Linux");
    mkdir("StaticDistCode",777);
    //取得命令行指定的place个数，若无指定则设置成与程序actor个数一致
    int nCpucore = CpuCoreNum > 0 ? CpuCoreNum : sssg->GetFlatNodes().size();
    X86CodeGenerate *X86Code = new X86CodeGenerate(nCpucore, sssg,  path,psa,mp);
    // 1.设置文件生成路径
}