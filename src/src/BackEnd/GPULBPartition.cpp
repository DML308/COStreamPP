#include "GPULBPartition.h"
#include "string.h"

//构造函数和析构函数
GPULBPartition::GPULBPartition(int CPUt, int GPUn, int K) :CPUthread(CPUt), GPUNum(GPUn), K(K)
{

	SPn = 448;
	Pthreshold = SPn*0.6;
	GPUworkload = 0;
	CPUworkload = 0;
	averageParallofGPUActors = 0;
	GPUworkTime = 0;
	T = GPUCPU_SPEEDUP;
	if (CPUthread == 0)
		CPUthread = 1;

#ifdef OLDPARTITION
	Pthreshold = 1;
	CPUthread = 1;
#endif
}
GPULBPartition::~GPULBPartition(){

}

void GPULBPartition::SssgPartition(SchedulerSSG *sssg)
{
	//步骤1，区分Stateless和statefull节点
	DetectiveActorsState(sssg);
	//步骤2，GPU节点的初步选取，获得三个集合，分别为CPU集合，GPU集合，和候选节点。 预处理
	SortActorsWithParall(sssg);
	//步骤3，将剩余ACTOR先暂时划分给CPU，处理孤零节点,初步划分结束
	PartitionOtherNodes(sssg);
	//步骤4. 分别统计CPU和GPU子图两部分的工作量。同时计算GPU的个ACTOR的加权并行度，确定需要使用多少GPU资源.
	EstimateWorkloadOfEachPart(sssg);
	//步骤5, 预估流水线中GPU stage的执行时间，这里采用我提出的GPUACTOR预估模型。根据CPU子图的预估执行时间，确认需要划分的份数n，然后进行划分，确定平衡因子
	CalculateCPUthreads(sssg);
	//步骤6，根据对程序的分析预测结果，采用亲和性算法，将CPU子图划分成CPUthread.
	PartitionCPUNodes(sssg);
	//步骤7，记录划分结果
	AddPartitionNum(sssg);
	
	SetMultiNum2FlatNode(sssg);
}

void GPULBPartition::SetMultiNum2FlatNode(SchedulerSSG *SSSG)
{
	vector<FlatNode *>::iterator iter_1;
	for (iter_1=SSSG->flatNodes.begin();iter_1!=SSSG->flatNodes.end();++iter_1)//遍历所有结点
	{
		if((*iter_1)->GPUPart == GPUNum)
			MultiNum2FlatNode.insert(make_pair((*iter_1),K));
		else
			MultiNum2FlatNode.insert(make_pair((*iter_1),K/GPUNum));
	}
}
//查询是否含有NDrange
bool GPULBPartition::findNDrange(FlatNode* flatNode)
{
    /*
	Node *work = flatNode->contents->body->u.operBody.work;//取得operator的body的work，只需要检查这里
	//ChildNode *work = flatNode->contents->body;//取得operator的body，只需要检查这里
	currentFlatNode = flatNode;
	rWorkCompute(work);
	if (!flatNode->NDrangeName.empty())
	{
		hasSpecalGPUnode = true;
		return true;
	}*/
	return false;
}
//************************************
// Method:    DetectiveActorsState
// FullName:  DetectiveActorsState
// Qualifier: 检测flatNode中的operator的状态,TRUE表示是stateful类型，函数调用也无法在GPU上运行，一同看为CPU节点
// Parameter: SchedulerSSG *SSSG
//************************************
void GPULBPartition::DetectiveActorsState(SchedulerSSG *SSSG)
{
	for (int i = 0; i < SSSG->flatNodes.size(); i++)
	{
		FlatNode *tmpFn = SSSG->flatNodes[i];
		if (tmpFn->hasCall)
		{
			tmpFn->stateful = true;
			CPUNodes.push_back(tmpFn);
			//1表示 CPU， 0表示GPU
			tmpFn->GPUPart = GPUNum;

		}
		else if (DetectiveActorState(tmpFn) || i == 0)	//第一个节点放在以及有状态节点
		{
			tmpFn->stateful = true;
			CPUNodes.push_back(tmpFn);
			//1表示 CPU， 0表示GPU
			tmpFn->GPUPart = GPUNum;
		}
		else
		{
			tmpFn->stateful = false;
			statelessNode.push_back(tmpFn);
			//1表示 CPU， 0表示GPU
			tmpFn->GPUPart = GPUNum;
		}
	}
}

