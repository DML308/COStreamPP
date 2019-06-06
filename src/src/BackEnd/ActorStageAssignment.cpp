#include "ActorStageAssignment.h"
void StageAssignment::actorTopologicalorder(vector<FlatNode *> original)
{
	vector<FlatNode *>::iterator iter1, iter2, iter4;
	vector<FlatNode *>::iterator iter;
	vector<int> nInSet; //用于保存各节点的nIn
	vector<int>::iterator iter3;
	int nsize = original.size();
	int flag;
	for (iter1 = original.begin(); iter1 != original.end(); ++iter1)
	{
		nInSet.push_back((*iter1)->nIn); //将各节点的nIn保存
	}
	while (nsize)
	{
		for (iter1 = original.begin(); iter1 != original.end(); ++iter1)
		{
			//找没有前驱的节点(入度为0的节点）
			if ((*iter1)->nIn == 0)
			{
				//判断节点是否在拓扑集合中
				flag = 0;
				//actortopo用于存储actor的拓扑序列
				for (iter4 = actortopo.begin(); iter4 != actortopo.end(); ++iter4)
				{
					if ((*iter4)->name == (*iter1)->name)
					{
						flag = 1; //如果结点已经存在于拓扑集合中
					}
				}

				//若该节点不在拓扑集合中
				if (flag == 0)
				{
					//删除与该节点相邻接的边
					for (iter2 = (*iter1)->outFlatNodes.begin(); iter2 != (*iter1)->outFlatNodes.end(); ++iter2)
					{
						(*iter2)->nIn--;
					}
					actortopo.push_back(*iter1); //将输入边为0的结点放入actortopo中
												 //original.erase(iter1);//删除原始结点集合中的拓扑结点
					nsize--;
				}
			}
		}
	}

	for (iter1 = original.begin(), iter3 = nInSet.begin(); iter1 != original.end() && iter3 != nInSet.end(); ++iter1)
	{
		(*iter1)->nIn = (*iter3);
		iter3++;
	}
	//actortopo=original;
	/*cout<<"拓扑序列如下"<<endl;  
	for (iter=actortopo.begin();iter!=actortopo.end();++iter)
	{
		cout<<(*iter)->name<<endl;
	}	*/
}

//根据拓扑排序结果、获得阶段赋值结果
//若节点和其输入节点在一个划分子图，则其阶段号一致，否则阶段号不一致为何？？？
void StageAssignment::actorStageMap(map<FlatNode *, int> processor2actor)
{
	int maxstage, stage;
	bool flag;
	vector<FlatNode *>::iterator iter1, iter2;
	map<FlatNode *, int>::iterator iter3, iter4, iter5;
	map<FlatNode *, int>::iterator iter;
	for (iter1 = actortopo.begin(); iter1 != actortopo.end(); ++iter1)
	{
		//表示阶段号
		maxstage = 0;
		flag = false;
		if (iter1 == actortopo.begin())
			; //新加判定YSZ
		else
		{
			for (iter2 = (*iter1)->inFlatNodes.begin(); iter2 != (*iter1)->inFlatNodes.end(); ++iter2)
			{
				//ActorStage代表阶段赋值的结果
				iter = Actor2Stage.find(*iter2);
				if ((iter->second) >= maxstage)
				{
					maxstage = iter->second;
				}
				iter3 = processor2actor.find(*iter1); //查找*iter1所对应的划分号
				iter4 = processor2actor.find(*iter2); //查找*iter2所对应的划分号

				//如果该节点与该节点的输入节点不在一个划分子图
				if (iter3->second != iter4->second)
				{
					flag = true;
				}
			}
		}

		if (flag)
		{
			stage = maxstage + 1;
		}
		else
		{
			stage = maxstage;
		}
		Actor2Stage.insert(make_pair(*iter1, stage));
		Stage2Actor.insert(make_pair(stage, *iter1));
	}
	/*cout<<"阶段赋值结果如下"<<endl; 
	for (iter5=Actor2Stage.begin();iter5!=Actor2Stage.end();++iter5)
	{
		cout<<iter5->first->name<<"     "<<iter5->second<<endl;
	}*/
}

void StageAssignment::Getprocessor2topoactors(map<FlatNode*,int> processor2actor)
{
	vector<FlatNode*>::iterator topoiter;
	for(topoiter = actortopo.begin();topoiter!=actortopo.end();++topoiter)
	{
		int processorId = processor2actor.find(*topoiter)->second;
		processor2topoactors[processorId].push_back(*topoiter);
	}
}

int StageAssignment::FindStage(FlatNode *actor)
{
	auto iter = Actor2Stage.find(actor);
	if (iter != Actor2Stage.end())
	{
		return iter->second;
	}
	else
		return -1;
}

int StageAssignment::MaxStageNum()
{
	int maxStage = 0;
	for (auto it : Stage2Actor)
	{
		if (it.first + 1 > maxStage)
			maxStage = it.first + 1;
	}
	return maxStage;
}

vector<FlatNode *> StageAssignment::FindActor(int stage)
{
	vector<FlatNode *> flatVec;
	pair<multimap<int, FlatNode *>::iterator, multimap<int, FlatNode *>::iterator> pos = Stage2Actor.equal_range(stage);
	while (pos.first != pos.second)
	{
		flatVec.push_back(pos.first->second);
		ActorSet.push_back(pos.first->second);
		++pos.first;
	}
	return flatVec;
}
