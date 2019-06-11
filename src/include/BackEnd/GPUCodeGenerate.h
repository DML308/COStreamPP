#ifndef _GPUCODEGENERATE_H_
#define _GPUCODEGENERATE_H_

#include "6.schedulerSSG.h"
#include "ActorStageAssignment.h"
#include "GPULBPartition.h"
#include "stdio.h"
#include <set>
#include <stack>
#include <vector>
using namespace std;
/***************************************************/
/*********************GPU(OpenCL)代码生成*******************/
/***************************************************/
extern list<Node *> *Program;
bool PrintTime = false;//cwb 打印各部分的运行时间
bool IsMod = true;//缓冲区取数据时是否使用取余操作
//生成多个文件
class GPUCodeGenerate
{
public:
    /******************构造函数******************/
	GPUCodeGenerate(SchedulerSSG *, const char *,StageAssignment *,GPULBPartition*);
    void CGGlobalvar();//生成全局变量
    void CGGlobalvarextern();
    void CGThreads();
    void CGglobalHeader();
    void CGactors();
    void CGAllKernel();
    void CGglobalCpp();
    void CGMain();//生成启动线程的main文件

    void CGThread(int index,stringstream&buf);
    void CGactor(FlatNode *actor);
    void CGexternbuffer(FlatNode *actor,stringstream &buf);
    void CGdatatag(FlatNode *actor,stringstream &buf);
    void CGEdgeParam(FlatNode *actor,stringstream &buf,vector<string> inEdgeName,vector<string> outEdgeName);
    void CGactorsStmts(stringstream &buf, list<Node *> *stmts);
    void CGactorsinitVarAndState(stringstream &buf, list<Node *> *stmts);
    void CGactorsInit(stringstream &buf, Node *init);
    void CGpopToken(FlatNode *actor,stringstream &buf,vector<string> inEdgeName);
    void CGpushToken(FlatNode *actor,stringstream &buf,vector<string> outEdgeName);
    void CGpopTokenForGPU(FlatNode *actor,stringstream &buf,vector<string> inEdgeName);
    void CGpushTokenForGPU(FlatNode *actor,stringstream &buf,vector<string> outEdgeName);
    void CGthis(FlatNode *actor, stringstream &buf,vector<string> inEdgeName,vector<string> outEdgeName);
    void CGdatataginit(FlatNode *actor,stringstream &buf);
    void CGinitWork(stringstream &buf);
    void CGwork(FlatNode *actor,stringstream &buf,vector<string> inEdgeName,vector<string> outEdgeName,Node *work);
    void CGdataGetforGPU(FlatNode *actor,stringstream &buf,vector<string> inEdgeName);
    void CGdataSendforGPU(FlatNode *actor,stringstream &buf,vector<string> outEdgeName);
    void CGrunInitScheduleWork(FlatNode *actor,stringstream &buf,vector<string> inEdgeName,vector<string> outEdgeName);
    void CGrunInitScheduleWorkforGPU(FlatNode *actor,stringstream &buf);
    void CGrunSteadyScheduleWork(FlatNode *actor,stringstream &buf,vector<string> inEdgeName,vector<string> outEdgeName);
    void CGrunSteadyScheduleWorkforGPU(FlatNode *actor,stringstream &buf);
    bool DataDependence(FlatNode *actora,FlatNode *actorb);//a->b
    bool ExistDependence(FlatNode *actor);
    bool IsUpBorder(FlatNode *actor);
    bool IsDownBorder(FlatNode *actor);

