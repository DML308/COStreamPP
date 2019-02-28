#ifndef _FLAT_NODE_H_
#define _FLAT_NODE_H_

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <math.h>
#include "node.h"

/*************************************************************************/
/*                                                                       */
/*                          SDF nodes                                    */
/*                                                                       */
/*************************************************************************/

class FlatNode
{
  public:
	string name;			   // opeator名字
	string PreName;			   //cwb记录Operator被重命名前的名字
	int visitTimes;			   // 表示该结点是否已经被访问过,与dumpdot有关
	operatorNode *contents;	//指向operator(经常量传播后的).
	operatorNode *oldContents; // 指向原始operator

	int nOut; // 输 出 边个数
	int nIn;  // 输 入 边个数

	//两级划分算法中,actor所在的place号、thread号、thread中的序列号
	int place_id;
	int thread_id, post_thread_id;
	int serial_id;

	vector<FlatNode *> outFlatNodes; // 输 出 边各operator
	vector<FlatNode *> inFlatNodes;  // 输 入 边各operator

	vector<int> outPushWeights; // 输 出 边各权重
	vector<int> inPopWeights;   // 输 入 边各权重
	vector<int> inPeekWeights;  // 输 入 边各权重

	vector<string> outPushString;
	vector<string> inPopString;
	vector<string> inPeekString;

	vector<string> pushString;
	vector<string> peekString;

	//节点work函数的静态工作量
	long work_estimate;

	// opeator在ssg的flatnodes中的顺序编号
	int num;

  public:
	FlatNode(operatorNode *node, Node *oldCom, compositeNode *newCom);
    string toString();
	void AddOutEdges(FlatNode *dest);
	void AddInEdges(FlatNode *src);
	void VisitNode(); // 访问该结点
	void ResetVisitTimes(); // 重置visitTimes信息
	// 获取该flatnode结点的operator的name
	string GetOperatorName()
	{
		return name;
	}
};

#endif 