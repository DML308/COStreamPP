#ifndef _X86CODEGENERATE_H_
#define _X86CODEGENERATE_H_
#include "schedulerSSG.h"
#include "ActorStageAssignment.h"
#include "Partition.h"
#include <set>
extern list<Node*> *Program;
class X86CodeGeneration
{
public:
  /******************构造函数*******************/
  X86CodeGeneration(int, SchedulerSSG *, const char *, StageAssignment *, Partition *);
  /********************************************/
  void CGMakefile();
  void CGGlobalvar();
  void CGGlobalvarHeader();
  void CGGlobal();
  void CGGlobalHeader();

private:
  StageAssignment *psa_;
  Partition *mp_;
  SchedulerSSG *sssg_;
  vector<FlatNode *> flatNodes_;
  //ActorEdgeInfo *pEdgeInfo; //存放各个边的类型信息
  int nCpucore_;
  map<FlatNode *, int> mapFlatNode2Place; // 存放各 FlatNode 对应的 place序号
  stringstream declList, declInitList, stateInit;
  stringstream strScheduler, parameterBuf, thisBuf;
  stringstream globalvarbuf, temp_declInitList; //globalvarbuf包含全局变量的声明
  stringstream declInitList_temp;
  int n_fdcl; //记录C函数声明的个数，用来正确规范函数声明代码格式
  int declInitLen; //记录declInitList初始长度，用来恢复函数定义前的内容
  int flag; //源代码中是否存在C函数调用定义标志
  string OutputPath;
  string InputPath;
  FlatNode *readerActor, *writerActor;                    //标识读写文件操作的actor
  map<operatorNode *, string> mapOperator2ClassName; // 存放各 class 对应的 composite
  multimap<FlatNode *, string> mapActor2InEdge;           //actor对应输入边的名称
  multimap<FlatNode *, string> mapActor2OutEdge;          //actor对应输出边的名称
  map<int, set<int>> mapNum2Stage;                        //处理器编号到stage的对应关系
  vector<string> staticNameInit;                          //存放actor内state和var成员的初始化，被输出到actor对应的源文件中
  vector<string> ptrname;                                 //用于存放每个actor中动态生成的数组的名称，以便于后面的delete操作
  vector<string> nDeclDim;                                //用于存放未定义的全局数组维度
};

#endif