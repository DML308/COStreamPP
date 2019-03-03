#include "flatNode.h"
#include "util.h"
/* 后两个参数暂时都未用到 ，暂时不清楚oldcontent和content的区别 */
FlatNode::FlatNode(operatorNode *node, Node *oldCom, compositeNode *newCom)
{
    name = node->operName;
    contents = node;
    oldContents = contents;
    nOut = 0;
    nIn = 0;
    visitTimes = 0;
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

void FlatNode::ResetVisitTimes()
{
	visitTimes = 0;
}

/** 
 * 输出 FlatNode 的各项参数,如果该参数是默认值(例如0)则略去不输出
 * 输出的样式类似于{ name: Source,visitTimes: 1, nOut: 1, nIn: 1, outFlatNodes:[F], inFlatNodes:[C] }
*/
string FlatNode::toString(){
    string s = "{ name: "+name;
    if(PreName.length()>0) s+= +", preName: "+PreName;
    if(visitTimes>0) s+= ", visitTimes: "+to_string(visitTimes);
    if(nOut>0) s += ", nOut: " + to_string(nOut);
    if(nIn>0) s += ", nIn: " + to_string(nIn);
    if(outPushWeights.size()>0)
        s += ", outPushWeights:" + vectorToString(outPushWeights);
    if(outPushString.size()>0)
        s += ", outPushString:" + vectorToString(outPushString);
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