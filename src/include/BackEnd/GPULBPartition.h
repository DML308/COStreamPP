#ifndef _GPU_LB_PARTITION_H
#define _GPU_LB_PARTITION_H

//#define OLDPARTITION  //使用旧划分算法

#include "Partition.h"
#include "ActorStateDetector.h"
#include <vector>
#include <algorithm>

class GPULBPartition :public Partition
{
	enum FlatNodeState{ INSIDE = 0, BORDER, ALONE };
public:
	GPULBPartition(int CPUt, int GPUn, int K);
	 ~GPULBPartition();


	//重载父类虚函数
	void SssgPartition(SchedulerSSG *sssg);

	//区分Stateless和statefull节点
	void DetectiveActorsState(SchedulerSSG *SSSG);
	//获取并行度并排序
	bool SortActorsWithParall(SchedulerSSG *SSSG);
	//划分剩余的节点集合到GPU或者CPU集合中，这一步主要目的最小化GPU-CPU子图通信
	void PartitionOtherNodes(SchedulerSSG *SSSG);
	//评估GPU部分的工作量和CPU部分工作量,同时评估GPUnode的并行度和可用GPU资源，决定使用多少块GPU
	void EstimateWorkloadOfEachPart(SchedulerSSG *SSSG);
	//边界调整，负载平衡确定CPU线程数量
	void CalculateCPUthreads(SchedulerSSG *SSSG);
	void PartitionCPUNodes(SchedulerSSG *SSSG);
	void AddPartitionNum(SchedulerSSG *SSSG);


	bool findNDrange(FlatNode* flatNode);
	//void rWorkCompute(Node *from);
	//void WEST_astwalk(Node *n);
	//void WEST_listwalk(List *l);

	//获取点的状态，是边界，内部，孤零节点
	GPULBPartition::FlatNodeState getFlatNodeState(FlatNode *p);
	void updateCandidate(SchedulerSSG *sssg, vector<FlatNode *>& X0, vector<FlatNode *>& S, int index);
	int chooseMaxGain(SchedulerSSG *sssg, vector<FlatNode *>& S, vector<FlatNode *>& Xi, vector<FlatNode *>& X0);
	int findID(SchedulerSSG *sssg, FlatNode *flatnode);
	void errorDecrease(SchedulerSSG *sssg, int k,int work);
	int getPart(FlatNode *p){
		for (int i = 0; i < X.size(); i++){
			if (find(X[i].begin(), X[i].end(), p) != X[i].end())return i;
		}
	}
	int getTotalEdge(SchedulerSSG *sssg, int k);

	std::vector<FlatNode *> GetGPUNodeSet();
	std::vector<FlatNode *> GetCPUNodeSet();
	

	int Index2Device(int index);
	int GetGpuNum(){return GPUNum;}
	int GetCpuThreadNum(){ return CPUthread; }
	void sort(std::vector<FlatNode *> &statelessNode);

	vector<FlatNode *> FindNodeInOneDevice(int parts);
	void SetMultiNum2FlatNode(SchedulerSSG *SSSG);
public:
	std::map<FlatNode *, int> MultiNum2FlatNode;
	int K;                //扩大因子  //不在使用扩大因子 ，直接K=1；
private:
	int CPUthread;        //最大可用CPU核心数
	int GPUNum;           //最大可用GPU数量
	int SPn;			  //GPU硬件参数单gpu流处理器个数
	int Pthreshold;       //并行度阈值
	int T;                // CPU/GPU性能比，通过测试程序可调整

	float GPUworkload;				 //GPU部分的原始工作量
	float CPUworkload;				 //cpu部分的原始工作量
	int averageParallofGPUActors;    //加权并行度

	int GPUworkTime;				 //GPU部分的预估相对执行时间
	FlatNode *currentFlatNode;
	bool hasSpecalGPUnode;

	std::vector<FlatNode *> statelessNode;//所有无状态节点集合
	std::vector<FlatNode *> GPUNodes;     //所有GPU节点
	std::vector<FlatNode *> CPUNodes;     //所有CPU节点集合
	std::map<FlatNode*, int> flatNodeParall; //并行度

	vector<vector<FlatNode *>> X;//CPU子图划分的结果
	vector<int> w;//划分的K个子图每个子图的总工作量
	vector<int> vwgt; //每个顶点的权重(总工作量)
	vector<int> edge;//划分的K个子图每个子图的通信边的权重

	int nvtxs;    //定义顶点个数
	//std::vector<FlatNode *> flatNodes_;
	int GPUCPU_SPEEDUP = 50;

};


#endif