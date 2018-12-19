#include "staticStreamGragh.h"

void StaticStreamGraph::GenerateFlatNodes(operatorNode *u, Node *oldComposite, compositeNode *newComposite)
{
    FlatNode *src = NULL, *dest = NULL;
    src = new FlatNode(u, oldComposite, newComposite);

    list<Node *> *outputs = NULL;
    list<Node *> *inputs = NULL;
    outputs = (u->outputs!=NULL) ? u->outputs : new list<Node *>();
    inputs = (u->inputs!=NULL) ? u->inputs : new list<Node *>();
    // cout<<"input.size() = "<<inputs->size()<<endl;
    // cout<<"output.size() = "<<outputs->size()<<endl;
    /* 寻找输出流  建立节点的输入输出流关系*/
    //cout<<u->operName<<endl;
    for (auto it : *outputs)
    {
        //cout<<it->type<<endl;
        //cout<<((idNode*)it)->name<<endl;
        mapEdge2UpFlatNode.insert(make_pair(it, src));
    }
    //cout<<"mapEdge2UpFlatNode.size()= "<<mapEdge2UpFlatNode.size()<<endl;
    flatNodes.push_back(src);
    //mapOper2FlatNode.insert(make_pair(u,src));
    dest = src; //边变了
    //搜索节点的输入流，建立节点流输入输出关系
    for (auto it : *inputs)
    {
        //将“有向边”与其“下”端operator绑定
        //cout<<it->type<<endl;
        mapEdge2DownFlatNode.insert(make_pair(it, dest));
        //这里不应该是it（输入节点） 应该是本节点
        //auto pos = mapEdge2UpFlatNode.find(it);
        //assert(pos != mapEdge2UpFlatNode.end()); //确保每一条输入边都有operator
        //src = pos->second;
        //src->AddOutEdges(dest);
        //dest->AddInEdges(src);
    }
    
    //cout<<"mapEdge2DownFlatNode.size()= "<<mapEdge2DownFlatNode.size()<<endl;
}

void FlatNode::AddOutEdges(FlatNode *dest)
{
    outFlatNodes.push_back(dest);
    ++nOut;
}

void FlatNode::AddInEdges(FlatNode *src)
{
    inFlatNodes.push_back(src);
    ++nIn;
}
