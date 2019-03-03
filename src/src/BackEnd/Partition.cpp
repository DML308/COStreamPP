#include "Partition.h"
Partition::Partition()
{
	mnparts = 1; //初始化划分数目为1
}

void Partition::setCpuCoreNum(int nplaces, SchedulerSSG *sssg)
{
	if (nplaces != 0)
	{
		mnparts = nplaces;
	}
}
int Partition::getParts()
{
	return this->mnparts;
}

int Partition::findID(SchedulerSSG *sssg, FlatNode *flatnode)
{
	for (int i = 0; i < sssg->GetFlatNodes().size(); i++)
		if (sssg->GetFlatNodes()[i]->name == flatnode->name)
			return i;
}

int Partition::findPartitionNumForFlatNode(FlatNode *flatnode)
{
	map<FlatNode *, int>::iterator iter = FlatNode2PartitionNum.find(flatnode);
	if (iter == FlatNode2PartitionNum.end())
		return -1;
	else
		return iter->second;
}

//根据编号num查找其中的节点(个数大于等于1个)，将节点集合返回给PartitonNumSet(编号->节点)
vector<FlatNode *> Partition::findNodeSetInPartition(int partitionNum)
{
	vector<FlatNode *> vecswap;
	PartitonNumSet.swap(vecswap);//清空原来的内容，并释放内存
	typedef multimap<int,FlatNode*>::iterator Num2NodeIter;
	pair<Num2NodeIter,Num2NodeIter>range=PartitonNum2FlatNode.equal_range(partitionNum);
	for(Num2NodeIter iter=range.first;iter!=range.second;++iter)
	{
		/*cout<<"$$: "<<iter->second->name<<endl;*/
		PartitonNumSet.push_back(iter->second);
	}
	return PartitonNumSet;
}