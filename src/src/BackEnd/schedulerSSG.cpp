#include "schedulerSSG.h"

using namespace std;

SchedulerSSG *SchedulingSSG(StaticStreamGraph *ssg)
{
    SchedulerSSG *sssg = new SchedulerSSG(ssg);
    if (sssg->SteadyScheduling())
    {
        sssg->InitScheduling();
#if 0
        std::map<FlatNode *, int>::iterator pos;
        cout << "稳态调度序列:" << endl;
        for (pos = sssg->mapSteadyCount2FlatNode.begin(); pos != sssg->mapSteadyCount2FlatNode.end(); ++pos)
            cout << pos->first->name << "\t" << pos->second << endl;
        cout << "初始化调度序列:" << endl;
        for (pos = sssg->mapInitCount2FlatNode.begin(); pos != sssg->mapInitCount2FlatNode.end(); ++pos)
            cout << pos->first->name << "\t" << pos->second << endl;

#endif
    }
    else
    {
        fprintf(stdout, "程序不存在稳态调度，无法进行代码生成！\n");
        system("pause");
        exit(1);
    }
    return sssg;
}

SchedulerSSG::SchedulerSSG(StaticStreamGraph *ssg)
{
    flatNodes = ssg->flatNodes;
    topNode = ssg->topNode;
    mapEdge2UpFlatNode = ssg->mapEdge2UpFlatNode;
    mapEdge2DownFlatNode = ssg->mapEdge2DownFlatNode;
    mapSteadyWork2FlatNode = ssg->mapSteadyWork2FlatNode;
    mapInitWork2FlatNode = ssg->mapInitWork2FlatNode;
}

bool SchedulerSSG::SteadyScheduling()
{
    list<FlatNode *> flatNodeList;
    std::map<FlatNode *, int>::iterator pos;
    // 默认第一个节点是源，也就是说peek和pop均为0,在图的表示上暂不允许有多个源，但可以有多个peek = pop = 0节点
    FlatNode *up = topNode, *down = NULL;
    int nPush = 0, nPop = 0, nLcm = 0;
    int x, y, i, j;
    // 现在考虑的是只有一个输入口的情况
    while (1)
    {
        // 稳态调度系列初始系数为1
        mapSteadyCount2FlatNode.insert(make_pair(up, 1));
        // 遍历该节点的所有输出节点（广度优先遍历）
        for (i = 0; i < up->nOut; ++i)
        {
            nPush = up->outPushWeights[i]; // 上端节点的push值
            down = up->outFlatNodes[i];    // 找到下端节点
            for (j = 0; down->inFlatNodes[j] != up; j++)
                ;                         // 下端节点找到与上端节点对应的标号
            nPop = down->inPopWeights[j]; // 下端节点取出对应的pop值
            // 检查该节点是否已进行稳态调度，每条只进行一次稳态调度
            //Map的find函数寻找元素elem出现的位置，返回对应位置的迭代器，若没有出现则返回尾元素的下一个位置
            pos = mapSteadyCount2FlatNode.find(down);
            // 该节点未进行稳态调度
            if (pos == mapSteadyCount2FlatNode.end())
            {
                // 得到上端节点的稳态调度系数（这个稳态调度系数就是一个迭代器，它指向Map中的指定节点《first，second》 《结点，执行次数》）
                pos = mapSteadyCount2FlatNode.find(up);
                x = pos->second;
                nPush *= x; // 为什么是x*nPush呢？理解稳态调度的概念--节点在流水线稳定运行中执行的最少次数 = 每次push * 稳态执行次数
                if (nPush != 0)
                {
                    // nPush, nPop的最小公倍数;
                    nLcm = lcm(nPush, nPop);
                    int temp = nLcm / nPush; //上下节点达到平衡时，需要的执行次数
                    if (temp != 1)           // 加一个判断，提高效率，乘1是不必要的
                    {
                        // 根据计算规则得来的
                        for (pos = mapSteadyCount2FlatNode.begin(); pos != mapSteadyCount2FlatNode.end(); ++pos)
                            pos->second *= temp;
                    }
                    mapSteadyCount2FlatNode.insert(make_pair(down, nLcm / nPop));
                }
                else // 对push(0)作处理
                {
                    // 取 1 值
                    mapSteadyCount2FlatNode.insert(make_pair(down, 1));
                }
                // 将down加入listNode是为了对down的输出节点进行调度,相当于遍历
                flatNodeList.push_back(down);
            }
            else //该节点已进行稳态调度，检查SDF图是否存在稳态调度系列，一般不存在的话表明程序有误
            {
                y = pos->second; //下方节点的执行次数
                pos = mapSteadyCount2FlatNode.find(up);
                x = pos->second; //上端节点的执行次数
                //nPop == 0 说明在进行join 0 操作
                if ((nPop != 0) && (nPush * x) != (nPop * y))
                {
                    cout << "不存在稳态调度..." << endl;
                    system("pause");
                    exit(1); // 表示不存在稳态调度
                }            //if
            }                //else
        }                    //for
        if (flatNodeList.size() == 0)
            break; // 链表为空，说明所有节点已调度完毕
        up = flatNodeList.front();
        flatNodeList.pop_front(); //对已经调度完的节点移除list
    }                             //while
    return true;
}

