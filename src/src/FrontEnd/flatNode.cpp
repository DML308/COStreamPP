#include "flatNode.h"
/* 后两个参数暂时都未用到 ，暂时不清楚oldcontent和content的区别 */
FlatNode::FlatNode(operatorNode *node, Node *oldCom, compositeNode *newCom)
{
    name = node->operName;
    contents = node;
    oldContents = contents;
    nOut = 0;
    nIn = 0;
}


void FlatNode::AddOutEdges(FlatNode *dest)
{
    outFlatNodes.push_back(dest);
    nOut++;
}

void FlatNode::AddInEdges(FlatNode *src)
{
    nIn++;
    inFlatNodes.push_back(src);
}

void FlatNode::VisitNode()
{
	visitTimes++;
}

void FlatNode::ResetVistTimes()
{
	visitTimes = 0;
}

string FlatNode::toString(){
    string s = "{ ";
    s+= "name: "+name+", preName: "+PreName;
    s+= ", visitTimes: "+to_string(visitTimes);
    s += ", nOut: " + to_string(nOut);
    s += ", nIn: " + to_string(nIn);
    if(outFlatNodes.size()>0){
        s+=", outFlatNodes:[";
        int k = 0;
        for(auto i :outFlatNodes){
            s += (k++ > 0 ? "," : "") + i->name;
        }
        s+="]";
    }
    if (inFlatNodes.size() > 0)
    {
        s += ", inFlatNodes:[";
        int k = 0;
        for (auto i : inFlatNodes)
        {
            s += (k++ > 0 ? "," : "") + i->name;
        }
        s+="]";
    }
    s+= " }";
    return s;
}