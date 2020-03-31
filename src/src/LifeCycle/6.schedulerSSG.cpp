//#define DEBUG
#include "6.schedulerSSG.h"
extern StaticStreamGraph *ssgs[MAX_SCOPE_DEPTH][MAX_SCOPE_DEPTH][MAX_SCOPE_DEPTH];
extern UnfoldComposite *unfold;

FlatNode* SchedulerSSG::findEndFlatNode(){
    //vector<FlatNode *> flatNodes = sssg->flatNodes;
    int i = 0;
    for (i = 0; i < flatNodes.size(); ++i)
    {
        vector<FlatNode *> outFlatNodes = flatNodes[i]->outFlatNodes;
        if(partFlatNodes.find(outFlatNodes[0]) == partFlatNodes.end()){
            endNode = flatNodes[i];
        }
    }
}

void getInOutputWeight(StaticStreamGraph *ssg)
{
    vector<FlatNode *> flat_nodes = ssg->flatNodes;
    for(int i=0;i<flat_nodes.size();i++){
        FlatNode *right_flatnode = flat_nodes[i];
        if(right_flatnode->real_faltnode){
            FlatNode *real_flatnode = right_flatnode->real_faltnode;
            right_flatnode->inPeekWeights = real_flatnode->inPeekWeights;
            right_flatnode->inPopWeights = real_flatnode->inPopWeights;
            right_flatnode->outPushWeights = real_flatnode->outPushWeights;

            vector<FlatNode *> copy_inflatnodes (real_flatnode->inFlatNodes.size());
            vector<FlatNode *> copy_outflatnodes (real_flatnode->outFlatNodes.size());
            copy(real_flatnode->inFlatNodes.begin(),real_flatnode->inFlatNodes.end(),copy_inflatnodes.begin());
            copy(real_flatnode->outFlatNodes.begin(),real_flatnode->outFlatNodes.end(),copy_outflatnodes.begin());
            right_flatnode->inFlatNodes = copy_inflatnodes;
            right_flatnode->outFlatNodes = copy_outflatnodes;

            right_flatnode->nIn = real_flatnode->nIn;
            right_flatnode->nOut = real_flatnode->nOut;

        }
    }
    
}

