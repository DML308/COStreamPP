#include "staticStreamGragh.h"
void StaticStreamGraph::GenerateFlatNodes(operatorNode *u, Node *oldComposite,compositeNode *newComposite){
    FlatNode *src = NULL, *dest = NULL;
    Node *item = NULL, *node = NULL;
    src = new FlatNode(u, oldComposite, newComposite);
    list<Node*> *outputs=u->outputs;
    list<Node*> *inputs=u->inputs;
    /* 寻找输出流  建立节点的输入输出流关系*/
    for(auto it:*outputs){
        mapEdge2UpFlatNode.insert(make_pair(it, src));
    }
    flatNodes.push_back(src);
    dest = src; //边变了
	//找输入边
    for(auto it:*inputs){
        //将“有向边”与其“下”端operator绑定
        mapEdge2DownFlatNode.insert(make_pair(it, dest));
        auto pos = mapEdge2UpFlatNode.find(it);
		assert(pos != mapEdge2UpFlatNode.end()); //确保每一条输入边都有operator
		src = pos->second;
		src->AddOutEdges(dest);
		dest->AddInEdges(src);
    }

}


void FlatNode::AddOutEdges(FlatNode* dest)
{
	outFlatNodes.push_back(dest);
	++nOut;
}

void FlatNode::AddInEdges(FlatNode *src)
{
	inFlatNodes.push_back(src);
	++nIn;
}
