#include "x86CodeGenaration.h"
X86CodeGenerate::X86CodeGenerate(int cpuCoreNum, SchedulerSSG *sssg, const char *, StageAssignment *psa, Partition *mp)
{
    psa_ = psa;
    sssg_ = sssg;
    mp_ = mp;
    flatNodes_ = sssg->GetFlatNodes();
    nCpucore_ = cpuCoreNum;
    //建立mapActor2InEdge，mapActor2OutEdge
    for (auto iter1 : flatNodes_)
    {
        for (auto iter2 : iter1->inFlatNodes)
        {
            string str = iter2->name + "_" + iter1->name; //得到形如A_B的边名称
            mapActor2InEdge.insert(make_pair(iter1, str));
        }
        for (auto iter3 : iter1->outFlatNodes)
        {
            string str = iter1->name + "_" + iter3->name;
            mapActor2OutEdge.insert(make_pair(iter1, str));
        }
    }
    //构造每个线程上的stage集合mapNum2Stage
    for (int i = 0; i < nCpucore_; i++)
    {
        set<int> tempstageset;
        tempstageset.clear();
        vector<FlatNode *> tempactors = mp->findNodeSetInPartition(i); //根据线程号找到actor集合
        //遍历actor
        for (auto iter : tempactors)
        {
            tempstageset.insert(psa_->FindStage(iter));
        }
        mapNum2Stage.insert(make_pair(i, tempstageset));
    }
}