void ResizeDefalutWindow(){
    int max_level;
    int max_version;

    for(int i=0;i<10;i++){
        int max_level;
        int max_version;
        int level = 0;
        while(ssgs[i][level][0]!=NULL){
            level++;
        }
        max_level = level-1;
        if(max_level == 0){
            continue;
        }
        for(int m=0;m<=max_level;m++){
            for(int n=0;n<10;n++){
                if(ssgs[i][m][n]){
                     getInOutputWeight(ssgs[i][m][n]);
                }else{
                    break;
                }
            }
        }
        for(max_level;max_level>=0;max_level--){
            int version = 0;
            while(ssgs[i][max_level][version]!=NULL){
                version++;
            }
            max_version = version;
            for(int j=0;j<max_version;j++){
                StaticStreamGraph *right_ssg = ssgs[i][max_level][j];

                string opt_name = right_ssg->topNode->contents->operName;
                

                SchedulerSSG *right_sssg = new SchedulerSSG(right_ssg);
                //getInOutputWeight(right_ssg);
                if(opt_name.compare("Duplicate") == 0 || opt_name.compare("Roundrobin") == 0){
                    
                    FlatNode *split_node = right_ssg->topNode;
                    FlatNode *join_node = right_ssg->endNode;
                    FlatNode *real_split_node = split_node->real_faltnode;
                    FlatNode *real_join_node = join_node->real_faltnode;

                    if(split_node->contents->isDefaultWindowSize || join_node->contents->isDefaultWindowSize){
                    vector<FlatNode *> split_out_flatnodes = split_node->outFlatNodes;
                    vector<FlatNode *> join_in_flatnodes = join_node->inFlatNodes;
                    vector<int> split_out_sum,join_in_sum;

                    for(int i=0;i<split_out_flatnodes.size();i++){
                        split_out_sum.push_back(split_out_flatnodes[i]->inPeekWeights[0]);
                    }
                    for(int i=0;i<join_in_flatnodes.size();i++){
                        join_in_sum.push_back(join_in_flatnodes[i]->outPushWeights[0]);
                    }
                    int split_lcm = right_sssg->multyLcm(split_out_sum);
                    int join_lcm = right_sssg->multyLcm(join_in_sum);
                    
                    int split_sum = 0,join_sum = 0;

                    bool if_need_resize = false;
                    //if(split_lcm != 1){
                        list<constantNode *> *arguments_value = new list<constantNode *>();
                        for(int i=0;i<split_out_flatnodes.size();i++){
                            split_out_sum[i]/=split_lcm;
                            if(split_out_sum[i] != split_node->outPushWeights[i]){
                                if_need_resize = true;
                            }
                            split_sum+=split_out_sum[i];
                            arguments_value->push_back(new constantNode("long long",(long long)split_out_sum[i]));
                        }
                        
                        if(if_need_resize){
                            split_node->outPushWeights = split_out_sum;
                            split_node->inPopWeights[0] = split_sum;
                            split_node->inPeekWeights[0] = split_sum;

                            //修改后的flatnode要放回去
                            real_split_node->outPushWeights = split_out_sum;
                            real_split_node->inPopWeights[0] = split_sum;
                            real_split_node->inPeekWeights[0] = split_sum;

                            int style;
                            if(opt_name.compare("Duplicate") == 0){
                            style = 0;
                            }
                            if(opt_name.compare("Roundrobin") == 0 ){
                                style = 1;
                            }
                            split_node->contents->operBody->work = unfold->MakeRoundrobinWork(split_node->contents->inputs,arguments_value,split_node->contents->outputs,style);
                        }
                        

                    //}
                    
                    //if(join_lcm != 1){
                        if_need_resize = false;
                        //list<constantNode *> *arguments_value = new list<constantNode *>();
                        arguments_value->clear();
                        for(int i=0;i<join_in_flatnodes.size();i++){
                            join_in_sum[i]/=split_lcm;
                            if(join_in_sum[i] != join_node->inPeekWeights[i]){
                                if_need_resize = true;
                            }
                            join_sum+=join_in_sum[i];
                            arguments_value->push_back(new constantNode("long long",(long long)join_in_sum[i]));
                        }
                        
                        if(if_need_resize){
                            join_node->inPeekWeights = join_in_sum;
                            join_node->inPopWeights = join_in_sum;
                            join_node->outPushWeights[0] = join_sum;
                            //修改window 重新生成？
                            real_join_node->inPeekWeights = join_in_sum;
                            real_join_node->inPopWeights = join_in_sum;
                            real_join_node->outPushWeights[0] = join_sum;

                            join_node->contents->operBody->work = unfold->MakeJoinWork(join_node->contents->inputs,arguments_value,join_node->contents->outputs);
                        }
                    }

                    //}
                    

                }
                right_sssg->PartSteadyScheduling();
                //right_sssg->findEndFlatNode();
                FlatNode* innerFlatNode = new FlatNode();
                //FlatNode *find_top_node,*find_end_node;
                /*if(right_sssg->topNode->real_faltnode){
                    find_top_node = right_sssg->topNode->real_faltnode;
                }else{
                    find_top_node = right_sssg->topNode;
                }
                if(right_sssg->endNode->real_faltnode){
                    find_end_node = right_sssg->endNode->real_faltnode;
                }else{
                    find_end_node = right_sssg->endNode;
                }*/
                int real_input_sum = right_sssg->mapFlatNode2SteadyCount.find(right_sssg->topNode)->second * right_sssg->topNode->inPeekWeights.front();
                int real_output_sum = right_sssg->mapFlatNode2SteadyCount.find(right_sssg->endNode)->second * right_sssg->endNode->outPushWeights.front();
                
                int max_sale = right_sssg->gcd(real_input_sum,real_output_sum);
                real_output_sum /= max_sale;
                real_input_sum /= max_sale;
                innerFlatNode->inPeekWeights.push_back(real_input_sum);
                innerFlatNode->inPopWeights.push_back(real_input_sum);
                innerFlatNode->outPushWeights.push_back(real_output_sum);
                innerFlatNode->nIn = 1;
                innerFlatNode->nOut = 1;
                innerFlatNode->inFlatNodes = right_sssg->topNode->inFlatNodes;
                innerFlatNode->outFlatNodes = right_sssg->endNode->outFlatNodes;
                
                FlatNode* upperFlatNode = right_sssg->topNode->inFlatNodes[0]; //real flatnode
                for(int x = 0;x<10;x++){
                    StaticStreamGraph *upper_ssg = ssgs[i][max_level-1][x];
                    if(!upper_ssg){
                        break;
                    }
                    if(upper_ssg->partFlatNodes.find(upperFlatNode) != upper_ssg->partFlatNodes.end())
                    {
                        
                        upper_ssg->flatNodes.push_back(innerFlatNode);
                        upper_ssg->partFlatNodes.insert(make_pair(innerFlatNode,0));
                        
                        int index = 0;
                        for(index;index<upper_ssg->topNode->outFlatNodes.size();index++){
                            FlatNode* real_flatnode;
                            if(right_sssg->topNode->real_faltnode){
                                real_flatnode = right_sssg->topNode->real_faltnode;
                            }else{
                                real_flatnode = right_sssg->topNode;
                            }
                            if(upper_ssg->topNode->outFlatNodes[index] == real_flatnode){
                                break;
                            }
                            index++;
                        }
                        upper_ssg->topNode->outFlatNodes[index] = innerFlatNode;
                        
                        for(index=0;index<upper_ssg->endNode->inFlatNodes.size();index++){
                            FlatNode* real_flatnode;
                            if(right_sssg->endNode->real_faltnode){
                                real_flatnode = right_sssg->endNode->real_faltnode;
                            }else{
                                real_flatnode = right_sssg->endNode;
                            }
                            if(upper_ssg->endNode->inFlatNodes[index] == real_flatnode){
                                break;
                            }
                            index++;
                        }
                        upper_ssg->endNode->inFlatNodes[index] = innerFlatNode;
                        break;
                    }
                }
            }
        }
        

    }
}

