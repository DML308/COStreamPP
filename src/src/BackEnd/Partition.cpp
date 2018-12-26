#include "Partition.h"
Partition::Partition(){
	mnparts=1;//初始化划分数目为1
}

void Partition::setCpuCoreNum(int nplaces,SchedulerSSG *sssg)
{
	if (nplaces != 0){
		mnparts = nplaces;
	}
}
int Partition::getParts(){
	return this->mnparts;
}

int Partition::findID(SchedulerSSG *sssg,FlatNode *flatnode)
{
	for (int i=0;i<sssg->GetFlatNodes().size();i++)
		if(sssg->GetFlatNodes()[i]->name==flatnode->name)
			return i;
}