#include "staticStreamGragh.h"
void StaticStreamGraph::GenerateFlatNodes(operatorNode *u, Node *oldComposite, compositeNode *newComposite)
{
    //cout<<u->operName<<endl;
    FlatNode *src = NULL, *dest = NULL;
    src = new FlatNode(u, oldComposite, newComposite);
    list<Node *> *outputs = NULL;
    list<Node *> *inputs = NULL;
    outputs = (u->outputs != NULL) ? u->outputs : new list<Node *>();
    inputs = (u->inputs != NULL) ? u->inputs : new list<Node *>();
    /* 寻找输出流  建立节点的输入输出流关系*/
    assert(inputs!=NULL);
    for (auto it : *outputs)
    {
        src->nOut++;
        //cout<<"output Name = "<<((idNode*)it)->name<<endl;
        mapEdge2UpFlatNode.insert(make_pair(it, src));
    }
    // for(auto it:mapEdge2UpFlatNode){
    //     cout<<((idNode*)(it.first))->name<<endl;
    // }
    //cout<<"---------------------------------------"<<endl;
    //cout << "mapEdge2UpFlatNode.size()= " << mapEdge2UpFlatNode.size() << endl;
    flatNodes.push_back(src);
    dest = src; //边变了
    //搜索节点的输入边，建立节点流输入输出关系
    for (auto it : (*inputs))
    {

        src->nIn++;
        //cout<<"name= "<<((idNode*)it)->name<<endl;
        //将“有向边”与其“下”端operator绑定
        mapEdge2DownFlatNode.insert(make_pair(it, dest));
        //这里不应该是it（输入节点） 应该是本节点
        // auto pos = mapEdge2UpFlatNode.find(it);
        // assert(pos != mapEdge2UpFlatNode.end()); //确保每一条输入边都有operator
        // src = pos->second;
        // src->AddOutEdges(dest);
        // dest->AddInEdges(src);
    }

    //cout<<"mapEdge2DownFlatNode.size()= "<<mapEdge2DownFlatNode.size()<<endl;
}

void StaticStreamGraph::ResetFlatNodeNames()
{
    for (int i = 0; i < flatNodes.size(); i++)
    {
        stringstream newName;
        newName << flatNodes[i]->name << "_" << i;
        flatNodes[i]->name = newName.str();
        //cout << flatNodes[i]->name << endl;
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
    /*
    for (int i = 0; i < flatNodes.size(); i++)
    {
        string tmpName = "";
        FlatNode *flatNode = flatNodes[i];
        operatorNode *contents = flatNode->contents;
        list<Node *> *win_stmts = contents->operBody->win->winStmt_List;
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
        if (win_stmts != NULL)
        {
            for (auto it : *win_stmts)
            {

            }
        }
    }
    */
}