SchedulerSSG *SchedulingSSG(StaticStreamGraph *ssg)
{
    SchedulerSSG *sssg = new SchedulerSSG(ssg);
    // 对默认窗口大小进行重新调整，使得能得到文涛调度
    ResizeDefalutWindow();
    if (sssg->SteadyScheduling())
    {
        sssg->InitScheduling();
#if 0
        cout << "稳态调度序列:" << endl;
        for (auto pos : sssg->mapFlatNode2SteadyCount)
            cout << pos.first->name << "\t" << pos.second << endl;
        cout << "初始化调度序列:" << endl;
        for (auto pos : sssg->mapFlatNode2InitCount)
            cout << pos.first->name << "\t" << pos.second << endl;

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
    partFlatNodes = ssg->partFlatNodes;
    endNode = ssg->endNode;
    flatNodes = ssg->flatNodes;
    topNode = ssg->topNode;
    mapEdge2UpFlatNode = ssg->mapEdge2UpFlatNode;
    mapEdge2DownFlatNode = ssg->mapEdge2DownFlatNode;
    mapSteadyWork2FlatNode = ssg->mapSteadyWork2FlatNode;
    mapInitWork2FlatNode = ssg->mapInitWork2FlatNode;
    
}



bool SchedulerSSG::PartSteadyScheduling()
{
    list<FlatNode *> flatNodeList;
    map<FlatNode *, int>::iterator pos;
    // 默认第一个节点是源，也就是说peek和pop均为0,在图的表示上暂不允许有多个源，但可以有多个peek = pop = 0节点
    FlatNode *up = topNode, *down = NULL;
    int nPush = 0, nPop = 0, nLcm = 0;
    int x, y, i, j;
    // 现在考虑的是只有一个输入口的情况
    while (1)
    {
        // 稳态调度系列初始系数为1
        if(up->real_faltnode){
            //up = up->real_faltnode;
            if(partFlatNodes.find(up->real_faltnode) == partFlatNodes.end() ){
            if (flatNodeList.size() == 0)
                break; // 链表为空，说明所有节点已调度完毕            up = flatNodeList.front();
            flatNodeList.pop_front();
            continue;
            }
        }else if(up->copy_flatnode){
            //up = up->real_faltnode;
            if(partFlatNodes.find(up) == partFlatNodes.end() ){
            if (flatNodeList.size() == 0)
                break; // 链表为空，说明所有节点已调度完毕            up = flatNodeList.front();
            flatNodeList.pop_front();
            continue;
            }
            up = up->copy_flatnode;
        }
        
        

        mapFlatNode2SteadyCount.insert(make_pair(up, 1));
        // 遍历该节点的所有输出节点（广度优先遍历）
        for (i = 0; i < up->nOut; ++i)
        {
            
            nPush = up->outPushWeights[i]; // 上端节点的push值
            down = up->outFlatNodes[i];    // 找到下端节点
            
            if(down->copy_flatnode){
                down = down->copy_flatnode;
            }

            for (j = 0; j < down->inFlatNodes.size(); j++){
                if(down->inFlatNodes[j] == up || down->inFlatNodes[j] == up->real_faltnode){
                    break;
                }
            }
                                 // 下端节点找到与上端节点对应的标号
            nPop = down->inPopWeights[j]; // 下端节点取出对应的pop值
            // 检查该节点是否已进行稳态调度，每条只进行一次稳态调度
            //Map的find函数寻找元素elem出现的位置，返回对应位置的迭代器，若没有出现则返回尾元素的下一个位置
            pos = mapFlatNode2SteadyCount.find(down);
            // down 节点未进行稳态调度
            if (pos == mapFlatNode2SteadyCount.end())
            {
                debug("对 down 节点 %s 进行稳态调度\n",down->name.c_str());
                // 得到上端节点的稳态调度系数（这个稳态调度系数就是一个迭代器，它指向Map中的指定节点《first，second》 《结点，执行次数》）
                pos = mapFlatNode2SteadyCount.find(up);
                x = pos->second;
                nPush *= x; // 为什么是x*nPush呢？理解稳态调度的概念--节点在流水线稳定运行中执行的最少次数 = 每次push * 稳态执行次数
                line("nPush %d nPop %d\n",nPush, nPop);
                if (nPush != 0)
                {
                    // nPush, nPop的最小公倍数;
                    nLcm = lcm(nPush, nPop);
                    int temp = nLcm / nPush; //上下节点达到平衡时，需要的执行次数
                    if (temp != 1)           // 加一个判断，提高效率，乘1是不必要的
                    {
                        // 根据计算规则得来的
                        for (pos = mapFlatNode2SteadyCount.begin(); pos != mapFlatNode2SteadyCount.end(); ++pos)
                            pos->second *= temp;
                    }
                    mapFlatNode2SteadyCount.insert(make_pair(down, nLcm / nPop));
                }
                else // 对push(0)作处理
                {
                    // 取 1 值
                    mapFlatNode2SteadyCount.insert(make_pair(down, 1));
                }
                // 将down加入listNode是为了对down的输出节点进行调度,相当于遍历
                flatNodeList.push_back(down);
            }
            else //该节点已进行稳态调度，检查SDF图是否存在稳态调度系列，一般不存在的话表明程序有误
            {
                y = pos->second; //下方节点的执行次数
                pos = mapFlatNode2SteadyCount.find(up);
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

bool SchedulerSSG::SteadyScheduling()
{
    list<FlatNode *> flatNodeList;
    map<FlatNode *, int>::iterator pos;
    // 默认第一个节点是源，也就是说peek和pop均为0,在图的表示上暂不允许有多个源，但可以有多个peek = pop = 0节点
    FlatNode *up = topNode, *down = NULL;
    int nPush = 0, nPop = 0, nLcm = 0;
    int x, y, i, j;
    // 现在考虑的是只有一个输入口的情况
    while (1)
    {
        // 稳态调度系列初始系数为1
        mapFlatNode2SteadyCount.insert(make_pair(up, 1));
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
            pos = mapFlatNode2SteadyCount.find(down);
            // down 节点未进行稳态调度
            if (pos == mapFlatNode2SteadyCount.end())
            {
                debug("对 down 节点 %s 进行稳态调度\n",down->name.c_str());
                // 得到上端节点的稳态调度系数（这个稳态调度系数就是一个迭代器，它指向Map中的指定节点《first，second》 《结点，执行次数》）
                pos = mapFlatNode2SteadyCount.find(up);
                x = pos->second;
                nPush *= x; // 为什么是x*nPush呢？理解稳态调度的概念--节点在流水线稳定运行中执行的最少次数 = 每次push * 稳态执行次数
                line("nPush %d nPop %d\n",nPush, nPop);
                if (nPush != 0)
                {
                    // nPush, nPop的最小公倍数;
                    nLcm = lcm(nPush, nPop);
                    int temp = nLcm / nPush; //上下节点达到平衡时，需要的执行次数
                    if (temp != 1)           // 加一个判断，提高效率，乘1是不必要的
                    {
                        // 根据计算规则得来的
                        for (pos = mapFlatNode2SteadyCount.begin(); pos != mapFlatNode2SteadyCount.end(); ++pos)
                            pos->second *= temp;
                    }
                    mapFlatNode2SteadyCount.insert(make_pair(down, nLcm / nPop));
                }
                else // 对push(0)作处理
                {
                    // 取 1 值
                    mapFlatNode2SteadyCount.insert(make_pair(down, 1));
                }
                // 将down加入listNode是为了对down的输出节点进行调度,相当于遍历
                flatNodeList.push_back(down);
            }
            else //该节点已进行稳态调度，检查SDF图是否存在稳态调度系列，一般不存在的话表明程序有误
            {
                y = pos->second; //下方节点的执行次数
                pos = mapFlatNode2SteadyCount.find(up);
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
    map<FlatNode *, int>::iterator pos;
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
    mapFlatNode2InitCount.insert(make_pair(down, 0));
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
            pos = mapFlatNode2InitCount.find(down);
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
            pos = mapFlatNode2InitCount.find(up);
            if (pos == mapFlatNode2InitCount.end()) //zww：20120322，为了找没有输出的节点而修改
            {
                mapFlatNode2InitCount.insert(make_pair(up, n));
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

int SchedulerSSG::multyLcm(vector<int>nums){
    int right_lcm = 1;
    for(int i=0;i<nums.size();i++){
        right_lcm = lcm(right_lcm,nums[i]);
    }
    return right_lcm;
}

int SchedulerSSG::GetInitCount(FlatNode *node)
{
	std::map<FlatNode *, int> ::iterator pos;
	pos = mapFlatNode2InitCount.find(node);
	assert(pos!= mapFlatNode2InitCount.end());
	return pos->second;
}
int SchedulerSSG::GetSteadyCount(FlatNode *node)
{
	map<FlatNode *, int> ::iterator pos;
	pos = mapFlatNode2SteadyCount.find(node);
	assert(pos!=mapFlatNode2SteadyCount.end());
	return pos->second;
}
