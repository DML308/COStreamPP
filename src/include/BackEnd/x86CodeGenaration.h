#ifndef _X86CODEGENERATE_H_
#define _X86CODEGENERATE_H_
#include "schedulerSSG.h"
#include "ActorStageAssignment.h"
#include "Partition.h"
#include <set>
extern list<Node *> *Program;
class X86CodeGeneration
{
public:
  /******************构造函数*******************/
  X86CodeGeneration(int, SchedulerSSG *, const char *, StageAssignment *, Partition *);
  /********************************************/
  void CGMakefile();                                                                                    //生成Makefile文件
  void CGGlobalvar();                                                                                   //生成流程序引入的全局变量定义文件 GlobalVar.cpp
  void CGGlobalvarHeader();                                                                             //生成流程序引入的全局变量的声明文件 GlobalVar.h
  void CGGlobal();                                                                                      //生成流程序的所有缓冲区信息Global.cpp
  void CGGlobalHeader();                                                                                //生成流程序的所有缓冲区声明Global.h
  void CGactors();                                                                                      //生成以类表示的计算单元actor
  void CGactorsConstructor(FlatNode *actor,stringstream &buf, string className, string inEdgeName, string outEdgeName); //actor构造函数
  void CGThreads();                                                                                     //生成所有线程

private:
  StageAssignment *psa_;                             //阶段赋值
  Partition *mp_;                                    //划分结果
  SchedulerSSG *sssg_;                               //静态数据流图
  vector<FlatNode *> flatNodes_;                     //节点集合
  int nCpucore_;                                     //设置cpu核的数量
  int nActors_;                                      //节点总数量
  map<FlatNode *, int> mapFlatNode2Place;            // 存放各 FlatNode 对应的 place序号
  map<operatorNode *, string> mapOperator2ClassName; // 存放各 class 对应的 composite
  multimap<FlatNode *, string> mapActor2InEdge;      //actor对应输入边的名称
  multimap<FlatNode *, string> mapActor2OutEdge;     //actor对应输出边的名称
  map<int, set<int>> mapNum2Stage;                   //处理器编号到stage的对应关系
};
#endif