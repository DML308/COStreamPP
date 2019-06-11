#include "HeuristicGreedyPartition.h"
void HeuristicGreedyPartition::SssgPartition(SchedulerSSG *sssg)
{
    //只有一个核心的话应不做划分
    if (this->mnparts == 1)
    {
       //之前这部分代码有问题, 所以先留空,直接进入下面流程
    }

    //初始化相关信息
    nvtxs = sssg->GetFlatNodes().size(); //顶点的个数
    int k = getParts();                  //划分的份数
    X.resize(k, vector<FlatNode *>());   //初始化k个子图所存储的节点数
    w.resize(k, 0);                      //划分的K个子图每个子图的总工作量初始化为0
    edge.resize(k, 0);                   //划分的K个子图每个子图的通信初始化为0
    vwgt.resize(nvtxs, 0);               //每个顶点的权重初始化为0
    part.resize(nvtxs, 0);               //每个节点对应的核号初始化为0

    setActorWorkload(sssg); //初始化每个节点的工作量(稳态工作量*稳态调度次数)

    //进入初始划分阶段
    cout << "initial partition" << endl;
    doPartition(sssg, k);
    for (int i = 0; i < k; i++)
        cout << w[i] << endl;
    cout << "comm:" << getTotalEdge(sssg, k) << endl;
    //保存初始划分结果
    vector<vector<FlatNode *>> tmp_X = X;
    vector<int> tmp_w = w;

    //误差下降算法1
    cout << "errordecrease" << endl;
    errorDecrease(sssg, k);
    float w_max = -1;
    float w_bal = 0;
    for (int i = 0; i < k; i++)
    {
        if (w[i] > w_max)
            w_max = w[i];
        w_bal += w[i];
        cout << w[i] << endl;
    }
    w_bal = w_bal / k;
    cout << "balance actor:" << w_max / w_bal << endl;
    cout << "comm:" << getTotalEdge(sssg, k) << endl;

    ee = w_max / w_bal * 1.01;

    //对初始化分使用禁忌搜索来优化划分结果
    cout << "dynamicOptimization:" << endl;
    dynamicOptimization(sssg, k);

    w_max = -1;
    w_bal = 0;
    int all = 0;
    for (int i = 0; i < k; i++)
    {
        if (w[i] > w_max)
            w_max = w[i];
        w_bal += w[i];
        all += w[i];
        //cout << w[i] << endl;
    }

    cout << "speed up:" << w_bal / w_max << endl;
    w_bal = w_bal / k;
    cout << "balance actor:" << w_max / w_bal << endl;
    cout << "comm:" << getTotalEdge(sssg, k) << endl;
    //cout <<"all workload:"<<all << endl;
    //存储最终的划分结果
    savePartitionResult(sssg);
}

void HeuristicGreedyPartition::savePartitionResult(SchedulerSSG *sssg)
{
    int s = orderPartitionResult(); //对划分结果进行重排序

    for (int i = 0; i < X.size(); i++)
    { //保存节点与之对应的核号
        for (int j = 0; j < X[i].size(); j++)
        {
            part[findID(sssg, X[i][j])] = i;
        }
    }

    if (1 /*X86Backend*/)
    {
        for (int i = 0; i < nvtxs; i++)
        {
            FlatNode2PartitionNum.insert(make_pair(sssg->GetFlatNodes()[i], part[i])); //建立节点到划分编号的映射
        }
        for (int i = 0; i < nvtxs; i++)
            PartitonNum2FlatNode.insert(make_pair(part[i], sssg->GetFlatNodes()[i]));
    }

}

