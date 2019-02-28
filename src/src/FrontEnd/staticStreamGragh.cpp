#include "staticStreamGragh.h"
void StaticStreamGraph::GenerateFlatNodes(operatorNode *u, Node *oldComposite, compositeNode *newComposite)
{
    cout<<"-----------------"<<u->operName<<"--------------------"<<endl;
    FlatNode *src = NULL, *dest = NULL;
    src = new FlatNode(u, oldComposite, newComposite);
    list<Node *> *outputs = NULL;
    list<Node *> *inputs = NULL;
    outputs = (u->outputs != NULL) ? u->outputs : new list<Node *>();
    inputs = (u->inputs != NULL) ? u->inputs : new list<Node *>();
    /* 寻找输出流  建立节点的输入输出流关系*/
    for (auto it : *outputs)
    {
        //cout<<"output Name = "<<((idNode*)it)->name<<endl;
        mapEdge2UpFlatNode.insert(make_pair(((idNode *)it)->name, src));
    }
    //cout << "mapEdge2UpFlatNode.size()= " << mapEdge2UpFlatNode.size() << endl;
    cout << "operatorNode:\t" << u->toString() << endl << "    ↓" << endl;
    flatNodes.push_back(src);
    dest = src;
    //搜索节点的输入边，建立节点流输入输出关系
    assert(inputs != NULL);
    for (auto it : *inputs)
    {
        //cout<<"input name= "<<((idNode*)it)->name<<endl;
        //将“有向边”与其“下”端operator绑定
        mapEdge2DownFlatNode.insert(make_pair(((idNode *)it)->name, dest));
        auto pos = mapEdge2UpFlatNode.find(((idNode *)it)->name);
        assert(pos != mapEdge2UpFlatNode.end()); //确保每一条输入边都有operator
        src = pos->second;
        src->AddOutEdges(dest);
        dest->AddInEdges(src);
    }
    cout << "flatNode:\t" <<dest->toString() << endl<<endl;
    //cout << "mapEdge2DownFlatNode.size()= " << mapEdge2DownFlatNode.size() << endl;
    //cout<<"-----------------operator end------------------------------"<<endl;
}

/**
 * @brief 给静态数据流图中所有的 FlatNode 加上数字后缀来表达顺序, 如 Source => Source_0
 */
void StaticStreamGraph::ResetFlatNodeNames()
{
    for (int i = 0; i < flatNodes.size(); i++)
    {
        stringstream newName;
        newName << flatNodes[i]->name << "_" << i;
        flatNodes[i]->name = newName.str();
    }
} 