    string ReturnTypeOfEdge(FlatNode *actorA,FlatNode *actorB);
    string ReturnNameOfEdge(FlatNode *actorA,FlatNode *actorB);
    string ReturnTypeOfEdgestr(string typestr);
    string ReturnNameOfEdgestr(string namestr);
    string ReturnNameOfTheEdge(string searchstring);//此处的参数形如A->name_B->name
    int ReturnPushWeight(FlatNode *actorA,FlatNode *actorB);
    int ReturnPeekWeight(FlatNode *actorA,FlatNode *actorB);
    int ReturnBufferSize(FlatNode *actora,FlatNode *actorb);//a->b
    int ReturnNumBiger(int size);


private:
	StageAssignment *pSa;
	GPULBPartition *Maflp;
	SchedulerSSG *sssg_;	
	std::vector<FlatNode *> flatNodes_;
	map<string,string> mapOperator2EdgeName;   //2019  用处很奇怪，莫名其妙
	int nGpu_;			  //最终实际使用GPU数量
	int CPUthread;        //最终实际使用CPU线程   
	int nActors_;
    bool extractDecl, isInParam;
    int MultiNum;
    FlatNode* curactor;
    int Comm_num =1;//buzhidao shisha 
    //int LocalSize = 1;    //为数据传输分配空间 用于传输
    int LocalSizeofWork = 32;    //NDrange

	stringstream strScheduler, parameterBuf, thisBuf;

    multimap<FlatNode*,string> mapActor2InEdge;
	multimap<FlatNode*,string> mapActor2OutEdge;
    map<int,set<int> > mapNum2Stage;//处理器编号到stage的对应关系
    map<string,string>allnameandtype;//用于存储所有的边的名称和类型，主要作用是生成globalheader中的自定义结构体
    map<string, string> EdgeNameToDefinetype;
    map<string,map<string,string>>edgestrnameandtype;//
    map<map<FlatNode *,FlatNode *>,map<string,string>>edge2nameandtype;//存储边的类型,前面两个FlatNode表示结点，第一个string表示该边对应的结构体包含变量的类型如double，第二个string表示其名称如x
	map<map<FlatNode*,FlatNode*>,string>BufferNameOfEdge;//用于存放结点之间边的名称
    map<string,string>edge2name;//存储边与其名称的对应关系

    vector<map<string,map<string,string>>>staticvariable,staticvariablefinal;//用于存储actor的静态变量，第一个string参数是变量名，第二个map中：若参数为二维数组则string存储其维数，为一维数组则第二个string为0，为常数则两个string都为0
	vector<string>staticvartype,staticvartypefinal;//用于存储与上面map对应的参数的类型
    vector<map<string,map<string,string>>>globalvariable,globalvariablefinal;//同上
    vector<string>globalvartype,globalvartypefinal;//同上
	set<string>array2Dname,array2Dnamefinal;//将static和global中的二维数组名称存储起来，用于kernel的生成
	set<string>array1Dname,array1Dnamefinal;//
	set<string>arraylocal2Dname;//将局部二维动态数组存起来，用于kernel的生成

    map<FlatNode *,vector<string>>Actor2KernelPar;//用于存放传给每个actor与其kernel的参数映射关系
	map<FlatNode *,vector<string>>Actor2KernelParType;//用于存放传给每个actor与其kernel的参数类型映射关系
	map<FlatNode*,vector<string>>OutbufferOfActor;//用于存放每个actor输出buffer
	map<FlatNode*,vector<string>>InbufferOfActor;//用于存放每个actor中为输入actor在CPU上时创建的输入buffer
	vector<string> ptrname;//用于存放每个actor中动态生成的数组的名称，以便于后面的delete操作
	vector<string> ptrtype;//用于存放与上面vector对应参数的类型
    multimap<string,string>ptrdim;//用于保存与上面对应的数组的位数
	multimap<FlatNode *,map<string,string>>actor2ptr;//用于存放所有的actor中动态生成的数组参数信息

    map<FlatNode *,int>Node2QueueID;//记录结点到GPU队列的映射关系
    map<int,int>thread2queue;//将线程号映射到队列号

    multimap<string,string> mapedge2xy;//存放各边的类型
	multimap<string,string> mapedge2xy2;//mapedge2xy的副本
    multimap<FlatNode *,map<string,map<string,string>>>allstaticvariable;//用于存储所有actor的静态变量
	multimap<FlatNode *,vector<string>>allstatictype;
	multimap<FlatNode *,map<string,map<string,string>>>alllocalvariable;//用于存储所有actor动态生成的局部数组
};

#endif