float HeuristicGreedyPartition::getBalance(int k)
{
    float w_max = -1;
    float w_bal = 0;
    for (int i = 0; i < k; i++)
    {
        if (w[i] > w_max)
            w_max = w[i];
        w_bal += w[i];
    }

    w_bal /= k;

    return w_max / w_bal;
}
void HeuristicGreedyPartition::updateGainStructure(SchedulerSSG *sssg, int k)
{
    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < X[i].size(); j++)
        {
            partRecord[X[i][j]] = i;
            for (int m = 0; m < X[i][j]->nIn; m++)
            {
                FlatNode *p = (X[i][j]->inFlatNodes)[m];
                if (find(X[i].begin(), X[i].end(), p) != X[i].end())
                { //如果上端节点在同一个子图中
                    int comm = (X[i][j]->inPopWeights[m]) * (sssg->GetSteadyCount(X[i][j]));
                    insideGraph[X[i][j]][p] = comm;
                }
                else
                {
                    int comm = (X[i][j]->inPopWeights[m]) * (sssg->GetSteadyCount(X[i][j]));
                    outsideGraph[X[i][j]][p] = comm;
                }
            }

            for (int m = 0; m < X[i][j]->nOut; m++)
            {
                FlatNode *p = (X[i][j]->outFlatNodes)[m];
                if (find(X[i].begin(), X[i].end(), p) != X[i].end())
                { //如果下端节点在同一个子图中
                    int comm = (X[i][j]->outPushWeights[m]) * (sssg->GetSteadyCount(X[i][j]));
                    insideGraph[X[i][j]][p] = comm;
                }
                else
                {
                    int comm = (X[i][j]->outPushWeights[m]) * (sssg->GetSteadyCount(X[i][j]));
                    outsideGraph[X[i][j]][p] = comm;
                }
            }
        }
    }
}
void HeuristicGreedyPartition::updateGainCandidate(SchedulerSSG *sssg, int k)
{
    candidates.clear();
    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < X[i].size(); j++)
        {
            if (outsideGraph[X[i][j]].size())
            {
                //cout <<"边界节点:" <<findID(sssg,X[i][j])<< endl;
                vector<int> commGain(k, 0); //记录节点移动到各个子图的增益值
                auto insideIter = insideGraph[X[i][j]].begin();
                auto outsideIter = outsideGraph[X[i][j]].begin();
                //记录节点在当前子图的增益值
                //cout << "边界节点内部相连节点:";
                while (insideIter != insideGraph[X[i][j]].end())
                {
                    //cout << findID(sssg,insideIter->first) << " ";
                    commGain[i] += insideIter->second;
                    insideIter++;
                }
                //cout << endl;
                //记录节点移动到其它子图的增益值
                //cout << "边界节点外部相连节点:";
                while (outsideIter != outsideGraph[X[i][j]].end())
                {
                    //cout << findID(sssg, outsideIter->first) << " ";
                    int tmp = outsideIter->second;
                    int index = partRecord[outsideIter->first];
                    commGain[index] += tmp;
                    outsideIter++;
                }
                //cout << endl;

                int moveDirect = i; //记录移动的子域方向
                int commgain = 0;   //记录当前的最大收益
                for (int m = 0; m < k; m++)
                {
                    int diff = commGain[m] - commGain[i];
                    if (diff > commgain)
                    {
                        commgain = diff;
                        moveDirect = m;
                    }
                }

                if (commgain >= 0 && moveDirect != i)
                    candidates.push_back(make_pair(X[i][j], make_pair(commgain, moveDirect)));
            }
        }
    }
    sort(candidates.begin(), candidates.end(), [](pair<FlatNode *, pair<int, int>> &a, pair<FlatNode *, pair<int, int>> &b) {       return a.second.first > b.second.first; 
    });
}
void HeuristicGreedyPartition::dynamicOptimization(SchedulerSSG *sssg, int k)
{
    //获取原始的划分效果数据
    cout << "before tabuSearch:" << endl;
    cout << "balance:" << getBalance(k) << endl;
    cout << "comm:" << getTotalEdge(sssg, k) << endl;

    //记录原始划分值
    vector<vector<FlatNode *>> bestX(X);
    vector<int> bestw(w);
    float bestBalacne = ee;
    int bestComm = getTotalEdge(sssg, k) * 1.01;

    //使用禁忌搜索进行优化
    //初始化干扰次数
    int disturbTimes = 1;
    for (int i = 0; i < disturbTimes; i++)
    {
        //初始化候选节点
        updateGainStructure(sssg, k);
        //初始化候选增益列表
        updateGainCandidate(sssg, k);
        cout << "candidates List" << endl;
        for (int i = 0; i < candidates.size(); i++)
        {
            cout << findID(sssg, candidates[i].first) << ":" << candidates[i].second.first << endl;
        }
        //初始化迭代次数
        int iterTimes = 100;
        while (iterTimes)
        {
            bool flag = false;
            for (int i = 0; i < candidates.size(); i++)
            {
                FlatNode *c = candidates[i].first;
                //判断是否在禁忌列表中
                if (!tabuList[c])
                {
                    if (moveActor(sssg, candidates[i], k))
                    { //执行成功，修改了candidates所以跳出
                        tabuList[c] = 5;
                        flag = true;
                        break;
                    }
                }
            }

            if (flag)
            { //flag==true代表本次迭代进行了移动，那么进入下一次迭代,将禁忌列表中所有元素计数减1
                iterTimes--;
                auto iter = tabuList.begin();
                while (iter != tabuList.end())
                {
                    if (iter->second != 0)
                        iter->second--;
                    iter++;
                }
            }
            else
            { //说明增益列表的移动都不符合条件，已经得到局部最优解，退出循环
                break;
            }
        }
        cout << "execute times:" << 100 - iterTimes << endl;

        vector<vector<FlatNode *>> beforeX(X);
        vector<int> beforew(w);

        int before = getTotalEdge(sssg, k);
        errorDecrease(sssg, k);
        int after = getTotalEdge(sssg, k);
        if (after > before * 1.01)
        {
            X = beforeX;
            w = beforew;
        }

        if (getTotalEdge(sssg, k) <= bestComm && getBalance(k) <= bestBalacne)
        {
            bestComm = getTotalEdge(sssg, k);
            bestBalacne = getBalance(k);
            bestX = X;
            bestw = w;
        }

        //收敛后对结果进行干扰
        int max_part = 0;
        int min_part = 0;
        for (int i = 0; i < k; i++)
        {
            if (w[i] > w[max_part])
                max_part = i;

            if (w[i] < w[min_part])
                min_part = i;
        }

        disturb(sssg, k, max_part, min_part);

        //干扰结束后初始化禁忌列表
        tabuList.clear();
    }
    X = bestX;
    w = bestw;
}