/*
1.若该operator没有输入，则FlatNode成员变量inPeekWeights，inPopWeights均为0
2.若该operator没有输出，则FlatNode成员变量inPeekWeights，inPopWeights根据4来定
3.若该operator有输出，有输入，则FlatNode成员变量inPeekWeights，inPopWeights根据4来定
4.一个window是和一条边对应的，inPeekWeights，inPopWeights，outPushWeights按如下方法计算
(1)所有window的outPushWeights均用窗口标识ID和Tumbling关键字指定
(2)若该window为sliding风格，则inPeekWeights，inPopWeights由第1,2个count值决定
(3)若该window为tumbling风格，则inPeekWeights和inPopWeights相等均由第一个count决定
(4)由于是静态计算，因此不对window的排空和触发策略为time类型做计算，只限于count类型的window策略
*/
// 计算每个节点的push，pop，peek值
void StaticStreamGraph::SetFlatNodesWeights()
{
    for (int i = 0; i < flatNodes.size(); i++)
    {
        string tmpName = "";
        FlatNode *src = NULL, *dest = NULL;
        FlatNode *flatNode = flatNodes[i];
        operatorNode *contents = flatNode->contents;
        list<Node *> *win_stmts = contents->operBody->win->win_list;
        // if(win_stmts!=NULL)
        // cout<<"win_stmt.size()= "<<win_stmts->size()<<endl;
        for (int j = 0; j < flatNode->nIn; j++)
        {
            // 在inPeekWeights为空的情况下，不能使用数组下标（inPeekWeights[j]）访问，否则出现断言错误，以下类同
            flatNode->inPeekWeights.push_back(1);
            flatNode->inPopWeights.push_back(1);
            tmpName = "nPeek_" + to_string(j);
            flatNode->inPeekString.push_back(tmpName);
            tmpName = "nPop_" + to_string(j);
            flatNode->inPopString.push_back(tmpName);
        }
        for (int j = 0; j < flatNode->nOut; j++)
        {
            flatNode->outPushWeights.push_back(1); // 对于每一个FlatNode来说都为1，下同
            tmpName = "nPush_" + to_string(j);
            flatNode->outPushString.push_back(tmpName);
        }
        //cout<<"win_stmt.size() ="<<win_stmts->size()<< endl;
        if (win_stmts != NULL)
        {
            for (auto it : *win_stmts)
            {
                assert(it->type == WindowStmt);
                string edgeName = ((winStmtNode *)it)->winName;
                auto pos = mapEdge2UpFlatNode.find(edgeName);
                assert(pos != mapEdge2UpFlatNode.end());
                src = pos->second; // 每条边有且只有一个上端节点
                // 说明该window指示的是peek和pop值
                if (src != flatNode)
                {
                    NodeType type = ((winStmtNode *)it)->winType->type;
                    assert(type == Tumbling || type == Sliding);
                    int j;
                    for (j = 0; src != flatNode->inFlatNodes[j]; j++)
                        ; //找到对应的j,准备写入值
                    if (type == Sliding)
                    {
                        Node *winType = ((winStmtNode *)it)->winType;
                        list<Node *> *args = ((slidingNode *)winType)->arg_list;
                        assert(args != NULL && args->size() == 2);
                        Node *first = args->front();
                        Node *second = args->back();
                        long long first_val = ((constantNode *)first)->llval;
                        long long second_val = ((constantNode *)second)->llval;
                        if (first_val < second_val)
                        {
                            error("peek must be greater than pop!\npeek = %d, pop = %d\n", first_val, second_val);
                            exit(-127);
                        }
                        flatNode->inPopWeights[j] = second_val;
                        flatNode->inPeekWeights[j] = first_val;
                    }
                    else if (type == Tumbling)
                    {
                        Node *winType = ((winStmtNode *)it)->winType;
                        Node *val = ((tumblingNode *)winType)->arg_list->front();
                        assert(val->type == constant);
                        flatNode->inPeekWeights[j] = ((constantNode *)val)->llval;
                        flatNode->inPopWeights[j] = flatNode->inPeekWeights[j];
                    }
                }
                // 说明该window指示的是push值
                else
                {
                    auto pos = mapEdge2DownFlatNode.find(edgeName);
                    assert(pos != mapEdge2DownFlatNode.end());
                    dest = pos->second;
                    int j;
                    for (j = 0; dest != flatNode->outFlatNodes[j]; j++)
                        ; //找到对应的j,准备写入值
                    NodeType type = ((winStmtNode *)it)->winType->type;
                    if (type == Tumbling)
                    {
                        Node *winType = ((winStmtNode *)it)->winType;
                        Node *val = ((tumblingNode *)winType)->arg_list->front();
                        flatNode->outPushWeights[j] = ((constantNode *)val)->llval;
                    }
                    else if (type == Sliding)
                    {
                        flatNode->outPushWeights[j] = 0;
                    }
                }
            }
        }
    }
}

void StaticStreamGraph::AddInitWork(FlatNode *flat, int work)
{
    mapInitWork2FlatNode.insert(make_pair(flat, work));
}
void StaticStreamGraph::AddSteadyWork(FlatNode *flat, int work)
{
    mapSteadyWork2FlatNode.insert(make_pair(flat, work));
}
/*重置ssg结点flatnodes内所有flatnode内的visitimes*/
void StaticStreamGraph::ResetFlatNodeVisitTimes()
{
    for (int i = 0; i < flatNodes.size(); i++)
    {
        this->flatNodes[i]->ResetVisitTimes();
    }
}