bool SchedulerSSG::InitScheduling()
{
    list<FlatNode *> flatNodeList;
    std::map<FlatNode *, int>::iterator pos;
    FlatNode *down = NULL, *up = NULL;
    int nPush = 0, nPop = 0, nPeek = 0;
    int x, y, i, j, n, num = 0;
    //现在考虑的是只有一个输出口的情况
    //找到sink节点
    for (i = 0; i < flatNodes.size(); ++i)
    {
        if (!flatNodes[i]->nOut)
        {
            down = flatNodes[i];
            num++;
        }
    }
    if (num > 1)
    {
        fprintf(stdout, "FATAL ERROR: 程序存在多个输出口！\n");
        system("pause");
        exit(1);
    }
    if (num == 0)
    {
        fprintf(stdout, "FATAL ERROR: 程序无输出口！\n");
        system("pause");
        exit(1);
    }
    //每个节点的初始化调度次数初始值为0
    mapInitCount2FlatNode.insert(make_pair(down, 0));
    while (1)
    {
        //遍历该节点的所有输入节点
        for (i = 0; i < down->nIn; i++)
        {
            //找到下端节点的peek、pop值
            nPeek = down->inPeekWeights[i];
            nPop = down->inPopWeights[i];
            up = down->inFlatNodes[i];

            //找到对应上端节点的push值
            for (j = 0; up->outFlatNodes[j] != down; j++)
                ;

            nPush = up->outPushWeights[j];
            pos = mapInitCount2FlatNode.find(down);
            //下端节点已有的初始化调度次数
            x = pos->second;
            //下端节点运行一次需要的额外数据量
            y = nPeek - nPop;
            if (y <= 0 || nPeek <= nPush)
                y = 0;
            if (nPush != 0)
                //ceil函数为上取整函数
                n = ceil((x * nPop + y) / float(nPush));
            else
                n = 0;
            pos = mapInitCount2FlatNode.find(up);
            if (pos == mapInitCount2FlatNode.end()) //zww：20120322，为了找没有输出的节点而修改
            {
                mapInitCount2FlatNode.insert(make_pair(up, n));
                flatNodeList.push_back(up);
            }
            else
            {
                if (pos->second < n)
                {
                    pos->second = n;
                    //该节点的初始化调度次数已改变，必须重新加入队列对其上端节点进行调度
                    flatNodeList.push_back(up);
                }
            }
        } //for
        if (flatNodeList.size() == 0)
            break; //链表为空，说明所有节点已调度完毕
        down = flatNodeList.front();
        flatNodeList.pop_front();
    }
    return true;
}

//求a,b的最大公约数
int SchedulerSSG::gcd(int a, int b)
{
    int r = 0;
    if (a < b)
    {
        r = a;
        a = b;
        b = r;
    }
    assert(b);
    while (a % b)
    {
        assert(b);
        r = a % b;
        a = b;
        b = r;
    }

    return b;
}

//求a,b的最小公倍数
int SchedulerSSG::lcm(int a, int b)
{
    int product = a * b;

    return product / gcd(a, b);
}

int SchedulerSSG::GetSteadyCount(FlatNode *node)
{
	std::map<FlatNode *, int> ::iterator pos;
	pos = mapSteadyCount2FlatNode.find(node);
	assert(pos!=mapSteadyCount2FlatNode.end());
	return pos->second;
}