//************************************
// Method:    SortActorsWithParall
// FullName:  SortActorsWithParall
// Qualifier: GPU节点的初步选取，获得三个集合，分别为CPU集合，GPU集合，和候选节点。
// Parameter: SchedulerSSG *SSSG
//************************************
bool GPULBPartition::SortActorsWithParall(SchedulerSSG *SSSG)
{
	//获取并行度
	for (int i = 0; i < statelessNode.size(); i++)
	{
		FlatNode *tmpFn = statelessNode[i];
		int P = 1;
		if (findNDrange(statelessNode[i]))
		{
			for (int j = 0; j < statelessNode[i]->NDrangeValue.size(); j++)
			{
				P = P* statelessNode[i]->NDrangeValue[j];
			}
		}
		else
		{
			P = SSSG->GetSteadyCount(tmpFn);
		}

		flatNodeParall.insert(make_pair(tmpFn, P*K));
	}
	//将并行度高于阈值的ACTOR优先交给GPU，
	std::vector<FlatNode *>::iterator iter;
	for (iter = statelessNode.begin(); iter != statelessNode.end();)
	{
		if (flatNodeParall[*iter] > Pthreshold)
		{
			GPUNodes.push_back(*iter);
			(*iter)->GPUPart = 0;
			iter = statelessNode.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	//排序
	//根据并行度
	sort(statelessNode);

	if (GPUNodes.size() == 0)
	{
		return false;
	}
	return true;
}
void GPULBPartition::sort(std::vector<FlatNode *> &statelessNode)
{
	int n = statelessNode.size();
	for (int i = 0; i < n-1; i++)
	{
		for (int j = 0; j <n-i-1; j++)
		{
			if (flatNodeParall[statelessNode[j]] < flatNodeParall[statelessNode[j + 1]])
			{
				FlatNode * temp = statelessNode[j];
				statelessNode[j] = statelessNode[j + 1];
				statelessNode[j + 1] = temp;
			}
		}
	}
}
// ************************************
// Method:    PartitionOtherNodes
// FullName:  PartitionOtherNodes
// Qualifier: 处理孤零节点
// Parameter: SchedulerSSG *SSSG
// ************************************
void GPULBPartition::PartitionOtherNodes(SchedulerSSG *SSSG)
{
	//先将剩余节点全部划分到CPU
	for (int i = 0; i < statelessNode.size(); i++)
	{
		CPUNodes.push_back(statelessNode[i]);
	}
	//处理CPU子图中的孤零节点，如果孤零节点的工作量很小,移动到GPU子图中
	std::vector<FlatNode *>::iterator iter;
	for (iter = statelessNode.begin(); iter != statelessNode.end();)
	{
		if (getFlatNodeState(*iter) == FlatNodeState::ALONE)
		{
			//该点工作量小于平均工作量，或者通信量大于平均通信量，移动到GPU子图中
			GPUNodes.push_back(*iter);
			(*iter)->GPUPart = 0;
			auto itCPU = find(CPUNodes.begin(), CPUNodes.end(), *iter);
			CPUNodes.erase(itCPU);
			iter = statelessNode.erase(iter);
		}
		else
		{
			iter++;
		}
	}
	/*旧代码
	for (int i = 0; i < statelessNode.size(); i++)
	{
		if (getFlatNodeState(statelessNode[i]) == FlatNodeState::ALONE)
		{
			//该点工作量小于平均工作量，或者通信量大于平均通信量，移动到GPU子图中
			GPUNodes.push_back(statelessNode[i]);
			statelessNode[i]->GPUPart = 0;
			auto iter = find(CPUNodes.begin(), CPUNodes.end(), statelessNode[i]);
			CPUNodes.erase(iter);
		}
	}*/
}

//************************************
// Method:    EstimateWorkloadOfEachPart
// FullName:  EstimateWorkloadOfEachPart
// Qualifier: 评估初始划分后的GPU子图和CPU子图的工作量，确定程序需要的GPU资源，不能超过提供的值，避免极端情况
// Parameter: SchedulerSSG *SSSG
//************************************
void GPULBPartition::EstimateWorkloadOfEachPart(SchedulerSSG *SSSG)
{
	//统计CPU部分的工作量
	for (int i = 0; i < CPUNodes.size(); i++)
	{
		FlatNode * curNode = CPUNodes[i];
		
		int workload = SSSG->mapSteadyWork2FlatNode[curNode];
		int steadycount = SSSG->GetSteadyCount(curNode)*K;
		CPUworkload += workload*steadycount;
	}
	//统计GPU部分的工作量
	for (int i = 0; i < GPUNodes.size(); i++)
	{
		FlatNode * curNode = GPUNodes[i];
		float workload = SSSG->mapSteadyWork2FlatNode[curNode] * flatNodeParall[curNode];
		GPUworkload += workload;
	}

	//统计GPU部分的加权并行度,根据加权并行度，选择可以用几个GPU,最大不超过可提供的设备
	if (GPUNodes.size() == 0)
	{
		averageParallofGPUActors = 0;
	}
	else
	{
		float tempParal = 0;
		for (int i = 0; i < GPUNodes.size(); i++)
		{
			int P = flatNodeParall[GPUNodes[i]];
			float workload = SSSG->mapSteadyWork2FlatNode[GPUNodes[i]] * P;
			tempParal += (float)P*workload /GPUworkload;
		}
		averageParallofGPUActors = tempParal;

		if (averageParallofGPUActors / SPn < GPUNum)
			GPUNum = averageParallofGPUActors / SPn + 1;
	}
}




//边界调整，负载平衡
void GPULBPartition::CalculateCPUthreads(SchedulerSSG *SSSG)
{
	//预估每块GPU的执行时间
	//由于设备的差异性，同一个ACTOR在GPU和CPU上执行时间一般会存在比较大的差别
	//这时候就需要建立一种预测模型，来评估ACTOR在CPU上执行的时间T和在GPU上执行的时间T
	//为了建立这种模型，我们需要获取对应设备的一些参数，
	//对于GPU，需要获得单块GPU的流处理数量，它代表了这块GPU的最大并行能力，每个流处理的工作频率
	//对于CPU，需要获取单块CPU的核数和核心的工作频率
	//此外，对于被评估的Actor，还需要获取它的并行度，这会影响到它在Gpu上的实际并行效果。
	//除此之外，ACTOR在CPU上执行的时间T和在GPU上执行的时间T，还会被设备间的通信开销所影响，此模型着重考虑执行部分的时间，设备间通信通过后续的手段来优化
	for (int i = 0; i < GPUNodes.size(); i++)
	{
//工作量
		int workload = SSSG->mapSteadyWork2FlatNode[GPUNodes[i]];
//在每个GPU上的实际并行度
		int P = flatNodeParall[GPUNodes[i]] / GPUNum;
		int CG = (P + SPn) / SPn;
		int ActorWorkloadGPU = (workload) * CG * T;
		GPUworkTime += ActorWorkloadGPU;  
	}
	/*
	//预测需要多少CPU核心资源
	//从1-CPUthread，找到第一个平衡点
	for (int i = 1; i <= CPUthread; i++)
	{
		int CPUworkTime = CPUworkload/i;
		if (CPUworkTime < GPUworkTime) {
			CPUthread = i; 
			break;
		}
	}

	*/
	//最大也不应该超过CPUNodes数量
	if (CPUthread > CPUNodes.size())
		CPUthread = CPUNodes.size();
	int CPUworkTime = CPUworkload / CPUthread;
	GPUworkTime /= GPUNum;
	//如果CPU部分线程已经用完，并且工作量仍然过大，分配一些到GPU
	if (CPUworkTime > GPUworkTime)
	{
		while (CPUworkTime > GPUworkTime && !statelessNode.empty())
		{
			std::vector<FlatNode *>::iterator iter;
			for (iter = statelessNode.begin(); iter != statelessNode.end();)
			{
				if (getFlatNodeState(*iter) == FlatNodeState::BORDER || (iter == statelessNode.end()-1))
				{
					//重新计算GPU负载
					int workload = SSSG->mapSteadyWork2FlatNode[*iter];
					int steadycount = SSSG->GetSteadyCount(*iter)*K;
					//在每个GPU上的实际并行度
					int P = flatNodeParall[*iter] / GPUNum;
					int CG = (P + SPn) / SPn;
					int ActorWorkloadGPU = (workload) * CG * T;
					GPUworkTime += ActorWorkloadGPU;
					//重新计算CPU负载
					CPUworkload -= workload*steadycount;
					CPUworkTime = CPUworkload / CPUthread;
					//该点工作量小于平均工作量，或者通信量大于平均通信量，移动到GPU子图中
					GPUNodes.push_back(*iter);
					(*iter)->GPUPart = 0;
					auto itCPU = find(CPUNodes.begin(), CPUNodes.end(), *iter);
					CPUNodes.erase(itCPU);
					iter = statelessNode.erase(iter);
					break;
				}
				else
				{
					iter++;
				}
			}
			
		}
	}
	else //否则减少CPU使用量，CPUworkTime < GPUworkTime
	{
		//if (!hasSpecalGPUnode)
			for (int i = CPUthread; i >= 1 ; i--)
			{
			
				int CPUworkTime = CPUworkload / i;
				if (CPUworkTime > GPUworkTime)
					break;
				CPUthread = i;
			}
	}
	if (CPUthread > CPUNodes.size())
		CPUthread = CPUNodes.size();

}
//划分cpu子图 ，分配2*nGpu_+1编号
//亲和性算法
void GPULBPartition::PartitionCPUNodes(SchedulerSSG *sssg)
{
	vwgt.resize(CPUNodes.size(), 0);
	int newwork = 0;
	for (int i = 0; i < CPUNodes.size(); i++)
	{
		FlatNode * curNode = CPUNodes[i];
		int workload = sssg->mapSteadyWork2FlatNode[curNode] * sssg->GetSteadyCount(curNode)*K;
		vwgt[i] = workload;
		newwork += workload;
	}

	int k = CPUthread;					//划分份数
	double we = newwork / k;		//每个子图的平均工作量
	vector<FlatNode *> V = CPUNodes;	//CPUnode是所有未划分的节点
	vector<FlatNode *> S;
	X.resize(k, vector<FlatNode *>());//k个子图
	w.resize(k, 0);//划分的K个子图每个子图的总工作量初始化为0
	X[0] = V;							//第一个子图初始化为全图
	w[0] = newwork;
	double e = 0.95;						//满足系数（2-ee）即可

	int choose = 0;						//所选节点在X[0]中的序号
	int index = 0;						//所选节点在原图中的序号
	for (int i = 1; i < k; i++)
	{//构造子图X[1] ~ X[k-1]
		S.clear();
		while (w[i] < we * e && X[0].size() >1)
		{//一定要加X[0].size() != 0，否则137行可能会出bug
			choose = 0;
			index = 0;
			if (S.size() == 0)//如果候选集合为空
			{
				if (X[0].size() == 0)
					choose = 0;
				else //choose = rand() % (X[0].size());//从X[0]中随机选择一个节点
				{

					//选择X[0]中顶点权重最大的节点
					int tmax = findID(sssg, X[0][0]);
					for (int j = 1; j < X[0].size(); j++)
					{
						int t = findID(sssg, X[0][j]);//找到该节点在原图中的序号
						if (vwgt[t] > vwgt[tmax])tmax = t;
					}
					choose = find(X[0].begin(), X[0].end(), V[tmax]) - X[0].begin();
				}
				index = findID(sssg, X[0][choose]);//找到该节点在原图中的序号

				X[i].push_back(X[0][choose]);//将该节点加入X[i]
				w[i] += vwgt[index];//调整子图的工作量

				X[0].erase(X[0].begin() + choose);//将该节点移出X[0]
				w[0] -= vwgt[index];//调整子图的工作量
			}
			else
			{//选择候选集合中收益函数值最大的节点
				int maxGain = chooseMaxGain(sssg, S, X[i], X[0]);//找到该节点在S中的序号
				choose = find(X[0].begin(), X[0].end(), S[maxGain]) - X[0].begin();//找到该节点在X[0]中的序号
				index = findID(sssg, X[0][choose]);//找到该节点在原图中的序号

				X[i].push_back(X[0][choose]);//将该节点加入X[i]
				w[i] += vwgt[index];//调整子图的工作量

				X[0].erase(X[0].begin() + choose);//将该节点移出X[0]
				w[0] -= vwgt[index];//调整子图的工作量

				S.erase(S.begin() + maxGain);//将该节点从候选集合中删除
			}
			updateCandidate(sssg, X[0], S, index);//更新候选节点集合
		}
	}

	errorDecrease(sssg, k,newwork);//划分的误差下降算法

}


void GPULBPartition::AddPartitionNum(SchedulerSSG *SSSG)
{
	vector<vector<FlatNode *>> Xnew;//CPU子图划分的结果
	
	//cpu子图分配GPUNum和 2*GPUNum + 1编号
	for (int i = 0; i < CPUthread; i++)
	{
		for (int j = 0; j < X[i].size(); j++)
		{
			if (i == 0)
			{
				FlatNode2PartitionNum.insert(make_pair(X[i][j], GPUNum));//0表示在GPU ，比如3个GPU  0 1 2 为GPU， GpuNum为CPU
				PartitonNum2FlatNode.insert(make_pair(GPUNum, X[i][j]));
				X[i][j]->GPUPart = GPUNum;
			}
			else
			{
				FlatNode2PartitionNum.insert(make_pair(X[i][j], 2*GPUNum + i));//0表示在GPU ，比如3个GPU  0 1 2 为GPU， GpuNum为CPU
				PartitonNum2FlatNode.insert(make_pair(2 * GPUNum + i, CPUNodes[i]));
				X[i][j]->GPUPart = GPUNum;
			}

		}
	}

	for (int i = 0; i < GPUNodes.size(); i++)
	{
		FlatNode2PartitionNum.insert(make_pair(GPUNodes[i], 0));//0表示在GPU ，比如3个GPU  0 1 2 为GPU， GpuNum之后的为CPU
		PartitonNum2FlatNode.insert(make_pair(0, GPUNodes[i]));

	}


}

GPULBPartition::FlatNodeState GPULBPartition::getFlatNodeState(FlatNode *p)
{
	int partNum = p->GPUPart;
	FlatNodeState res = INSIDE;
	int cnt1 = 0;    //相邻节点相同划分数量
	int cnt2 = 0;    //相邻节点不同划分数量
	int upNum = 0;   //上端节点的划分编号
	int downNum = 0; //下端节点的划分编号
	FlatNode *q;
	for (int i = 0; i < p->nIn; i++)
	{
		q = (p->inFlatNodes)[i];//得到输入边对应的上端节点
		upNum = q->GPUPart;//得到上端节点对应的划分号
		if (upNum == partNum){
			cnt1++;
		}
		else cnt2++;
	}
	for (int i = 0; i < p->nOut; i++){//遍历所有输出边
		q = (p->outFlatNodes)[i];//得到输出边对应的下端端节点
		downNum = q->GPUPart;
		if (downNum == partNum){
			cnt1++;
		}
		else cnt2++;
	}
	if (cnt1 == 0)res = ALONE;
	else if (cnt2 == 0)res = INSIDE;
	else res = BORDER;
	return res;
}

std::vector<FlatNode *> GPULBPartition::GetGPUNodeSet()
{
	return GPUNodes;
}
std::vector<FlatNode *> GPULBPartition::GetCPUNodeSet()
{
	return CPUNodes;
}



void GPULBPartition::updateCandidate(SchedulerSSG *sssg, vector<FlatNode *>& X0, vector<FlatNode *>& S, int index)
{
	vector<FlatNode *> V = sssg->GetFlatNodes();//sssg所有顶点的vector
	for (int j = 0; j < V[index]->nIn; j++){//遍历所有输入边
		FlatNode *p = (V[index]->inFlatNodes)[j];//得到输入边对应的上端节点
		if (find(X0.begin(), X0.end(), p) != X0.end() && find(S.begin(), S.end(), p) == S.end()){//如果上端节点在X0中并且不在S中
			S.push_back(p);//将它加入S中
		}
	}
	for (int j = 0; j < V[index]->nOut; j++){//遍历所有输出边
		FlatNode *p = (V[index]->outFlatNodes)[j];//得到输出边对应的下端节点
		if (find(X0.begin(), X0.end(), p) != X0.end() && find(S.begin(), S.end(), p) == S.end()){//如果上端节点在X0中并且不在S中
			S.push_back(p);//将它加入S中
		}
	}
}

int GPULBPartition::chooseMaxGain(SchedulerSSG *sssg, vector<FlatNode *>& S, vector<FlatNode *>& Xi, vector<FlatNode *>& X0){
	int result = 0;
	int maxGain = INT_MIN;
	for (int i = 0; i < S.size(); i++){//遍历所有候选节点
		int wi = 0;
		int w0 = 0;
		for (int j = 0; j < S[i]->nIn; j++){//遍历所有输入边
			FlatNode *p = (S[i]->inFlatNodes)[j];//得到输入边对应的上端节点
			if (find(Xi.begin(), Xi.end(), p) != Xi.end()){//如果上端节点在Xi中
				wi += (S[i]->inPopWeights[j]) * (sssg->GetSteadyCount(S[i]));
			}
			else if (find(X0.begin(), X0.end(), p) != X0.end()){//如果上端节点在X0中
				w0 += (S[i]->inPopWeights[j]) * (sssg->GetSteadyCount(S[i]));
			}
		}
		for (int j = 0; j < S[i]->nOut; j++){//遍历所有输出边
			FlatNode *p = (S[i]->outFlatNodes)[j];//得到输出边对应的下端端节点
			if (find(Xi.begin(), Xi.end(), p) != Xi.end()){//如果下端节点在Xi中
				wi += (S[i]->outPushWeights[j]) * (sssg->GetSteadyCount(S[i]));
			}
			else if (find(X0.begin(), X0.end(), p) != X0.end()){//如果下端节点在X0中
				w0 += (S[i]->outPushWeights[j]) * (sssg->GetSteadyCount(S[i]));
			}
		}
		if (wi - w0 > maxGain){
			maxGain = wi - w0;
			result = i;//更新最大收益节点
		}
	}
	return result;
}

int GPULBPartition::findID(SchedulerSSG *sssg, FlatNode *flatnode)
{
	for (int i = 0; i<CPUNodes.size(); i++)
		if (strcmp(CPUNodes[i]->name.c_str(), flatnode->name.c_str()) == 0)
			return i;
}

void GPULBPartition::errorDecrease(SchedulerSSG *sssg, int k, int newwork){//误差下降算法
	double we = newwork / k;
	int imax = 0;
	int imin = 0;
	for (int i = 0; i < w.size(); i++){
		if (w[i] > w[imax]){
			imax = i;
		}
		if (w[i] < w[imin]){
			imin = i;
		}
	}

	int cnt = X[imax].size();
	while (w[imax] > we && cnt > 0){//大于平衡因子时进行误差下降算法
		int t = w[imax];
		int choose = 0;
		for (int i = 0; i < X[imax].size(); i++){
			int index = findID(sssg, X[imax][i]);//找到该节点在原图中的序号
			if (w[imin] + vwgt[index] < w[imax]){//如果小于最大子图工作量
				int tt = max(w[imin] + vwgt[index], w[imax] - vwgt[index]); //最大子图工作量
				if (tt < t)
				{ //以工作量为首要指标
					t = tt;
					choose = i;
				}
			}

		}

		int index = findID(sssg, X[imax][choose]);
		if (w[imin] + vwgt[index] < w[imax]){

			X[imin].push_back(X[imax][choose]);//将该节点加入X[imin]
			w[imin] += vwgt[index];//调整子图的工作量

			X[imax].erase(X[imax].begin() + choose);//将该节点移出X[max]
			w[imax] -= vwgt[index];//调整子图的工作量
		}

		int tmax = 0;
		int tmin = 0;
		for (int i = 0; i < w.size(); i++){
			if (w[i] > w[tmax]){
				tmax = i;
			}
			if (w[i] < w[tmin]){
				tmin = i;
			}
		}
		if (tmax == imax){
			imin = tmin;
			cnt--; //如果原子图的工作量依然最大，则减少循环次数
		}
		else{
			imax = tmax;
			imin = tmin;
			cnt = X[imax].size();
		}
	}
}

int GPULBPartition::getTotalEdge(SchedulerSSG *sssg, int k){//得到各核间通信量的大小和总大小
	edge.clear();
	edge.resize(k, 0);
	int total_edge = 0;
	for (int i = 0; i < k; i++){//计算此时的通信量
		for (int j = 0; j < X[i].size(); j++){//遍历划分后每一部分的节点
			for (int m = 0; m < X[i][j]->nIn; m++){//遍历所有输入边
				FlatNode *p = (X[i][j]->inFlatNodes)[m];//得到输入边对应的上端节点
				if (find(X[i].begin(), X[i].end(), p) == X[i].end()){//如果上端节点不在Xi中
					edge[i] += (X[i][j]->inPopWeights[m]) * (sssg->GetSteadyCount(X[i][j]));
				}
			}
			for (int m = 0; m < X[i][j]->nOut; m++){//遍历所有输出边
				FlatNode *p = (X[i][j]->outFlatNodes)[m];//得到输出边对应的下端端节点
				if (find(X[i].begin(), X[i].end(), p) == X[i].end()){//如果下端节点不在Xi中
					edge[i] += (X[i][j]->outPushWeights[m]) * (sssg->GetSteadyCount(X[i][j]));
				}
			}
		}
		total_edge += edge[i];
	}
	return total_edge;
}

vector<FlatNode *> GPULBPartition::FindNodeInOneDevice(int parts)
{
	if (parts == GPUNum || parts >= 2*GPUNum+1)
	{
		return CPUNodes;
	}
	else
		return GPUNodes;
}

int GPULBPartition::Index2Device(int index)
{
	if (index == GPUNum || index >= 2*GPUNum+1 )
	{
		return GPUNum;
	}
	else 
		return 0;
}