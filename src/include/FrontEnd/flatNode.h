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

class  FlatNode
{
public:
	string name; // opeator名字
	string PreName;//cwb记录Operator被重命名前的名字

	operatorNode *contents;	//指向原有的operatorNode

	int nOut; // 输 出 边个数
	int nIn; // 输 入 边个数

	//两级划分算法中,actor所在的place号、thread号、thread中的序列号
	int place_id;
	int thread_id, post_thread_id;
	int serial_id;

	vector<FlatNode *> outFlatNodes; // 输 出 边各operator
	vector<FlatNode *> inFlatNodes; // 输 入 边各operator
	
	//vector<int> AddPopAtCodeGen;//	zww:20120313,一般情况下为零，但采用Horizontalfission时会产生额外的pop值，用此数据来记录
	vector<int> outPushWeights; // 输 出 边各权重
	vector<int> inPopWeights; // 输 入 边各权重
	vector<int> inPeekWeights; // 输 入 边各权重

	// 为类模板代码生成方案添加
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
	FlatNode(operatorNode *node, compositeNode *com, compositeNode *newCom);
	//FlatNode(operatorNode *node);//重载构造函数，为了以后在构造新的节点的时候使用
	void AddOutEdges(FlatNode *dest);
	void AddInEdges(FlatNode *src);
	string GetOperatorName(); // 获取该flatnode结点的operator的name
	
};

#endif /* _FLAT_NODE_H_ */