bool HeuristicGreedyPartition::moveActor(SchedulerSSG *sssg, pair<FlatNode *, pair<int, int>> &candidate, int k)
{
    int cur = partRecord[candidate.first];
    int dest = candidate.second.second;

    vector<int> tmp_w(w);
    int index = findID(sssg, candidate.first);

    tmp_w[dest] += vwgt[index];
    tmp_w[cur] -= vwgt[index];

    float w_max = -1;
    float w_bal = 0;

    for (int i = 0; i < k; i++)
    {
        if (tmp_w[i] > w_max)
            w_max = tmp_w[i];
        w_bal += tmp_w[i];
    }

    w_bal /= k;

    //移动节点后发现大于平衡因子，那么不进行移动
    if (w_max / w_bal > ee)
        return false;

    cout << "calculate balance:" << w_max / w_bal << endl;
    cout << "move actor" << index << endl;
    cout << "gain:" << candidate.second.first << endl;

    //如果移动后负载仍然小于平衡因子，那么执行这次移动，并更新所有的数据
    //调整负载w和各子图划分结果
    int choose = find(X[cur].begin(), X[cur].end(), candidate.first) - X[cur].begin();

    X[dest].push_back(candidate.first);
    w[dest] += vwgt[index];

    X[cur].erase(X[cur].begin() + choose);
    w[cur] -= vwgt[index];

    //调整增益结构相关数据
    //调整调整划分区间partRecord
    partRecord[candidate.first] = dest;
    //调整增益列表candidates,删除收益节点
    int gainChoose = find(candidates.begin(), candidates.end(), candidate) - candidates.begin();
    candidates.erase(candidates.begin() + gainChoose);
    //获取当前的FlatNode*
    FlatNode *actor = candidate.first;
    //调整与移动节点相关联的内部节点，并更新对应节点的关系
    map<FlatNode *, int> inside = insideGraph[actor];
    auto insideIter = inside.begin();
    vector<FlatNode *> vclear1; //移动后变成不同划分需要删除的内部节点
    while (insideIter != inside.end())
    {
        int comm = insideIter->second;
        FlatNode *actor2 = insideIter->first;
        //修改移动节点的insideGraph和outsideGraph
        vclear1.push_back(actor2);
        outsideGraph[actor][insideIter->first] = comm;
        //修改移动节点邻接节点的insideGraph和outsideGraph
        insideGraph[actor2].erase(actor);
        outsideGraph[actor2][actor] = comm;
        //修改对应节点在candidates中的相关信息
        insideIter++;
    }
    for (int i = 0; i < vclear1.size(); i++)
        insideGraph[actor].erase(vclear1[i]);

    //调整与移动节点相关联的外部节点，并更新对应节点的关系
    map<FlatNode *, int> outside = outsideGraph[actor];
    auto outsideIter = outside.begin();
    vector<FlatNode *> vclear2; //移动后变成同一划分的节点需要删除的外部节点
    while (outsideIter != outside.end())
    {
        //如果与之临接的节点在调整后在同一个子图的话进行调整，否则不进行调整
        if (partRecord[outsideIter->first] == dest)
        {
            int comm = outsideIter->second;
            FlatNode *actor2 = outsideIter->first;
            //修改移动节点的insideGraph和outsideGraph
            vclear2.push_back(actor2);
            insideGraph[actor][outsideIter->first] = comm;
            //修改移动节点邻接节点的insideGraph和outsideGraph
            outsideGraph[actor2].erase(actor);
            insideGraph[actor2][actor] = comm;
            //修改对应节点在candidates中的相关信息
        }
        outsideIter++;
    }
    for (int i = 0; i < vclear2.size(); i++)
        outsideGraph[actor].erase(vclear2[i]);

    //重新执行上面的算法更新candidates列表
    updateGainCandidate(sssg, k);
}

