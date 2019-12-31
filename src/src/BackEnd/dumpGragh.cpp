#include "6.schedulerSSG.h"
#include "Partition.h"
static stringstream buf;
static SchedulerSSG *ssg;
static Partition *mpp;
//146种颜色，划分部分大于给定颜色种类则无法表示
static string color[] = {"aliceblue", "antiquewhite", "yellowgreen", "aquamarine", "azure",
                         "magenta", "maroon", "mediumaquamarine", "mediumblue", "mediumorchid",
                         "mediumpurple", "mediumseagreen", "mediumslateblue", "mediumspringgreen", "mediumturquoise",
                         "mediumvioletred", "midnightblue", "mintcream", "mistyrose", "moccasin",
                         "navajowhite", "navy", "oldlace", "olive", "olivedrab",
                         "orange", "orangered", "orchid", "palegoldenrod", "palegreen",
                         "paleturquoise", "palevioletred", "papayawhip", "peachpuff", "peru",
                         "pink", "plum", "powderblue", "purple", "red",
                         "rosybrown", "royalblue", "saddlebrown", "salmon", "sandybrown",
                         "seagreen", "seashell", "sienna", "silver", "skyblue",
                         "slateblue", "slategray", "slategrey", "snow", "springgreen",
                         "steelblue", "tan", "teal", "thistle", "tomato",
                         "deeppink", "deepskyblue", "dimgray", "dimgrey", "dodgerblue",
                         "firebrick", "floralwhite", "forestgreen", "fuchsia", "gainsboro",
                         "ghostwhite", "gold", "goldenrod", "gray", "grey",
                         "green", "greenyellow", "honeydew", "hotpink", "indianred",
                         "indigo", "ivory", "khaki", "lavender", "lavenderblush",
                         "lawngreen", "lemonchiffon", "lightblue", "lightcoral", "lightcyan",
                         "lightgoldenrodyellow", "lightgray", "lightgreen", "lightgrey", "lightpink",
                         "lightsalmon", "lightseagreen", "lightskyblue", "lightslategray", "lightslategrey",
                         "lightsteelblue", "lightyellow", "lime&nbsp", "limegreen", "linen",
                         "beige", "bisque", "yellow", "blanchedalmond", "blue",
                         "blueviolet", "brown", "burlywood", "cadetblue", "chartreuse",
                         "chocolate", "coral", "cornflowerblue", "cornsilk", "crimson",
                         "cyan", "darkblue", "darkcyan", "darkgoldenrod", "darkgray",
                         "darkgreen", "darkgrey", "darkkhaki", "darkmagenta", "darkolivegreen",
                         "darkorange", "darkorchid", "darkred", "darksalmon", "darkseagreen",
                         "darkslateblue", "darkslategray", "darkslategrey", "darkturquoise", "darkviolet",
                         "turquoise", "violet", "wheat", "white", "whitesmoke"};

/*访问该结点并打印相关的信息*/
void MyVisitNode(FlatNode *node)
{
    node->VisitNode();
    if (node->contents != NULL)
    {
        //print the name and multiplicities and some other crap
        //buf <<node->name << "[ label = \"" <<	node->GetOperatorName() << "\\n";
        buf << "\n"
            << node->name << "[ label = \"" << node->name << " \\n ";
        buf << "init Mult: " << ssg->mapFlatNode2InitCount.find(node)->second << " steady Mult: " << ssg->mapFlatNode2SteadyCount.find(node)->second << " \\n ";
        buf << "init work: " << ssg->mapInitWork2FlatNode.find(node)->second << " steady work:" << ssg->mapSteadyWork2FlatNode.find(node)->second << " \\n ";
        for (auto i : node->inPeekWeights)
            buf << " peek: " << i;
        if (node->nIn != 0)
            buf << " \\n";
        for (int j = 0; j < node->nIn; j++)
            buf << " pop: " << node->inPopWeights[j];
        if (node->nIn != 0)
            buf << " \\n";
        for (int k = 0; k < node->nOut; k++)
            buf << " push: " << node->outPushWeights[k];
        if (node->nOut != 0)
            buf << " \\n";
        buf << "\"";
        if(mpp != NULL)//Partition后则对节点着色
		{		
			buf<<" color=\""<<color[mpp->findPartitionNumForFlatNode(node)]<<"\""; 
			buf<<" style=\"filled\" "; 	
		}
        buf << " ]";
    }
    //假如当前遇到的node是一个多输入的结点，该结点的链接边如下处理
    if (node->nIn > 1)
    {
        for (int i = 0; i < node->nIn; i++)
        {
            //joiners may have null upstream neighbors
            assert(node->inFlatNodes[i] != NULL);
            buf << node->inFlatNodes[i]->name << " -> " << node->name;
            buf << "[label=\"" << node->inPopWeights[i] << "\"];\n";
            //buf << "[label=\"1\"];\n"; //输出权重固定为1
        }
    }

    //链接当前 node 和它的所有输出边
    for (int i = 0; i < node->nOut; i++)
    {
        if (node->outFlatNodes[i] == NULL)
            continue;
        if (node->outFlatNodes[i]->nIn > 1)
            continue;
        buf << node->name << " -> "
            << node->outFlatNodes[i]->name;
        buf << "[label=\"" << node->outPushWeights[i] << "\"];\n";
    }
}

void toBuildOutPutString(FlatNode *node)
{
    MyVisitNode(node);
    for (int i = 0; i < node->nOut; i++)
    {                                                                                
        /*深度优先遍历*/
        if (node->outFlatNodes[i] == NULL || node->outFlatNodes[i]->visitTimes != 0) 
        {
            //该结点的后续结点还未被访问过
            continue;
        }
        toBuildOutPutString(node->outFlatNodes[i]);
    }
}

void DumpStreamGraph(SchedulerSSG *sssg, Partition *mp,string fileName)
{
    ssg = sssg;
    mpp = mp;
    buf.str("");
    buf << "digraph Flattend {\n";
    toBuildOutPutString(ssg->topNode);
    buf << "\n\n}\n";
    ssg->ResetFlatNodeVisitTimes(); //将flatnode的visitimes置0
    ofstream fw;
    fw.open(fileName);
    fw << buf.str();
    fw.close();
}