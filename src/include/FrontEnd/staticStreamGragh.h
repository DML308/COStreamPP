#ifndef _STATIC_STREAM_GRAPH_H_
#define _STATIC_STREAM_GRAPH_H_

#include "flatNode.h"
#include <set>

class StaticStreamGraph
{
  public:
    StaticStreamGraph() {}
    
};

StaticStreamGraph *AST2FlatStaticStreamGraph(compositeNode *mainComposite);

#endif