void HeuristicGreedyPartition::disturb(SchedulerSSG *sssg, int k, int max_part, int min_part)
{
    //记录每个节点的工作量
    map<FlatNode *, int> FlatNodeWorkload;
    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < X[i].size(); j++)
        {
            FlatNodeWorkload[X[i][j]] = vwgt[findID(sssg, X[i][j])];
        }
    }
    int w_bal = 0;
    for (int i = 0; i < k; i++)
    {
        w_bal += w[i];
    }
    w_bal /= k;
    //记录节点的移动方向
    vector<pair<FlatNode *, int>> out;
    vector<pair<FlatNode *, int>> in;
    //大的节点尝试将边界节点移动到邻接子图直至小于w_bal
    for (int i = 0; i < X[max_part].size(); i++)
    {
        if (w[max_part] <= w_bal)
        {
            break;
        }
        FlatNode *p = X[max_part][i];

        pair<FlatNode *, int> pf;
        int work = 2147483647;//INT_MAX
        bool flag = false;

        for (int j = 0; j < p->nIn; j++)
        {
            FlatNode *actor = (p->inFlatNodes)[j];
            int partResult = getPart(actor);
            if (w[partResult] < work && partResult != max_part)
            {
                work = w[partResult];
                pf = make_pair(p, partResult);
                flag = true;
            }
        }
        for (int j = 0; j < p->nOut; j++)
        {
            FlatNode *actor = (p->outFlatNodes)[j];
            int partResult = getPart(actor);

            if (w[partResult] < work && partResult != max_part)
            {
                work = w[partResult];
                pf = make_pair(p, partResult);
                flag = true;
            }
        }

        if (flag && find(out.begin(), out.end(), pf) == out.end())
        {
            w[max_part] -= FlatNodeWorkload[pf.first];
            w[pf.second] += FlatNodeWorkload[pf.first];
            out.push_back(pf);
        }
    }
    for (int i = 0; i < out.size(); i++)
    {
        X[out[i].second].push_back(out[i].first);
        int index = find(X[max_part].begin(), X[max_part].end(), out[i].first) - X[max_part].begin();
        X[max_part].erase(X[max_part].begin() + index);
    }

    //小的节点尝试从邻接子图吸纳节点直至大于w_bal
    for (int i = 0; i < X[min_part].size(); i++)
    {
        if (w[min_part] >= w_bal)
        {
            break;
        }
        FlatNode *p = X[min_part][i];

        pair<FlatNode *, int> pf;
        int work = INT_MIN;
        bool flag = false;

        for (int j = 0; j < p->nIn; j++)
        {
            FlatNode *actor = p->inFlatNodes[j];
            int partResult = getPart(actor);

            if (w[partResult] > work && partResult != min_part)
            {
                work = w[partResult];
                pf = make_pair(actor, partResult);
                flag = true;
            }
        }

        for (int j = 0; j < p->nOut; j++)
        {
            FlatNode *actor = p->outFlatNodes[j];
            int partResult = getPart(actor);

            if (w[partResult] > work && partResult != min_part)
            {
                work = w[partResult];
                pf = make_pair(actor, partResult);
                flag = true;
            }
        }

        if (flag && find(in.begin(), in.end(), pf) == in.end())
        {
            w[min_part] += FlatNodeWorkload[pf.first];
            w[pf.second] -= FlatNodeWorkload[pf.first];
            in.push_back(pf);
        }
    }
    for (int i = 0; i < in.size(); i++)
    {
        X[min_part].push_back(in[i].first);
        int index = find(X[in[i].second].begin(), X[in[i].second].end(), in[i].first) - X[in[i].second].begin();
        X[in[i].second].erase(X[in[i].second].begin() + index);
    }
}