#include "GPUCodeGenerate.h"
using namespace std;

bool PrintFlag =false;//是否打印结果，默认不输出
GPUCodeGenerate::GPUCodeGenerate(SchedulerSSG *Sssg, const char *currentDir,StageAssignment *psa,GPULBPartition *maflptemp)
{
	//pEdgeInfo = new ActorEdgeInfo(Sssg);
	pSa = psa;
	Maflp=maflptemp;
	sssg_ = Sssg;
	flatNodes_ = Sssg->GetFlatNodes();
	nGpu_ = maflptemp->GetGpuNum();
	CPUthread = maflptemp->GetCpuThreadNum();
	nActors_ = flatNodes_.size();
    MultiNum = Maflp->K;

	vector<FlatNode*>::iterator iter1,iter2,iter3;
	multimap<FlatNode*,string>::iterator iter4,iter5;
	for (iter1=flatNodes_.begin();iter1!=flatNodes_.end();++iter1)
	{
		for (iter2=(*iter1)->inFlatNodes.begin();iter2!=(*iter1)->inFlatNodes.end();++iter2)
		{
			string tempstring1=(*iter2)->name+"_"+(*iter1)->name;//得到形如A_B的边名称
			mapActor2InEdge.insert(make_pair((*iter1),tempstring1));
		}
		for (iter3=(*iter1)->outFlatNodes.begin();iter3!=(*iter1)->outFlatNodes.end();++iter3)
		{
			string tempstring2=(*iter1)->name+"_"+(*iter3)->name;
			mapActor2OutEdge.insert(make_pair((*iter1),tempstring2));
		}
	}
	vector<FlatNode*>tempactors;
	vector<FlatNode*>::iterator iter;
	set<int>tempstageset;
	for (int i=0;i<nGpu_+1;i++)
	{
		tempstageset.clear();
		tempactors=Maflp->FindNodeInOneDevice(i);
		multimap<int,map<FlatNode*,int>>::iterator iter_datastage;
		map<FlatNode*,int>::iterator iter_submap;
		for (iter=tempactors.begin();iter!=tempactors.end();++iter)
		{
			tempstageset.insert(pSa->FindStage(*iter));
			for (iter_datastage = pSa->datastage.begin();iter_datastage != pSa->datastage.end();++iter_datastage)
			{
				iter_submap = iter_datastage->second.begin();
				if (iter_submap->first->name == (*iter)->name)
				{
					tempstageset.insert(iter_datastage->first);
				}
			}
		}
		mapNum2Stage.insert(make_pair(i,tempstageset));
	}
	/*string tmp = dir_ + "lib\\";
	_mkdir(tmp.c_str()); */

	extractDecl = false;
	isInParam = false;
}

void GPUCodeGenerate::CGGlobalvar()
{
    stringstream buf;
    buf << "#include \"GlobalVar.h\"\n";
    for (auto iter : *Program)
    {
        if (iter->type == Decl)
            buf << iter->toString()+ ";" << "\n";
    }
    ofstream out("GlobalVar.cpp");
    out << buf.str();
}

void GPUCodeGenerate::CGGlobalvarextern()
{
    stringstream buf, res;
    buf << "#ifndef GLOBALVAL_H\n";
    buf << "#define GLOBALVAL_H\n";
    for (auto iter : *Program)
    {
        if (iter->type == Decl)
            buf << "extern " + iter->toString() << "\n";
    }
    buf << "#endif";
    string line;
    bool flag = false;
    while (getline(buf, line))
    {
        string str;
        auto pos = line.find("=");
        if (pos != string::npos)
        {
            str = line.substr(0, pos) + ";";
        }
        else
            str = line;
        res << str << "\n";
    }
    ofstream out("GlobalVar.h");
    out << res.str();
}

void GPUCodeGenerate::CGThreads()
{
	string threadname;
	stringstream buf;

	string numname;//存储thread结尾数字转化为字符串
	//nGpu_*2+NCpuThreads  每个GPU有各自的传输线程？
	for (int i = 0; i<nGpu_ * 2 + CPUthread; i++)
	{
		buf<<"/*该文件定义各thread的入口函数，在函数内部完成软件流水迭代*/\n";
        string filename = "thread_" + to_string(i) + ".cpp";
		CGThread(i,buf);      
        ofstream out(filename);
        out << buf.str();
		buf.str("");
	}

}

void GPUCodeGenerate::CGThread(int index,stringstream&buf)
{

	string numname=to_string(index);
	int totalstagenum=pSa->MaxStageNumForGPU();
	vector<FlatNode *>tempactorset;
	vector<FlatNode *>::iterator iter;
	set<int>*ptempstagenum;
	pair<multimap<FlatNode *,string>::iterator,multimap<FlatNode *,string>::iterator>pos1,pos2;
	map<int,set<int>>::iterator iter1;
	buf<<"#include \"Buffer.h\"\n";
	buf<<"#include \"Producer.h\"\n";
	buf<<"#include \"Consumer.h\"\n";
	buf<<"#include \"global.h\"\n";
	buf<<"#include \"barrier_sync.h\"\t//包含barrier函数\n";
	buf<<"#include \"AllActorHeader.h\"\t//包含所有actor的头文件\n";
	buf<<"#include \"rdtsc.h\"\n";
    buf<<"//extern pthread_barrier_t barrier;\n";
    if (PrintTime)
	{
		if(index > nGpu_ && index < 2*nGpu_+1)                           //传输线程
			buf<<"extern double cpu_gpu_com"<<index-nGpu_-1<<";\n";
		else if(index == nGpu_)                                          //cpu计算线程
			buf<<"extern double cpu0_compute;\n";
		else if(index >= 2*nGpu_+1)
			buf<<"extern double cpu"<<index-2*nGpu_<<"_compute;\n";      //cpu计算线程
		else
			buf<<"extern double gpu"<<index<<"_compute;\n";              //gpu计算线程
	}
	for (int i = 0;i < nGpu_;i++)
	{
		buf<<"extern cl::CommandQueue queue_"<<i+1<<"_0;\n";
		buf<<"extern cl::CommandQueue queue_"<<i+1<<"_1;\n";
	}
    buf<<"extern int MAX_ITER;\n";
    bool IsDataTransfer = false;
	if (index > nGpu_ && index < 2*nGpu_+1)
	{
		IsDataTransfer = true;
		//tempactorset=Maflp->findNodeSetInPartition(index-nGpu_-1);
	}
	tempactorset = Maflp->FindNodeInOneDevice(index);
    for (iter=tempactorset.begin();iter!=tempactorset.end();++iter)
    {
        buf<<"extern "<<(*iter)->name<<" "<<(*iter)->name<<"_obj;\n";
    }
	if (index)
	{
		buf<<"void thread_"<<numname<<"_fun()\n{\n";
		//buf<<"\tworkerSync("<<index<<");\n";
	}
	else
	{
		buf<<"void thread_"<<numname<<"_fun()\n{\n";
	}
	buf<<"\tchar stage["<<totalstagenum<<"]={0};\n";
	buf<<"\tstage[0]=1;\n";
	if(PrintTime)
	buf<<"\ttsc_counter t0,t1;\n";
	if (IsDataTransfer)
	{
		iter1=mapNum2Stage.find(index-nGpu_-1);
	}
	else
	{
		if(index == nGpu_ || index >= 2*nGpu_+1)
			iter1=mapNum2Stage.find(nGpu_);
		else
			iter1=mapNum2Stage.find(index);
	}
	ptempstagenum=&iter1->second;
	set<int>::iterator enditer = ptempstagenum->end();

	buf<<"\tfor(int _stageNum=0;_stageNum<"<<totalstagenum<<";_stageNum++)\n";
	buf<<"\t{\n";

    //生成各线程的调用
	for (int i=totalstagenum-1;i>=0;i--)	//迭代stage
	{
		set<int>::iterator stageiter = ptempstagenum->find(i);
		if(stageiter!=enditer){					//该stage在该thread上
			vector<FlatNode*> flatVec = pSa->FindActor(i);
			vector<FlatNode*>::iterator iter1;
			if (flatVec.size())
			{	
				if (!IsDataTransfer)
				{
					//cpu线程
					if (index == nGpu_ || index >= 2*nGpu_+1)
					{
						buf<<"\t\tif("<<i<<"==_stageNum)\n\t\t{\n";
						for(iter1=flatVec.begin();iter1!=flatVec.end();++iter1)
						{
							if (Maflp->findPartitionNumForFlatNode(*iter1) == index)
							{
								buf<<"\t\t\t"<<(*iter1)->name<<"_obj.runInitScheduleWork(";
								//获取当前actor输入stream和输出stream
								multimap<FlatNode*,string>::iterator iter3,iter2;
								iter3=mapActor2InEdge.find((*iter1));
								for(int inflatnodeindex =0;inflatnodeindex< (*iter1)->inFlatNodes.size();inflatnodeindex++)
								{
									if (!DataDependence((*iter1)->inFlatNodes[inflatnodeindex],*iter1))
									{
											buf<<"&"<<iter3->second<<"[0][0],";
									}
									++iter3;
								}
								iter2=mapActor2OutEdge.find((*iter1));
								for(int outflatnodeindex =0;outflatnodeindex< (*iter1)->outFlatNodes.size();outflatnodeindex++)
								{
									if (!DataDependence(*iter1,(*iter1)->outFlatNodes[outflatnodeindex]))
									{
											buf<<"&"<<iter2->second<<"[0][0],";
									}
									++iter2;
								}
								buf<<"NULL);\n";
							}
						}
						buf<<"\t\t}\n";
					}
					//GPU控制线程
					else if(index < nGpu_)
					{
						buf<<"\t\tif("<<i<<"==_stageNum)\n\t\t{\n";
						for(iter1=flatVec.begin();iter1!=flatVec.end();++iter1)
						{
							if(Maflp->findPartitionNumForFlatNode(*iter1) == Maflp->Index2Device(index))
							{
								buf<<"\t\t\t"<<(*iter1)->name<<"_obj.runInitScheduleWork_"<<index<<"();\n";
							}
						}
						buf<<"\t\t}\n";
					}
				}
			}
			//传输线程,所有边界节点都在这里
			if(IsDataTransfer)
			{
				flatVec = pSa->FindDataOfActor(i);
				if (flatVec.size())
				{
					buf<<"\t\tif("<<i<<"==_stageNum)\n\t\t{\n";
					map<FlatNode*,int>::iterator iter_subdatastage;
					for(iter1=flatVec.begin();iter1!=flatVec.end();++iter1)
					{
						pair<multimap<int,map<FlatNode*,int>>::iterator,multimap<int,map<FlatNode*,int>>::iterator>pos = pSa->datastage.equal_range(i);//
						if(Maflp->Index2Device(index-nGpu_-1) == Maflp->findPartitionNumForFlatNode(*iter1))
						{
							while(pos.first!=pos.second)
							{
								for (iter_subdatastage = pos.first->second.begin();iter_subdatastage != pos.first->second.end();iter_subdatastage++)
								{
									if (iter_subdatastage->first->name == (*iter1)->name)
									{
										//如果是读数据节点
										if (iter_subdatastage->second == 1)
										{
											buf<<"\t\t\t\t"<<(*iter1)->name<<"_obj.dataget_"<<index-nGpu_-1<<"("<<(*iter1)->name<<"_obj.initScheduleCount,";
											multimap<FlatNode*,string>::iterator iter3;
											//bianli shuru bian s
											iter3=mapActor2InEdge.find((*iter1));
											for(int inflatnodeindex =0;inflatnodeindex< (*iter1)->inFlatNodes.size();inflatnodeindex++)
											{
												if (!DataDependence((*iter1)->inFlatNodes[inflatnodeindex],*iter1))
												{
													if(IsDownBorder((*iter1)->inFlatNodes[inflatnodeindex]) && IsUpBorder(*iter1))
													{
														buf<<"&"<<iter3->second<<"[0][0],";
													}
												}
												++iter3;
												
											}
											buf<<"NULL);\n";
										}
										else//如果是发送数据节点
										{
											buf<<"\t\t\t\t"<<(*iter1)->name<<"_obj.datasend_"<<index-nGpu_-1<<"("<<(*iter1)->name<<"_obj.initScheduleCount,";
											multimap<FlatNode*,string>::iterator iter2;
											iter2=mapActor2OutEdge.find((*iter1));
											for(int outflatnodeindex =0;outflatnodeindex< (*iter1)->outFlatNodes.size();outflatnodeindex++)
											{
												if (!DataDependence(*iter1,(*iter1)->outFlatNodes[outflatnodeindex]))
												{
													if(IsUpBorder((*iter1)->outFlatNodes[outflatnodeindex]) && IsDownBorder(*iter1))
														buf<<"&"<<iter2->second<<"[0][0],";
												}
												++iter2;
											}
											buf<<"NULL);\n";
										}
									}
								}
								++pos.first;
							}
							//buf<<"\t\t\t\t"<<(*iter1)->name<<"_obj.datatransform();\n";

						}
					}
					buf<<"\t\t}\n";
				}
			}
		}
	}
	if (index > nGpu_ && index < 2*nGpu_+1)
	{
		buf<<"\t\tqueue_"<<index-nGpu_<<"_0.finish();\n";
		buf<<"\t\tqueue_"<<index-nGpu_<<"_1.finish();\n";
	}
	else if (index < nGpu_)
	{
		buf<<"\t\tqueue_"<<index+1<<"_0.finish();\n";
		buf<<"\t\tqueue_"<<index+1<<"_1.finish();\n";
	}
	buf<<"\t\tawait();\n";
	buf<<"\t\n\t//pthread_barrier_wait (&barrier);\n\t}\n";
	buf<<"\tfor(int _stageNum=0;_stageNum<"<<totalstagenum<<"+MAX_ITER-1;_stageNum++)\n\t{\n";
	if(PrintTime)
	buf<<"\t\tRDTSC(t0);\n";
	for (int i=totalstagenum-1;i>=0;i--)	//迭代stage
	{			
		set<int>::iterator stageiter = ptempstagenum->find(i);
		if(stageiter!=enditer){					//该stage在该thread上
			vector<FlatNode*> flatVec = pSa->FindActor(i);
			vector<FlatNode*>::iterator iter1;
			if (flatVec.size())
			{
				if (!IsDataTransfer)
				{
					if (index == nGpu_ || index >= 2*nGpu_+1)
					{
						buf<<"\t\tif(stage["<<i<<"])\n\t\t{\n";
						for(iter1=flatVec.begin();iter1!=flatVec.end();++iter1)
						{
							if(Maflp->findPartitionNumForFlatNode(*iter1) == index)
							{
								buf<<"\t\t\t"<<(*iter1)->name<<"_obj.runSteadyScheduleWork(";
								multimap<FlatNode*,string>::iterator iter3,iter2;
								iter3=mapActor2InEdge.find((*iter1));
								for(int inflatnodeindex =0;inflatnodeindex< (*iter1)->inFlatNodes.size();inflatnodeindex++)
								{
									if (!DataDependence((*iter1)->inFlatNodes[inflatnodeindex],*iter1))
									{
										buf<<"&"<<iter3->second<<"[(_stageNum-"<<i<<")%(sizeof("<<(*iter1)->inFlatNodes[inflatnodeindex]->name<<"_"<<(*iter1)->name<<")/sizeof("<<(*iter1)->inFlatNodes[inflatnodeindex]->name<<"_"<<(*iter1)->name<<"[0]))]["<<sssg_->GetInitCount(*iter1)*(*iter1)->inPopWeights[inflatnodeindex]<<"],";
									}
									++iter3;
								}
								iter2=mapActor2OutEdge.find((*iter1));

								for(int outflatnodeindex =0;outflatnodeindex< (*iter1)->outFlatNodes.size();outflatnodeindex++)
								{
									if (!DataDependence(*iter1,(*iter1)->outFlatNodes[outflatnodeindex]))
									{
										buf<<"&"<<iter2->second<<"[(_stageNum-"<<i<<")%(sizeof("<<(*iter1)->name<<"_"<<(*iter1)->outFlatNodes[outflatnodeindex]->name<<")/sizeof("<<(*iter1)->name<<"_"<<(*iter1)->outFlatNodes[outflatnodeindex]->name<<"[0]))]["<<sssg_->GetInitCount(*iter1)*(*iter1)->outPushWeights[outflatnodeindex]<<"],";
									}
									++iter2;
								}
								buf<<"NULL);\n";
							}
						}
						buf<<"\t\t}\n";
					}
					else //!(index == nGpu_ || index >= 2*nGpu_+1) //非传输线程
					{
						buf<<"\t\tif(stage["<<i<<"])\n\t\t{\n";
						for(iter1=flatVec.begin();iter1!=flatVec.end();++iter1)
						{
							if(Maflp->findPartitionNumForFlatNode(*iter1) == Maflp->Index2Device(index))
							{
								buf<<"\t\t\t"<<(*iter1)->name<<"_obj.runSteadyScheduleWork_"<<index<<"();\n";
								if (PrintFlag)
								{
									if ((*iter1)->outFlatNodes.size() == 0 && (*iter1)->GPUPart != nGpu_)
									{
										buf<<"\t\t\t"<<(*iter1)->name<<"_obj.print_"<<index<<"("<<sssg_->GetSteadyCount(*iter1)*Maflp->MultiNum2FlatNode[*iter1]<<");\n";
									}
								}
							}
						}
						buf<<"\t\t}\n";
					}
				}
			}
			if(IsDataTransfer)
			{
				flatVec = pSa->FindDataOfActor(i);
				if (flatVec.size())
				{
					buf<<"\t\tif(stage["<<i<<"])\n\t\t{\n";
					map<FlatNode*,int>::iterator iter_subdatastage;
					for(iter1=flatVec.begin();iter1!=flatVec.end();++iter1)
					{
						pair<multimap<int,map<FlatNode*,int>>::iterator,multimap<int,map<FlatNode*,int>>::iterator>pos = pSa->datastage.equal_range(i);
						if(Maflp->Index2Device(index-nGpu_-1) == Maflp->findPartitionNumForFlatNode(*iter1))
						{

							while(pos.first!=pos.second)
							{
								for (iter_subdatastage = pos.first->second.begin();iter_subdatastage != pos.first->second.end();iter_subdatastage++)
								{
									if (iter_subdatastage->first->name == (*iter1)->name)
									{
										if (iter_subdatastage->second == 1)
										{
											//buf<<"\t\t\t\t"<<(*iter1)->name<<"_obj.dataget_"<<index-nGpu_-1<<"("<<(*iter1)->name<<"_obj.steadyScheduleCount,(_stageNum-"<<i+index-nGpu_-1<<")%"<<(nGpu_+1)<<");\n";
											buf<<"\t\t\t\t"<<(*iter1)->name<<"_obj.dataget_"<<index-nGpu_-1<<"("<<(*iter1)->name<<"_obj.steadyScheduleCount,";
											multimap<FlatNode*,string>::iterator iter3;
											iter3=mapActor2InEdge.find((*iter1));
											for(int inflatnodeindex =0;inflatnodeindex< (*iter1)->inFlatNodes.size();inflatnodeindex++)
											{
												if (!DataDependence((*iter1)->inFlatNodes[inflatnodeindex],*iter1))
												{
													if(IsDownBorder((*iter1)->inFlatNodes[inflatnodeindex]) && IsUpBorder(*iter1))
													{
														int j;
														for (j = 0; j < (*iter1)->inFlatNodes[inflatnodeindex]->outFlatNodes.size(); ++j)
														{
															if((*iter1)->inFlatNodes[inflatnodeindex]->outFlatNodes[j] == *iter1)
																break;
														}
														buf<<"&"<<iter3->second<<"[(_stageNum-"<<i<<")%(sizeof("<<(*iter1)->inFlatNodes[inflatnodeindex]->name<<"_"<<(*iter1)->name<<")/sizeof("<<(*iter1)->inFlatNodes[inflatnodeindex]->name<<"_"<<(*iter1)->name<<"[0]))]["<<sssg_->GetInitCount((*iter1)->inFlatNodes[inflatnodeindex])*(*iter1)->inFlatNodes[inflatnodeindex]->outPushWeights[j]+sssg_->GetSteadyCount(*iter1)*(*iter1)->inPopWeights[inflatnodeindex]*Maflp->MultiNum2FlatNode[*iter1]*(index-nGpu_-1)<<"],";
													}
												}
											}
											buf<<"NULL);\n";
										}
										else
										{
											//buf<<"\t\t\t\t"<<(*iter1)->name<<"_obj.datasend_"<<index-nGpu_-1<<"("<<(*iter1)->name<<"_obj.steadyScheduleCount,(_stageNum-"<<i+index-nGpu_-1<<")%"<<(nGpu_+1)<<");\n";
											buf<<"\t\t\t\t"<<(*iter1)->name<<"_obj.datasend_"<<index-nGpu_-1<<"("<<(*iter1)->name<<"_obj.steadyScheduleCount,";
											multimap<FlatNode*,string>::iterator iter2;
											iter2=mapActor2OutEdge.find((*iter1));
											for(int outflatnodeindex =0;outflatnodeindex< (*iter1)->outFlatNodes.size();outflatnodeindex++)
											{
												if (!DataDependence(*iter1,(*iter1)->outFlatNodes[outflatnodeindex]))
												{
													if(IsUpBorder((*iter1)->outFlatNodes[outflatnodeindex]) && IsDownBorder(*iter1))
														buf<<"&"<<iter2->second<<"[(_stageNum-"<<i<<")%(sizeof("<<(*iter1)->name<<"_"<<(*iter1)->outFlatNodes[outflatnodeindex]->name<<")/sizeof("<<(*iter1)->name<<"_"<<(*iter1)->outFlatNodes[outflatnodeindex]->name<<"[0]))]["<<sssg_->GetInitCount(*iter1)*(*iter1)->outPushWeights[outflatnodeindex]+sssg_->GetSteadyCount(*iter1)*(*iter1)->outPushWeights[outflatnodeindex]*Maflp->MultiNum2FlatNode[*iter1]*(index-nGpu_-1)<<"],";
												}
												++iter2;
											}
											buf<<"NULL);\n";
										}
									}
								}
								++pos.first;
							}
						}
					}
					buf<<"\t\t}\n";
				}
			}
		}
	}
	buf<<"\t\tfor(int index="<<totalstagenum-1<<"; index>= 1; --index)\n";
	buf<<"\t\t\tstage[index] = stage[index-1];\n\t\tif(_stageNum == (MAX_ITER - 1))\n\t\t{\n\t\t\tstage[0]=0;\n\t\t}\n";
		//传输线程
	if (index > nGpu_ && index < 2*nGpu_+1)
	{
		buf<<"\t\tqueue_"<<index-nGpu_<<"_0.finish();\n";
	}
	//GPU线程
	else if (index < nGpu_)
	{
		buf<<"\t\tqueue_"<<index+1<<"_1.finish();\n";
	}
	if (PrintTime)
	{
		buf << "\t\tRDTSC(t1);\n";
		if (index < nGpu_)
			buf << "\t\tgpu" << index << "_compute += COUNTER_DIFF(t1,t0,1);\n";
		else if (index == nGpu_)
			buf << "\t\tcpu0_compute += COUNTER_DIFF(t1,t0,1);\n";
		else if (index >= 2 * nGpu_ + 1)
			buf << "\t\tcpu" << index - 2 * nGpu_ << "_compute += COUNTER_DIFF(t1,t0,1);\n";
		else
			buf << "\t\tcpu_gpu_com" << index - nGpu_ - 1 << " += COUNTER_DIFF(t1,t0,1);\n";
	}
	//传输线程
	if (index > nGpu_ && index < 2 * nGpu_ + 1)
	{
		buf << "\t\tqueue_" << index - nGpu_ << "_1.finish();\n";
	}
	//GPU线程
	else if (index < nGpu_)
	{
		buf << "\t\tqueue_" << index + 1 << "_0.finish();\n";
	}
	buf<<"\t\tawait();\n";
	buf<<"\t\n\t//pthread_barrier_wait (&barrier);\n\t}\n";
	buf<<"}\n";
}

void GPUCodeGenerate::CGglobalHeader()
{
	stringstream buf;
	string edgename;
	vector<FlatNode *>::iterator iter_1,iter_2;
	buf<<"#ifndef _GLOBAL_H\n";
	buf<<"#define _GLOBAL_H\n";
	buf<<"/*全局变量，用于存储边的信息*/\n";
	buf<<"/*边的命名规则：A_B,其中A->B*/\n\n";
	buf<<"#include \"Buffer.h\"\n";
	buf<<"#include \"CL/cl.hpp\"\n";
	buf<<"#include <string>\n";
	buf<<"#include <iostream>\n";
	buf<<"#include \"MathExtension.h\"\n";
	buf<<"using namespace std;\n";

	//cout<<flatNodes_.size();
	string notCommonEdgeStrName;
	vector<bool> vecEdgeDefine(flatNodes_.size(),0);
	vector<bool>::iterator iedgeIndex= vecEdgeDefine.begin();
	map<string,string>::iterator iterName;
	map<string,string> tmpVec;
	int stageminus; //stageminus表示两个actor所分配的阶段差
	//bool printStruct = false;
	
    //遍历所有compositeNode的streamType，找到流中所有包含的数据类型，作为结构体streamData中的数据
    map<string, string> typeSet;
    for (auto iter1 : *Program)
    {
        if (iter1->type == Composite)
        {
            compositeNode *comNode = static_cast<compositeNode *>(iter1);
            compHeadNode *head = comNode->head;
            ComInOutNode *inout = head->inout;
            if (inout != NULL)
            {
                list<Node *> *inputs = inout->input_List;
                list<Node *> *outputs = inout->output_List;
                if (inputs != NULL)
                {
                    for (auto it : *inputs)
                    {
                        auto nd = static_cast<inOutdeclNode *>(it)->strType;
                        string temp = static_cast<strdclNode *>(nd)->toString();
                        string type = temp.substr(0, temp.find(' '));
                        string data = temp.substr(temp.find(' '), temp.size() - 1);
                        typeSet.insert(make_pair(type, data));
                    }
                }
                if (outputs != NULL)
                {
                    for (auto it : *outputs)
                    {
                        auto nd = static_cast<inOutdeclNode *>(it)->strType;
                        string temp = static_cast<strdclNode *>(nd)->toString();
                        string type = temp.substr(0, temp.find(' '));
                        string data = temp.substr(temp.find(' '), temp.size() - 1);
                        typeSet.insert(make_pair(type, data));
                    }
                }
            }
        }
    }
	string typeName = "streamData";
	string typedefine = "";
	//写入数据流数据类型结构体
	buf << "struct streamData{\n";
    for (auto it : typeSet)
    {
		typedefine += it.first + " " +it.second +";\n";
		buf << "\t" << it.first << " " << it.second << ";\n";
    }
	buf << "};\n";
	allnameandtype.insert(make_pair(typeName, typedefine));


	//存储边的信息 结构体信息
	vector<FlatNode *>::iterator iter;
	for(iter = flatNodes_.begin(); iter != flatNodes_.end(); ++iter)//遍历所有结点
	{
		for(iter_2 = (*iter)->outFlatNodes.begin(); iter_2 != (*iter)->outFlatNodes.end(); iter_2++)
		{
			string edgename = (*iter)->name + "_" + (*iter_2)->name;
			//string edgetype = pEdgeInfo->getEdgeInfo((*iter), (*iter_2)).typeName;
			string edgetype = "streamData";
			EdgeNameToDefinetype.insert(make_pair(edgename, edgetype));
		}
	}

	for(iter_1=flatNodes_.begin();iter_1!=flatNodes_.end();++iter_1)//遍历所有结点
	{
		//什么标记位
		int flag = 0;
		int outflatnodeindex = 0;
		for(iter_2=(*iter_1)->outFlatNodes.begin();iter_2!=(*iter_1)->outFlatNodes.end();iter_2++)
		{
			if((*iter_1)->name == "LowPassFilter_24")
				int i = 1;
			if((*iter_2)->name == "Duplicate_2_1")
				int i = 1;

			string declaredEdgeStr;
			flag = 0;
			edgename=(*iter_1)->name+"_"+(*iter_2)->name;
			/*
			if(!printStruct)
			{
				buf<<"struct mystruct_x\n{\n";   //废弃
			}
			if(!flag&&(*iter_1)->contents->ot != Common_)
			{
				if(( iterName= mapOperator2EdgeName.find((*iter_1)->name))!=mapOperator2EdgeName.end())
				{
					flag = 1;
					declaredEdgeStr = iterName->second;
					if((*iter_2)->contents->ot != Common_)
					{
						mapOperator2EdgeName.insert(make_pair((*iter_2)->name,declaredEdgeStr));
						UpdateFlatNodeEdgeName(iter_2,declaredEdgeStr);
					}
				}
			}
			if(!flag&&(*iter_2)->contents->ot != Common_)
			{
				if(( iterName= mapOperator2EdgeName.find((*iter_2)->name))!=mapOperator2EdgeName.end())
				{
					flag = 1;
					declaredEdgeStr = iterName->second;
				}
			}
			if(flag)
			{
				//if((*iter_1)->GPUPart != (*iter_2)->GPUPart)
				//	buf<<"typedef "<<declaredEdgeStr<<"_str "<<edgename<<"_str;\n";
				vector<FlatNode *>::iterator iter_3 = iter_1;
				map<string,string>submap;
				submap.clear();
				submap.insert(make_pair(ReturnTypeOfEdgestr(declaredEdgeStr),ReturnNameOfEdgestr(declaredEdgeStr)));
				map<FlatNode *,FlatNode *>mapofflatnodes;
				mapofflatnodes.clear();
				mapofflatnodes.insert(make_pair(*iter_1,*iter_2));
				edge2nameandtype.insert(make_pair(mapofflatnodes,submap));
				while((*iter_3)->contents->ot != Common_)
				{
					map<string,string>::iterator strname= mapOperator2EdgeName.find((*iter_3)->name);
					if( strname !=mapOperator2EdgeName.end())
					{
						tmpVec.insert(make_pair(edgename,strname->second));
						//mapStruct2Struct.swap(tmpVec);
						//mapStruct2Struct.clear();
						break;
					}
					iter_3 = (*iter_3)->inFlatNodes.begin();
				}
			}
			else
			{
				Node *outputNode = NULL;
				List *outputList =(*iter_1)->contents->decl->u.decl.type->u.operdcl.outputs;
				ListMarker output_maker; 
				structflag = true;
				map<FlatNode *,FlatNode *>submapofstruct;
				submapofstruct.clear();
				submapofstruct.insert(make_pair(*iter_1,*iter_2));
				
				//buf<<"class "<<edgename<<"_str\n{\npublic:\n";
				StreamType *streamType = NULL;
				IterateList(&output_maker,outputList);
				while(NextOnList(&output_maker,(GenericREF)&outputNode))
				{
					if(outputNode->typ == Id) streamType = outputNode->u.id.decl->u.decl.type->u.strdcl.type;
					else if (outputNode->typ == Decl)  streamType = outputNode->u.decl.type->u.strdcl.type;
				}
				List *streamField = streamType->fields;
				ListMarker streamField_maker;
				IterateList(&streamField_maker,streamField);
				Node *fieldNode = NULL;
				while(NextOnList(&streamField_maker,(GenericREF)&fieldNode))
				{
					declList.str("");
					edgexy.str("");
					ExtractDeclVariables(fieldNode);
					string s = edgexy.str();
					mapedge2xy.insert(make_pair(edgename,s));
					mapedge2xy2.insert(make_pair(edgename,s));
					if(!printStruct)
						buf<<declList.str();
				}	
				edge2nameandtype.insert(make_pair(submapofstruct,nameandtypeofedge));
				edgestrnameandtype.insert(make_pair((*iter_1)->name+"_"+(*iter_2)->name,nameandtypeofedge));
				structflag = false;
				if((*iter_1)->contents->ot != Common_){
					mapOperator2EdgeName.insert(make_pair((*iter_1)->name,edgename));
					UpdateFlatNodeEdgeName(iter_1,edgename);
				}
				if((*iter_2)->contents->ot != Common_){
					mapOperator2EdgeName.insert(make_pair((*iter_2)->name,edgename));
					UpdateFlatNodeEdgeName(iter_2,edgename);
				}
			}
			if(!printStruct)
			{
				buf<<"};\n";
				printStruct = true;
			}*/
			
			stageminus = pSa->FindStage(*iter_2) - pSa->FindStage(*iter_1);
			if((*iter_1)->GPUPart == nGpu_ || (*iter_2)->GPUPart == nGpu_)
			{
				//如果不存在依赖关系，就使用自定义缓存
				//如果存在依赖关系，使用原本的Buffer
				if (!DataDependence(*iter_1,*iter_2))
				{
					if((*iter_1)->GPUPart != (*iter_2)->GPUPart && (*iter_1)->GPUPart == nGpu_)
						//buf << "extern " << EdgeNameToDefinetype[edgename] <<" " << edgename << "[" << stageminus << "][" << (sssg_->GetInitCount(*iter_1) + sssg_->GetSteadyCount(*iter_1)*Maflp->MultiNum2FlatNode[*iter_1])*(*iter_1)->outPushWeights[outflatnodeindex] << "];\n";
						buf << "extern " << EdgeNameToDefinetype[edgename] <<" " << edgename << "[" << stageminus << "][" << (sssg_->GetInitCount(*iter_1) + sssg_->GetSteadyCount(*iter_1)) *MultiNum*(*iter_1)->outPushWeights[outflatnodeindex] << "];\n";
					else if((*iter_1)->GPUPart != (*iter_2)->GPUPart && (*iter_2)->GPUPart == nGpu_)
						buf << "extern " << EdgeNameToDefinetype[edgename] <<" " << edgename << "[" << stageminus << "][" << (sssg_->GetInitCount(*iter_1) + sssg_->GetSteadyCount(*iter_1)*MultiNum)*(*iter_1)->outPushWeights[outflatnodeindex] << "];\n";
					else	
						buf << "extern " << EdgeNameToDefinetype[edgename] <<" " <<edgename<<"["<<stageminus+1<<"]["<<(sssg_->GetInitCount(*iter_1)+sssg_->GetSteadyCount(*iter_1)*MultiNum)*(*iter_1)->outPushWeights[outflatnodeindex]<<"];\n";
				}
				else
				{
					if((*iter_1)->GPUPart != (*iter_2)->GPUPart && (*iter_1)->GPUPart == nGpu_)
						buf << "extern Buffer<" << EdgeNameToDefinetype[edgename] << "> " << edgename << ";\n";
						//<<"("<<(sssg_->GetInitCount(*iter_1)+sssg_->GetSteadyCount(*iter_1)*Maflp->MultiNum2FlatNode[*iter_1]*stageminus)*(*iter_1)->outPushWeights[outflatnodeindex]<<");\n";
				    else if((*iter_1)->GPUPart != (*iter_2)->GPUPart && (*iter_2)->GPUPart == nGpu_)
						buf << "extern Buffer<" << EdgeNameToDefinetype[edgename] << "> " << edgename << ";\n";
						//<<"("<<(sssg_->GetInitCount(*iter_1)+sssg_->GetSteadyCount(*iter_1)*Maflp->MultiNum2FlatNode[*iter_1]*nGpu_*stageminus)*(*iter_1)->outPushWeights[outflatnodeindex]<<");\n";
				    else
						buf << "extern Buffer<" << EdgeNameToDefinetype[edgename] << "> " << edgename << ";\n";
						//<<"("<<(sssg_->GetInitCount(*iter_1)+sssg_->GetSteadyCount(*iter_1)*Maflp->MultiNum2FlatNode[*iter_1]*(stageminus+1))*(*iter_1)->outPushWeights[outflatnodeindex]<<");\n";
				}
			}
			outflatnodeindex++;
		}
	}
	/*multimap<string,string>::iterator iterxy=mapedge2xy2.begin();
	pair<multimap<string,string>::iterator,multimap<string,string>::iterator>pos = mapedge2xy2.equal_range(iterxy->first);*/
	map<string,string>::iterator iter_nametype;
	for (iter_nametype = allnameandtype.begin();iter_nametype != allnameandtype.end();++iter_nametype)
	{
		buf<<"typedef struct\n";
		buf<<"{\n";
		buf<<"\t"<<iter_nametype->second<<"\n";
		buf<<"}mystruct_"<<iter_nametype->first<<";\n";
	}
	buf<<"#endif\n";
	//输出到文件
	string filename = "global.h";
	ofstream out(filename);
	out << buf.str();
	buf.str("");
	//mapOperator2EdgeName.clear();
}
void GPUCodeGenerate::CGglobalCpp()
{
	stringstream buf;
	string edgename;
	int size;//缓冲区的大小
	vector<FlatNode *>::iterator iter_1,iter_2;
	vector<int>::iterator iter;
	int stageminus; //stageminus表示两个actor所分配的阶段差
	buf<<"/*cpp文件,全局变量，用于存储边的信息*/\n";
	buf<<"#include \"Buffer.h\"\n";
	buf<<"#include \"global.h\"\n";
	for (iter_1=flatNodes_.begin();iter_1!=flatNodes_.end();++iter_1)//遍历所有结点
	{
		iter=(*iter_1)->outPushWeights.begin();
		for (iter_2=(*iter_1)->outFlatNodes.begin();iter_2!=(*iter_1)->outFlatNodes.end();iter_2++)
		{
			edgename=(*iter_1)->name+"_"+(*iter_2)->name;
			stageminus=pSa->FindStage(*iter_2)-pSa->FindStage(*iter_1);
			if ((*iter_1)->GPUPart == nGpu_ || (*iter_2)->GPUPart == nGpu_)
			{
				if (!DataDependence(*iter_1,*iter_2))
				{
					if((*iter_1)->GPUPart != (*iter_2)->GPUPart && (*iter_1)->GPUPart == nGpu_)
						buf << EdgeNameToDefinetype[edgename] << " "<< edgename << "[" << stageminus << "][" << (sssg_->GetInitCount(*iter_1) + sssg_->GetSteadyCount(*iter_1)*Maflp->MultiNum2FlatNode[*iter_1])*(*iter) << "];\n";
					else if((*iter_1)->GPUPart != (*iter_2)->GPUPart && (*iter_2)->GPUPart == nGpu_)
						buf << EdgeNameToDefinetype[edgename] << " " << edgename << "[" << stageminus << "][" << (sssg_->GetInitCount(*iter_1) + sssg_->GetSteadyCount(*iter_1)*Maflp->MultiNum2FlatNode[*iter_1] * nGpu_)*(*iter) << "];\n";
					else	
						buf << EdgeNameToDefinetype[edgename] << " " << edgename << "[" << stageminus + 1 << "][" << (sssg_->GetInitCount(*iter_1) + sssg_->GetSteadyCount(*iter_1)*Maflp->MultiNum2FlatNode[*iter_1])*(*iter) << "];\n";
				}
				else
				{
					if((*iter_1)->GPUPart != (*iter_2)->GPUPart && (*iter_1)->GPUPart == nGpu_)
						buf << "Buffer<" << EdgeNameToDefinetype[edgename] << "> " << edgename << "(" << (sssg_->GetInitCount(*iter_1) + sssg_->GetSteadyCount(*iter_1)*Maflp->MultiNum2FlatNode[*iter_1] * stageminus)*(*iter) << ");\n";
					else if((*iter_1)->GPUPart != (*iter_2)->GPUPart && (*iter_2)->GPUPart == nGpu_)
						buf << "Buffer<" << EdgeNameToDefinetype[edgename] << "> " << edgename << "(" << (sssg_->GetInitCount(*iter_1) + sssg_->GetSteadyCount(*iter_1)*Maflp->MultiNum2FlatNode[*iter_1] * nGpu_*stageminus)*(*iter) << ");\n";
					else
						buf << "Buffer<" << EdgeNameToDefinetype[edgename] << "> " << edgename << "(" << (sssg_->GetInitCount(*iter_1) + sssg_->GetSteadyCount(*iter_1)*Maflp->MultiNum2FlatNode[*iter_1] * (stageminus + 1))*(*iter) << ");\n";
				}
				iter++;
			}
		}
	}

	//输出到文件
	string filename = "global.cpp";
	ofstream out(filename);
	out << buf.str();
}
void GPUCodeGenerate::CGMain()
{
	stringstream buf,ss;
	char a[10];
	buf<<"///当前的扩大因子是"<<MultiNum<<"\n";
	buf<<"#include \"iostream\"\n";
	buf<<"#include \"stdlib.h\"\n";
	buf<<"#include <fstream>\n";

	buf<<"#include <pthread.h>\n";
	buf<<"#include \"setCpu.h\"\n";

	buf<<"#include \"global.h\"\n";
	buf<<"#include \"barrier_sync.h\"\t//包含barrier函数\n";
	buf<<"#include \"AllActorHeader.h\"\t//包含所有actor的头文件\n";
	buf<<"using namespace std;\n";//home/chenwenbin/Desktop/GPU201310/FFT5/barrier_sync.cpp
	buf<<"int com_num = "<<Comm_num<<";\n"; //cwb
	
	//buf<<"int ThreadNum="<<2*nGpu_+NCpuThreads<<";\n";
	buf << "int ThreadNum=" << 2 * nGpu_ + CPUthread << ";\n";
	buf<<"int MAX_ITER=1;//默认的执行次数是1\n";

	if (PrintTime)  //cwb 统计时间
	{
		buf<<"double cpu0_compute = 0;\n";
		if (CPUthread > 1)
		{
			for (int i = 0; i < CPUthread - 1; i++)
				buf<<"double cpu"<<i+1<<"_compute = 0;\n";
		}
		for (int i = 0; i < nGpu_; i++)
		{
			buf<<"double cpu_gpu_com"<<i<<" = 0;\n";
			buf<<"double gpu"<<i<<"_compute = 0;\n";
		}
	}
	/************************************************************************/
	/*                     OpenCL环境初始化代码                             */
	/************************************************************************/
	buf<<"vector<cl::CommandQueue>allqueue;\n";
	buf<<"vector<cl::Platform>platforms;\n";
	buf<<"vector<cl::Device>platformsDevices;\n";
	buf<<"vector<cl::Event>dataevents;\n";
	buf<<"cl_int err_0 = cl::Platform::get(&platforms);\n";
	buf<<"cl_int err_1 = platforms[0].getDevices(CL_DEVICE_TYPE_GPU,&platformsDevices);\n";
	buf<<"cl::Context context(platformsDevices);\n";
	buf<<"std::ifstream programFile(\"allkernel.cl\");\n";
	buf<<"string programString(istreambuf_iterator<char>(programFile),(istreambuf_iterator<char>()));\n";
	buf<<"cl::Program::Sources source_opencl(1,make_pair(programString.c_str(),programString.length()+1));\n";
	buf<<"cl::Program program(context,source_opencl);\n";
	buf<<"cl_int err_2 = program.build(platformsDevices);\n";
	for (int i = 0;i < nGpu_;i++)
	{
		buf<<"cl::CommandQueue queue_"<<i+1<<"_0(context,platformsDevices["<<i<<"]);\n";
		buf<<"cl::CommandQueue queue_"<<i+1<<"_1(context,platformsDevices["<<i<<"]);\n";
	}
	/*buf<<"cl::CommandQueue queue_1(context,platformsDevices[0]);\n";
	buf<<"cl::CommandQueue queue_2(context,platformsDevices[1]);\n";
	buf<<"cl::CommandQueue queue_3(context,platformsDevices[2]);\n";*/
	vector<FlatNode*>::iterator iter1,iter2;
	vector<int>::iterator iter;
	int stageminus,size;
	map<FlatNode *,int>::iterator iterofqueueid,iterofqueueid1;
	for (int i = 0; i < nGpu_; i++)
	{
		for (iter1 = flatNodes_.begin();iter1!=flatNodes_.end();iter1++)
		{
			iter=(*iter1)->outPushWeights.begin();
			for (iter2=(*iter1)->outFlatNodes.begin();iter2!=(*iter1)->outFlatNodes.end();iter2++)
			{
				stageminus=pSa->FindStage(*iter2)-pSa->FindStage(*iter1);
				//size=(sssg_->GetInitCount(*iter1)+sssg_->GetSteadyCount(*iter1)*Maflp->MultiNum2FlatNode[*iter1]*(stageminus+1))*(*iter);
				if (!IsMod)
				{
					size = ReturnNumBiger(size);
				}
				//if (DetectiveFilterState(*iter1)&&!DetectiveFilterState(*iter2))
				if ((*iter1)->GPUPart == nGpu_ && (*iter2)->GPUPart != nGpu_)
				{
					size=(sssg_->GetInitCount(*iter1)+sssg_->GetSteadyCount(*iter1)*Maflp->MultiNum2FlatNode[*iter1]/nGpu_*stageminus)*(*iter);
					int pos = (*iter2)->name.find('_');
					string substring = (*iter2)->name.substr(pos);
					iterofqueueid = Node2QueueID.find(*iter2);
					
					//陈名韬 2019 修改 sssg_->GetInitCount(*iter1)*(*iter) 如果等于0，在GPU上创建buffer会失败,但是相应的也不会使用这个buffer
					int initbuggsize = sssg_->GetInitCount(*iter1)*(*iter);
					if (initbuggsize == 0)
						initbuggsize = 1;

					buf<<"cl::Buffer Inbuffer_"<<(*iter1)->name<<"_"<<(*iter2)->name<<"_"<<i<<"(context,CL_MEM_READ_ONLY ,"<<size<<"*sizeof(mystruct_"<<ReturnTypeOfEdge(*iter1,*iter2)<<"),NULL,NULL);\n";
					buf << "cl::Buffer cmPinnedBufIn_" << (*iter1)->name << "_" << (*iter2)->name << "_init_" << i << "(context,CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR," << initbuggsize << "*sizeof(mystruct_" << ReturnTypeOfEdge(*iter1, *iter2) << "),NULL,NULL);\n";
					buf << "cl::Buffer cmDevBufIn_" << (*iter1)->name << "_" << (*iter2)->name << "_init_" << i << "(context,CL_MEM_READ_ONLY," << initbuggsize << "*sizeof(mystruct_" << ReturnTypeOfEdge(*iter1, *iter2) << "),NULL,NULL);\n";
					buf<<"mystruct_"<<ReturnTypeOfEdge(*iter1,*iter2)<<" *Inst_"<<(*iter1)->name<<"_"<<(*iter2)->name<<"_init_"<<i<<" = (mystruct_"<<ReturnTypeOfEdge(*iter1,*iter2)<<"*)queue_"<<iterofqueueid->second<<"_0.enqueueMapBuffer(cmPinnedBufIn_"<<(*iter1)->name<<"_"<<(*iter2)->name<<"_init_"<<i<<",CL_TRUE, CL_MAP_WRITE,0,"<<sssg_->GetInitCount(*iter1)*(*iter)<<"*sizeof(mystruct_"<<ReturnTypeOfEdge(*iter1,*iter2)<<"),NULL,NULL,NULL);\n";
					buf<<"cl::Buffer cmPinnedBufIn_"<<(*iter1)->name<<"_"<<(*iter2)->name<<"_steady_"<<i<<"(context,CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR,"<<Maflp->MultiNum2FlatNode[*iter2]*sssg_->GetSteadyCount(*iter1)*(*iter)<<"*sizeof(mystruct_"<<ReturnTypeOfEdge(*iter1,*iter2)<<"),NULL,NULL);\n";
					buf<<"cl::Buffer cmDevBufIn_"<<(*iter1)->name<<"_"<<(*iter2)->name<<"_steady_"<<i<<"(context,CL_MEM_READ_ONLY,"<<Maflp->MultiNum2FlatNode[*iter2]*sssg_->GetSteadyCount(*iter1)*(*iter)<<"*sizeof(mystruct_"<<ReturnTypeOfEdge(*iter1,*iter2)<<"),NULL,NULL);\n";
					buf<<"mystruct_"<<ReturnTypeOfEdge(*iter1,*iter2)<<" *Inst_"<<(*iter1)->name<<"_"<<(*iter2)->name<<"_steady_"<<i<<" = (mystruct_"<<ReturnTypeOfEdge(*iter1,*iter2)<<"*)queue_"<<iterofqueueid->second<<"_0.enqueueMapBuffer(cmPinnedBufIn_"<<(*iter1)->name<<"_"<<(*iter2)->name<<"_steady_"<<i<<",CL_TRUE, CL_MAP_WRITE,0,"<<Maflp->MultiNum2FlatNode[*iter1]*sssg_->GetSteadyCount(*iter1)*(*iter)<<"*sizeof(mystruct_"<<ReturnTypeOfEdge(*iter1,*iter2)<<"),NULL,NULL,NULL);\n";
				}
				
				if((*iter1)->GPUPart != nGpu_ && (*iter2)->GPUPart != nGpu_)
				{
					size=(sssg_->GetInitCount(*iter1)+sssg_->GetSteadyCount(*iter1)*Maflp->MultiNum2FlatNode[*iter1]*(stageminus+1))*(*iter);
					int pos = (*iter1)->name.find('_');
					string substring = (*iter1)->name.substr(pos);
					buf<<"cl::Buffer Outbuffer_"<<(*iter1)->name<<"_"<<(*iter2)->name<<"_"<<i<<"(context,CL_MEM_WRITE_ONLY ,"<<size<<"*sizeof(mystruct_"<<ReturnTypeOfEdge(*iter1,*iter2)<<"),NULL,NULL);\n";
				}
				//*iter1 是GPU节点   *iter2 是cpu节点
				if((*iter1)->GPUPart != nGpu_ && (*iter2)->GPUPart == nGpu_)
				{
					size=(sssg_->GetInitCount(*iter1)+sssg_->GetSteadyCount(*iter1)*Maflp->MultiNum2FlatNode[*iter1]*stageminus)*(*iter);
					int pos = (*iter1)->name.find('_');
					string substring = (*iter1)->name.substr(pos);
					iterofqueueid = Node2QueueID.find(*iter1);

					//陈名韬 2019 修改 sssg_->GetInitCount(*iter1)*(*iter) 如果等于0，在GPU上创建buffer会失败,但是相应的也不会使用这个buffer
					int initbuggsize = sssg_->GetInitCount(*iter1)*(*iter);
					if (initbuggsize == 0)
						initbuggsize = 1;

					buf<<"cl::Buffer Outbuffer_"<<(*iter1)->name<<"_"<<(*iter2)->name<<"_"<<i<<"(context,CL_MEM_WRITE_ONLY ,"<<size<<"*sizeof(mystruct_"<<ReturnTypeOfEdge(*iter1,*iter2)<<"),NULL,NULL);\n";
					buf << "cl::Buffer cmPinnedBufOut_" << (*iter1)->name << "_" << (*iter2)->name << "_init_" << i << "(context,CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR," << initbuggsize << "*sizeof(mystruct_" << ReturnTypeOfEdge(*iter1, *iter2) << "),NULL,NULL);\n";
					buf << "cl::Buffer cmDevBufOut_" << (*iter1)->name << "_" << (*iter2)->name << "_init_" << i << "(context,CL_MEM_WRITE_ONLY," << initbuggsize << "*sizeof(mystruct_" << ReturnTypeOfEdge(*iter1, *iter2) << "),NULL,NULL);\n";
					buf<<"mystruct_"<<ReturnTypeOfEdge(*iter1,*iter2)<<" *Outst_"<<(*iter1)->name<<"_"<<(*iter2)->name<<"_init_"<<i<<" = (mystruct_"<<ReturnTypeOfEdge(*iter1,*iter2)<<"*)queue_"<<iterofqueueid->second<<"_0.enqueueMapBuffer(cmPinnedBufOut_"<<(*iter1)->name<<"_"<<(*iter2)->name<<"_init_"<<i<<",CL_TRUE, CL_MAP_READ,0,"<<sssg_->GetInitCount(*iter1)*(*iter)<<"*sizeof(mystruct_"<<ReturnTypeOfEdge(*iter1,*iter2)<<"),NULL,NULL,NULL);\n";
					buf<<"cl::Buffer cmPinnedBufOut_"<<(*iter1)->name<<"_"<<(*iter2)->name<<"_steady_"<<i<<"(context,CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,"<<Maflp->MultiNum2FlatNode[*iter1]*sssg_->GetSteadyCount(*iter1)*(*iter)<<"*sizeof(mystruct_"<<ReturnTypeOfEdge(*iter1,*iter2)<<"),NULL,NULL);\n";
					buf<<"cl::Buffer cmDevBufOut_"<<(*iter1)->name<<"_"<<(*iter2)->name<<"_steady_"<<i<<"(context,CL_MEM_WRITE_ONLY,"<<Maflp->MultiNum2FlatNode[*iter1]*sssg_->GetSteadyCount(*iter1)*(*iter)<<"*sizeof(mystruct_"<<ReturnTypeOfEdge(*iter1,*iter2)<<"),NULL,NULL);\n";
					buf<<"mystruct_"<<ReturnTypeOfEdge(*iter1,*iter2)<<" *Outst_"<<(*iter1)->name<<"_"<<(*iter2)->name<<"_steady_"<<i<<" = (mystruct_"<<ReturnTypeOfEdge(*iter1,*iter2)<<"*)queue_"<<iterofqueueid->second<<"_0.enqueueMapBuffer(cmPinnedBufOut_"<<(*iter1)->name<<"_"<<(*iter2)->name<<"_steady_"<<i<<",CL_TRUE, CL_MAP_READ,0,"<<Maflp->MultiNum2FlatNode[*iter1]*sssg_->GetSteadyCount(*iter1)*(*iter)<<"*sizeof(mystruct_"<<ReturnTypeOfEdge(*iter1,*iter2)<<"),NULL,NULL,NULL);\n";
				}

				iter++;
			}
		}
	}
	for (iter1=flatNodes_.begin();iter1!=flatNodes_.end();++iter1)
	{
		buf<<(*iter1)->name<<" "<<(*iter1)->name<<"_obj(";
		int index = 0;
		for (; index < (*iter1)->inFlatNodes.size(); ++index)
		{
			if (((IsUpBorder(*iter1)&&IsDownBorder((*iter1)->inFlatNodes[index])) || ((*iter1)->GPUPart == nGpu_&&(*iter1)->inFlatNodes[index]->GPUPart == nGpu_)) && DataDependence((*iter1)->inFlatNodes[index],*iter1))
			{
				buf<<(*iter1)->inFlatNodes[index]->name<<"_"<<(*iter1)->name<<",";
			}
		}
		for (index = 0; index < (*iter1)->outFlatNodes.size(); ++index)
		{
			if(((IsDownBorder(*iter1)&&IsUpBorder((*iter1)->outFlatNodes[index])) || ((*iter1)->GPUPart==nGpu_&&(*iter1)->outFlatNodes[index]->GPUPart==nGpu_)) && DataDependence(*iter1,(*iter1)->outFlatNodes[index]))
				buf<<(*iter1)->name<<"_"<<(*iter1)->outFlatNodes[index]->name<<",";
		}
		buf<<"NULL);\n";
	}
	for (int i = 0; i<nGpu_ * 2 + CPUthread; i++)
	{
		string numname=to_string(i);
		buf<<"extern void thread_"<<numname<<"_fun();\n";
	}
	for (int i = 1; i<nGpu_ * 2 + CPUthread; i++)
	{
		buf<<"void* thread_"<<i<<"_fun_start(void *)\n{\n\tset_cpu("<<i<<");\n\tthread_"<<i<<"_fun();\n\treturn 0;\n}\n";
	}
	buf<<"int main(int argc,char **argv)\n{\n";
	buf<<"\tofstream resultfw;\n";
	buf<<"\tint oc;\n";
	buf<<"\tchar *b_opt_arg;\n";
	buf<<"\twhile((oc=getopt(argc,argv,\"i:\"))!=-1)\n";
	buf<<"\t{\n";
	buf<<"\t\tswitch(oc)\n";
	buf<<"\t\t{\n";
	buf<<"\t\t\tcase 'i':\n";
	buf<<"\t\t\t\tMAX_ITER=atoi(optarg);\n";
	buf<<"\t\t\t\tbreak;\n";
	buf<<"\t\t}\n";
	buf<<"\t}\n";
	buf<<"\tcpu_set_t mask;\n\tset_cpu(0);\n";

	//for linux thread
	//buf<<"\tpthread_barrier_init (&barrier, NULL, "<<nGpu_<<");\n";
	buf << "\tpthread_t tid[" << nGpu_ * 2 + CPUthread - 1 << "];\n";
	for (int i = 1; i<nGpu_ * 2 + CPUthread; i++)
	{
		buf<<"\tpthread_create (&tid["<<i-1<<"], NULL, thread_"<<i<<"_fun_start, (void*)NULL);\n";
	}
	buf<<"\tthread_0_fun();\n";
	if (PrintTime)
	{
		if (CPUthread > 0)
			for (int i = 0; i < CPUthread; i++)
				buf<<"\tcout<<\"cpu"<<i<<"_compute:  \"<<cpu"<<i<<"_compute<<endl;\n";
		for (int i = 0; i < nGpu_; i++)
		{
			buf<<"\tcout<<\"cpu_gpu_com"<<i<<":  \"<<cpu_gpu_com"<<i<<"<<endl;\n";
		}
		for(int i = 0; i < nGpu_; i++)
			buf<<"\tcout<<\"gpu"<<i<<"_compute: \"<<gpu"<<i<<"_compute<<endl;\n";
	}
	buf<<"\treturn 0;\n";
	buf<<"\n}";

	string filename = "main.cpp";
	ofstream out(filename);
	out << buf.str();

}
void GPUCodeGenerate::CGactors()
{
	stringstream ss,buf;
	std::map<operatorNode *, string>::iterator pos;

	for (int i = 0; i < nActors_; ++i)//生成 各个 类模板  2019 好像并没有生成
	{
		buf<<"#include \""<<flatNodes_[i]->name<<".h\"\n";
		CGactor(flatNodes_[i]);

	}
	string filename = "AllActorHeader.h";
	ofstream out(filename);
	out << buf.str();
	buf.str("");
}

void GPUCodeGenerate::CGactor(FlatNode *actor)
{
	stringstream buf;
	vector<string>::iterator iter;
	map<string,string>tempprt;
	vector<string>::iterator iter_ptrtype,iter_ptrname;
	string classNmae = actor->name, streamName = "Token_Temp";
	curactor=actor;
	assert(actor);
	buf <<"/**\n * Class "<<classNmae<<"\n */\n";
	buf<<"#include \"Buffer.h\"\n";
	buf<<"#include \"Consumer.h\"\n";
	buf<<"#include \"Producer.h\"\n";
	buf<<"#include \"global.h\"\n";
	//buf<<"#include \"math.h\"\n";
	buf<<"#include \"iostream\"\n";
	buf<<"#include \"GlobalVar.h\"\n";
	buf<<"using namespace std;\n";
	if(actor->GPUPart != nGpu_)//cwb   
	{
		CGexternbuffer(actor,buf);
	}
	//
	vector<string> inEdgeName;
	vector<string> outEdgeName;
	auto oper = actor->contents;
	operBodyNode *body = oper->operBody;
	list<Node *> stmts = body->stmt_list;
	Node *init = body->init;
	Node *work = body->work;
	list<Node *> *inputs = oper->inputs;
	list<Node *> *outputs = oper->outputs;
	if (inputs != NULL)
	{
		for (auto it : *inputs)
			inEdgeName.push_back(((idNode *)it)->name);
	}
	if (outputs != NULL)
	{
		for (auto it : *outputs)
			outEdgeName.push_back(((idNode *)it)->name);
	}
	buf <<"class "<<classNmae<<"{\n"; // 类块开始
	parameterBuf.str(""); // 清空parameterBuf内容
	thisBuf.str(""); // 清空thisBuf内容
	buf<<"private:\n\t";
	//写入读写标志位
	CGdatatag(actor,buf);
	//写入边的私有变量
	CGEdgeParam(actor,buf,inEdgeName,outEdgeName);
	// 写入语法树成员变量信息（param, var, state）
	CGactorsStmts(buf, &stmts);
	// 写入 var, state 的初始化信息
	CGactorsinitVarAndState(buf, &stmts);
    /* composite 中init函数 */
    CGactorsInit(buf, init);
	if(actor->GPUPart == nGpu_)
	{
		CGpopToken(actor,buf,inEdgeName);
		CGpushToken(actor,buf,outEdgeName);
	}
	else//如果是GPU上的节点
	{
		CGpopTokenForGPU(actor,buf,inEdgeName);
		CGpushTokenForGPU(actor,buf,outEdgeName);
	}
	buf<<"public:\n";
	// 写入构造函数信息
	buf<<"\tint steadyScheduleCount;\t//稳态时一次迭代的执行次数\n";
	buf<<"\tint initScheduleCount;\n";
	CGthis(actor,  buf,inEdgeName,outEdgeName);
	// 写入成员函数信息
	buf <<"\t// ------------Member Mehods------------\n";
	// 写入initWork函数
	CGinitWork(buf);
	// 写入work函数,
	//重写
	CGwork(actor, buf,inEdgeName,outEdgeName,work);
	//为分配在GPU上的actor写入数据传输函数(输入到GPU缓冲区中)
	if(actor->GPUPart != nGpu_)
	{
		bool inactorofcpu = false;
		vector<FlatNode*>::iterator iter;
		for (iter=actor->inFlatNodes.begin();iter!=actor->inFlatNodes.end();iter++)
		{
			//if ((!DetectiveFilterState(actor)&&(DetectiveFilterState(*iter)||Maflp->UporDownStatelessNode(actor)==3))||((!DetectiveFilterState(actor)&&!DetectiveFilterState(*iter)&&(Maflp->UporDownStatelessNode(*iter)!=3)&&Maflp->findPartitionNumForFlatNode(actor)!=Maflp->findPartitionNumForFlatNode(*iter))))
			if((actor->GPUPart != nGpu_ && (*iter)->GPUPart == nGpu_) || (actor->GPUPart != nGpu_ && (*iter)->GPUPart != nGpu_ && Maflp->findPartitionNumForFlatNode(actor)!=Maflp->findPartitionNumForFlatNode(*iter)))
			{
				inactorofcpu = true;
			}
		}		
		if (inactorofcpu)
		{
			CGdataGetforGPU(actor,buf,inEdgeName);
		}
	}
	//为分配在GPU上的actor写入数据传输函数(输出到CPU缓冲区中)
	//if (!DetectiveFilterState(actor)&&(Maflp->UporDownStatelessNode(actor)!=3))//cwb
	if(actor->GPUPart != nGpu_)
	{
		bool outactorofcpu = false;
		vector<FlatNode*>::iterator iter;
		for (iter=actor->outFlatNodes.begin();iter!=actor->outFlatNodes.end();iter++)
		{
			//if ((!DetectiveFilterState(actor)&&(DetectiveFilterState(*iter)||Maflp->UporDownStatelessNode(actor)==3))||(!DetectiveFilterState(actor)&&!DetectiveFilterState(*iter)&&(Maflp->UporDownStatelessNode(*iter)!=3)&&Maflp->findPartitionNumForFlatNode(actor)!=Maflp->findPartitionNumForFlatNode(*iter)))
			if((actor->GPUPart != nGpu_ && (*iter)->GPUPart == nGpu_) || (actor->GPUPart != nGpu_ && (*iter)->GPUPart != nGpu_ && Maflp->findPartitionNumForFlatNode(actor)!=Maflp->findPartitionNumForFlatNode(*iter)))
			{
				outactorofcpu = true;
			}
		}		
		if (outactorofcpu)
		{
			CGdataSendforGPU(actor,buf,outEdgeName);
		}
	}
	//打印输出结果cwb

	if (actor->outFlatNodes.size() ==0 && PrintFlag && actor->GPUPart != nGpu_)
	{
		for (int i = 0; i < nGpu_; i++)
		{
			buf<<"\tvoid print_"<<i<<"(int count)\n\t{\n";
			int queuenum = i+1;
			vector<FlatNode*>::iterator iter = actor->inFlatNodes.begin();
			vector<int>::iterator iter_pop = actor->inPopWeights.begin();
			buf<<"\t\tmystruct_"<<ReturnTypeOfEdge(*iter,actor)<<" *Outst = new mystruct_"<<ReturnTypeOfEdge(*iter,actor)<<"[count*"<<*iter_pop<<"];\n";
			buf<<"\t\tqueue_"<<queuenum<<"_1.enqueueReadBuffer(Outbuffer_"<<(*iter)->name<<"_"<<actor->name<<"_"<<queuenum-1<<",CL_TRUE,0,count*"<<*iter_pop<<"*sizeof(mystruct_"<<ReturnTypeOfEdge(*iter,actor)<<"),Outst,NULL,NULL);\n";
			buf<<"\t\t\tfor(int i = 0;i < count*"<<*iter_pop<<";i++)\n";
			buf<<"\t\t\t\tcout<<Outst[i].x<<endl;\n";
			buf<<"\t\tdelete [] Outst;\n\t}\n";
		}
	}
	if(actor->GPUPart == nGpu_)
	{
		CGrunInitScheduleWork(actor,buf,inEdgeName,outEdgeName);
		CGrunSteadyScheduleWork(actor,buf,inEdgeName,outEdgeName);
	}
	else
	{
		CGrunInitScheduleWorkforGPU(actor,buf);
		CGrunSteadyScheduleWorkforGPU(actor,buf);
	}
	buf <<"};";//类块结束
	//输出到文件
	string filename = classNmae+".h";
	ofstream out(filename);
	out << buf.str();
	buf.str("");
}

void GPUCodeGenerate::CGwork(FlatNode *actor,stringstream &buf,vector<string> inEdgeName,vector<string> outEdgeName,Node *work)
{
	//if (DetectiveFilterState(actor)||(Maflp->UporDownStatelessNode(actor)==3))
	if (actor->GPUPart == nGpu_) //生成CPUactor
	{
		buf << "\t// work\n";
		buf << "\tvoid work(";
		vector<string>::iterator iter;
		multimap<FlatNode*,string>::iterator iter1,iter2;
		stringstream tmpBuf;
		string actorname = actor->name;
		string inputString;
		string outputString;
		for(int index = 0;index < actor->inFlatNodes.size();index++)
		{
			inputString = inEdgeName[index];
			if (!DataDependence(actor->inFlatNodes[index], actor))
			{
				string edgetype = ReturnTypeOfEdge(actor->inFlatNodes[index], actor);
				buf << edgetype << " *" << inputString << ",";
			}
		}
		for(int index = 0;index < actor->outFlatNodes.size();index++)
		{
			outputString = outEdgeName[index];
			if(!DataDependence(actor, actor->outFlatNodes[index]))
			{
				string edgetype = ReturnTypeOfEdge(actor, actor->outFlatNodes[index]);
				buf << edgetype << " *" << outputString << ",";
			}
		}
		buf<<"void* p = NULL){\n\t";
    	if (work != NULL)
        	buf << work->toString();
		
		if(ExistDependence(actor))
			buf<<"\n\tpopToken();\n";
		bool flag = false;
		for (int index = 0; index < actor->outFlatNodes.size(); ++index)
		{
			if(ExistDependence(actor->outFlatNodes[index]))
			{
				flag = true;
				break;
			}
		}
		if(flag)
			buf<<"\tpushToken();\n";
		buf<<"\t}\n";
	}
	else// 生成GPU actor
	{
		for (int i = 0; i < nGpu_; i++)
		{
			staticvariablefinal = staticvariable;
			staticvartypefinal = staticvartype;
			buf << "\t// work\n";
			buf << "\tvoid work_"<<i<<"(int count) \n\t{\n";
			string num;
			int pos = actor->name.find('_');
			num = actor->name.substr(pos+1);
			int argnum = 0;//kernel函数的参数个数
			buf<<"\t\tcl::Kernel kernel(program,\"kernel"<<actor->name<<"\");\n";
			vector<FlatNode*>::iterator iter,iter_1;
			map<FlatNode*,vector<string>>::iterator iterofOut,iterofIn;
			vector<string>curkerpar,othkerpar,tempstring;
			vector<string>::iterator cur_iter,oth_iter;
			for (iter=actor->inFlatNodes.begin();iter!=actor->inFlatNodes.end();iter++)
			{
				if((actor->GPUPart != nGpu_ && (*iter)->GPUPart == nGpu_) || (actor->GPUPart != nGpu_ && (*iter)->GPUPart != nGpu_ && Maflp->findPartitionNumForFlatNode(actor)!=Maflp->findPartitionNumForFlatNode(*iter)))
				{
					buf<<"\t\tkernel.setArg("<<argnum++<<",Inbuffer_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<");\n";
					buf<<"\t\tkernel.setArg("<<argnum++<<",readtag_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<");\n";
				}
				if(actor->GPUPart != nGpu_ && (*iter)->GPUPart != nGpu_ && Maflp->findPartitionNumForFlatNode(actor)==Maflp->findPartitionNumForFlatNode(*iter))
				{
					buf<<"\t\tkernel.setArg("<<argnum++<<","<<"Outbuffer_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<");\n";
					buf<<"\t\tkernel.setArg("<<argnum++<<",readtag_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<");\n";
				}
			}
			for (iter_1=actor->outFlatNodes.begin();iter_1!=actor->outFlatNodes.end();iter_1++)
			{	
				if((actor->GPUPart != nGpu_ && (*iter_1)->GPUPart == nGpu_) || (actor->GPUPart != nGpu_ && (*iter_1)->GPUPart != nGpu_ && Maflp->findPartitionNumForFlatNode(actor)!=Maflp->findPartitionNumForFlatNode(*iter_1)))
				{
					buf<<"\t\tkernel.setArg("<<argnum++<<",Outbuffer_"<<actor->name<<"_"<<(*iter_1)->name<<"_"<<i<<");\n";
					buf<<"\t\tkernel.setArg("<<argnum++<<",writetag_"<<actor->name<<"_"<<(*iter_1)->name<<"_"<<i<<");\n";
				}
				tempstring.clear();
				if(actor->GPUPart != nGpu_ && (*iter_1)->GPUPart != nGpu_ && Maflp->findPartitionNumForFlatNode(actor)==Maflp->findPartitionNumForFlatNode(*iter_1))
				{
					buf<<"\t\tkernel.setArg("<<argnum++<<",Outbuffer_"<<actor->name<<"_"<<(*iter_1)->name<<"_"<<i<<");\n";
					buf<<"\t\tkernel.setArg("<<argnum++<<",writetag_"<<actor->name<<"_"<<(*iter_1)->name<<"_"<<i<<");\n";
					tempstring.push_back("Outbuffer_"+(*iter_1)->name);
				}
				OutbufferOfActor.insert(make_pair(actor,tempstring));
			}
			map<FlatNode *,vector<string>>::iterator iter_param;
			vector<string>paramofactor;
			vector<string>::iterator iter_paofac;
			iter_param = Actor2KernelPar.find(actor);
			paramofactor = iter_param->second;
			vector<string>::iterator iter_prname;
			//////////////////////////////////////////////////////////////////////////
			//此处需要将数组封装成为内存对象
			for (iter_paofac = paramofactor.begin();iter_paofac != paramofactor.end();iter_paofac++)
			{
				buf<<"\t\tkernel.setArg("<<argnum++<<","<<(*iter_paofac)<<");\n";
			}
			vector<string>::iterator iter_ptrtype = ptrtype.begin();
			multimap<string,string>::iterator iter_ptrdim = ptrdim.begin();
			for (iter_prname = ptrname.begin();iter_prname != ptrname.end();++iter_prname)
			{
				if (iter_ptrdim->second == "0")
				{
					buf<<"\t\tcl::Buffer "<<(*iter_prname)<<"_buffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,"<<(iter_ptrdim->first)<<"*sizeof("<<(*iter_ptrtype)<<"),"<<(*iter_prname)<<",NULL);\n";
					buf<<"\t\tkernel.setArg("<<argnum++<<","<<(*iter_prname)<<"_buffer);\n";
				}
				else
				{
					//////////////////////////////////////////////////////////////////////////
					//如果是二维数组则需要转化为一维数组
					buf<<"\t\t"<<(*iter_ptrtype)<<" "<<(*iter_prname)<<"_new["<<(iter_ptrdim->first)<<"*"<<(iter_ptrdim->second)<<"];\n";
					buf<<"\t\tfor(int i = 0;i < "<<iter_ptrdim->first<<";i++)\n";
					buf<<"\t\t{\n";
					buf<<"\t\t\tfor(int j = 0;j < "<<iter_ptrdim->second<<";j++)\n";
					buf<<"\t\t\t{\n";
					buf<<"\t\t\t\t"<<(*iter_prname)<<"_new[i*"<<iter_ptrdim->second<<"+j] = "<<(*iter_prname)<<"[i][j];\n";
					buf<<"\t\t\t}\n";
					buf<<"\t\t}\n";
					buf<<"\t\tcl::Buffer "<<(*iter_prname)<<"_buffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,"<<(iter_ptrdim->first)<<"*"<<iter_ptrdim->second<<"*sizeof("<<(*iter_ptrtype)<<"),"<<(*iter_prname)<<"_new,NULL);\n";
					buf<<"\t\tkernel.setArg("<<argnum++<<","<<(*iter_prname)<<"_buffer);\n";
				}
				++iter_ptrdim;
				++iter_ptrtype;
			}

			stringstream tmpBuf;
			string actorname = actor->name;
			//List *inputList =actor->contents->decl->u.decl.type->u.operdcl.inputs;
			//List *outputList = actor->contents->decl->u.decl.type->u.operdcl.outputs;
			//ListMarker input_maker;
			//ListMarker output_maker;
			//Node *inputNode = NULL;
			//Node *outputNode = NULL;
			//string inputString;
			//string outputString;
			//IterateList(&input_maker,inputList);
			/*
			while(NextOnList(&input_maker,(GenericREF)&inputNode))
			{

				if(inputNode->typ == Id)inputString = inputNode->u.id.text;
				else if (inputNode->typ == Decl) inputString = inputNode->u.decl.name;
			}
			IterateList(&output_maker,outputList);
			vector<string> vecOutEdgeName;
			while(NextOnList(&output_maker,(GenericREF)&outputNode))
			{

				if(outputNode->typ == Id)outputString = outputNode->u.id.text;
				else if (outputNode->typ == Decl) outputString = outputNode->u.decl.name;
			}*/

			string workstring = work->toString();
			//cout<<declInitList.str()<<endl;
			//////////////////////////////////////////////////////////////////////////
			//需要在work部分寻找相关的全局以及静态变量
			map<string,map<string,string>>::iterator iter_staticvar;
			map<string,string>::iterator iter_submapofstatic;
			vector<string>::iterator iter_typeofstatic = staticvartypefinal.begin();
			map<string,map<string,string>>::iterator iter_globalvar;
			map<string,string>::iterator iter_submapofglobal;
			vector<string>::iterator iter_typeofglobal = globalvartypefinal.begin();
			vector<map<string,map<string,string>>>::iterator iter_vecofstatic,iter_vecofglobal;
			for (iter_vecofstatic = staticvariablefinal.begin();iter_vecofstatic != staticvariablefinal.end();++iter_vecofstatic)
			{
				for (iter_staticvar = (*iter_vecofstatic).begin();iter_staticvar != (*iter_vecofstatic).end();iter_staticvar++,iter_typeofstatic++)
				{
					if (workstring.find(iter_staticvar->first) != -1)
					{
						if (array2Dname.count(iter_staticvar->first))//如果改变量是二维数组，则需要对其进行改造，变成一维数组
						{
							iter_submapofstatic = iter_staticvar->second.begin();
							stringstream numss1,numss2;
							numss1<<iter_submapofstatic->first;
							int num1,num2;
							numss1>>num1;
							numss2<<iter_submapofstatic->second;
							numss2>>num2;
							buf<<"\t\t"<<*iter_typeofstatic<<" "<<iter_staticvar->first<<"_new["<<num1*num2<<"];\n";
							buf<<"\t\tfor(int i = 0;i < "<<iter_submapofstatic->first<<";i++)\n\t\t{";
							buf<<"\n\t\t\tfor(int j = 0;j <"<<iter_submapofstatic->second<<" ;j++)\n\t\t\t{\n";
							buf<<"\t\t\t\t"<<iter_staticvar->first<<"_new[i*"<<iter_submapofstatic->second<<"+j] = "<<iter_staticvar->first<<"[i][j];\n";
							buf<<"\t\t\t}\n";
							buf<<"\t\t}\n";
							//////////////////////////////////////////////////////////////////////////
							//将新生成的一维数组封装成buffer对象
							buf<<"\t\tcl::Buffer "<<iter_staticvar->first<<"_buffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,"<<(num1*num2)<<"*sizeof("<<(*iter_typeofstatic)<<"),"<<(iter_staticvar->first)<<"_new,NULL);\n";
							buf<<"\t\tkernel.setArg("<<argnum++<<","<<iter_staticvar->first<<"_buffer);\n";
						}
						else if (array1Dname.count(iter_staticvar->first))//如果改变量是一维数组，则直接封装成buffer
						{
							iter_submapofstatic = iter_staticvar->second.begin();
							stringstream numss1;
							numss1<<iter_submapofstatic->first;
							int num1;
							numss1>>num1;
							buf<<"\t\tcl::Buffer "<<iter_staticvar->first<<"_buffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,"<<num1<<"*sizeof("<<(*iter_typeofstatic)<<"),"<<(iter_staticvar->first)<<",NULL);\n";
							buf<<"\t\tkernel.setArg("<<argnum++<<","<<iter_staticvar->first<<"_buffer);\n";
						}
						else
						{
							buf<<"\t\tkernel.setArg("<<argnum++<<","<<iter_staticvar->first<<");\n";
						}
					}
				}

			}
			for (iter_vecofglobal = globalvariablefinal.begin();iter_vecofglobal != globalvariablefinal.end();++iter_vecofglobal)
			{
				for (iter_globalvar = (*iter_vecofglobal).begin();iter_globalvar != (*iter_vecofglobal).end();iter_globalvar++,iter_typeofglobal++)
				{
					int pos;
					int startpos = 0;
					while((pos = workstring.find(iter_globalvar->first,startpos))!=-1)
					{
						if (!((((int)workstring[pos+(iter_globalvar->first).length()])>=65&&((int)workstring[pos+(iter_globalvar->first).length()])<=90)||(((int)workstring[pos+(iter_globalvar->first).length()])>=97&&((int)workstring[pos+(iter_globalvar->first).length()])<=122))&&!((((int)workstring[pos-1])>=65&&((int)workstring[pos-1])<=90)||(((int)workstring[pos-1])>=97&&((int)workstring[pos-1])<=122)))
						{
							if (array2Dname.count(iter_globalvar->first))//如果改变量是二维数组，则需要对其进行改造，变成一维数组
							{
								iter_submapofglobal = iter_globalvar->second.begin();
								stringstream numss1,numss2;
								numss1<<iter_submapofglobal->first;
								int num1,num2;
								numss1>>num1;
								numss2<<iter_submapofglobal->second;
								numss2>>num2;
								buf<<"\t\t"<<*iter_typeofglobal<<" "<<iter_globalvar->first<<"_new["<<num1*num2<<"];\n";
								buf<<"\t\tfor(int i = 0;i < "<<iter_submapofglobal->first<<";i++)\n\t\t{";
								buf<<"\n\t\t\tfor(int j = 0;j <"<<iter_submapofglobal->second<<" ;j++)\n\t\t\t{\n";
								buf<<"\t\t\t\t"<<iter_globalvar->first<<"_new[i*"<<iter_submapofglobal->second<<"+j] = "<<iter_globalvar->first<<"[i][j];\n";
								buf<<"\t\t\t}\n";
								buf<<"\t\t}\n";
								//////////////////////////////////////////////////////////////////////////
								//将新生成的一维数组封装成buffer对象
								buf<<"\t\tcl::Buffer "<<iter_globalvar->first<<"_buffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,"<<(num1*num2)<<"*sizeof("<<(*iter_typeofglobal)<<"),"<<(iter_globalvar->first)<<"_new,NULL);\n";
								buf<<"\t\tkernel.setArg("<<argnum++<<","<<iter_globalvar->first<<"_buffer);\n";
							}
							else if (array1Dname.count(iter_globalvar->first))//如果改变量是一维数组，则直接封装成buffer
							{
								iter_submapofglobal = iter_globalvar->second.begin();
								stringstream numss1;
								numss1<<iter_submapofglobal->first;
								int num1;
								numss1>>num1;
								buf<<"\t\tcl::Buffer "<<iter_globalvar->first<<"_buffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,"<<num1<<"*sizeof("<<(*iter_typeofglobal)<<"),"<<(iter_globalvar->first)<<",NULL);\n";
								buf<<"\t\tkernel.setArg("<<argnum++<<","<<iter_globalvar->first<<"_buffer);\n";
							}
							else
							{
								buf<<"\t\tkernel.setArg("<<argnum++<<","<<iter_globalvar->first<<");\n";
							}
						}
						startpos = pos + 1;
					}
				}
			}
			//////////////////////////////////////////////////////////////////////////
			vector<int>::iterator iter_outpushweight = actor->outPushWeights.begin();
			vector<int>::iterator iter_inpopweight = actor->inPopWeights.begin();

			/**********************陈名韬2018-2019添加,GPU后端扩展*******************/
			
			int queuenum = i + 1;
			/*
			if (!actor->NDrangeName.empty())
			{
				buf << "\t\tcl::NDRange offset(";
				for (int i = 0; i < actor->NDrangeName.size(); i++)
				{
					if (i > 0) buf << ",";
					buf << "0";
				}
				buf << ");\n";

				buf << "\t\tcl::NDRange global_size(";
				for (int i = 0; i < actor->NDrangeName.size(); i++)
				{
					if (i > 0) buf << ",";
					buf << actor->NDrangeValue[i];
				}
				buf << ");\n";
				Node2QueueID.insert(make_pair(actor, queuenum));
				int localsizex = 1;
				buf << "\t\tcl::NDRange local_size(";
				for (int i = 0; i < actor->NDrangeName.size(); i++)
				{
					if (i > 0) buf << ",";
					buf << localsizex;
				}
				buf << ");\n";


				buf << "\t\tqueue_" << queuenum << "_1.enqueueNDRangeKernel(kernel, offset, global_size, local_size);\n";
			}
			else
			{
				//NDrange优化算法
				int actorSteadyCount = sssg_->GetSteadyCount(actor)*MultiNum;
				int localsize = 128;
				int globalsize = actorSteadyCount;
				int SMP = 14;
				int sp = 32;
				//选择localsize大小
				if (actorSteadyCount <= SMP)
				{
					localsize = 1;
				}
				else if (actorSteadyCount < SMP*sp)
				{
					localsize = actorSteadyCount / SMP;
					if (actorSteadyCount % SMP != 0)
						localsize += 1;
					globalsize = localsize * SMP;
				}
				else
				{
					localsize = 2*sp;
					if (actorSteadyCount % localsize != 0)
						globalsize = (actorSteadyCount / localsize + 1) *localsize;
				}

				buf << "\t\tcl::NDRange offset(0);\n";
				buf << "\t\tcl::NDRange global_size(" << globalsize << "/com_num);\n";
				Node2QueueID.insert(make_pair(actor, queuenum));
				buf << "\t\tif((count/com_num)%" << LocalSizeofWork << " == 0)\n";
				buf << "\t\t{\n";
				buf << "\t\t\tcl::NDRange local_size(" << localsize << ");\n";
				buf << "\t\t\tqueue_" << queuenum << "_1.enqueueNDRangeKernel(kernel, offset, global_size, local_size);\n";
				buf << "\t\t}\n";
				buf << "\t\telse\n";
				buf << "\t\t{\n";
				buf << "\t\t\tcl::NDRange local_size(" << localsize << ");\n";
				buf << "\t\t\tqueue_" << queuenum << "_1.enqueueNDRangeKernel(kernel, offset, global_size, local_size);\n";
				buf << "\t\t}\n";

			}*/
			/**********************陈名韬2018-2019添加,GPU后端扩展END*******************/
			/* 旧固定分配算法
			buf << "\t\tcl::NDRange offset(0);\n";
			buf << "\t\tcl::NDRange global_size(count/com_num);\n";
			Node2QueueID.insert(make_pair(actor, queuenum));
			buf << "\t\tif((count/com_num)%" << LocalSizeofWork << " == 0)\n";
			buf << "\t\t{\n";
			buf << "\t\t\tcl::NDRange local_size(" << LocalSizeofWork << ");\n";
			buf << "\t\t\tqueue_" << queuenum << "_1.enqueueNDRangeKernel(kernel, offset, global_size, local_size);\n";
			buf << "\t\t}\n";
			buf << "\t\telse\n";
			buf << "\t\t{\n";
			buf << "\t\t\tcl::NDRange local_size(1);\n";
			buf << "\t\t\tqueue_" << queuenum << "_1.enqueueNDRangeKernel(kernel, offset, global_size, local_size);\n";
			buf << "\t\t}\n";*/
			//NDrange优化算法
			int actorSteadyCount = sssg_->GetSteadyCount(actor)*MultiNum;
			int localsize = 128;
			int globalsize = actorSteadyCount;
			int SMP = 14;
			int sp = 32;
			//选择localsize大小
			if (actorSteadyCount <= SMP)
			{
				localsize = 1;
			}
			else if (actorSteadyCount < SMP*sp)
			{
				localsize = actorSteadyCount / SMP;
				if (actorSteadyCount % SMP != 0)
					localsize += 1;
				globalsize = localsize * SMP;
			}
			else
			{
				localsize = 2*sp;
				if (actorSteadyCount % localsize != 0)
					globalsize = (actorSteadyCount / localsize + 1) *localsize;
			}

			buf << "\t\tcl::NDRange offset(0);\n";
			buf << "\t\tcl::NDRange global_size(" << globalsize << "/com_num);\n";
			Node2QueueID.insert(make_pair(actor, queuenum));
			buf << "\t\tif((count/com_num)%" << LocalSizeofWork << " == 0)\n";
			buf << "\t\t{\n";
			buf << "\t\t\tcl::NDRange local_size(" << localsize << ");\n";
			buf << "\t\t\tqueue_" << queuenum << "_1.enqueueNDRangeKernel(kernel, offset, global_size, local_size);\n";
			buf << "\t\t}\n";
			buf << "\t\telse\n";
			buf << "\t\t{\n";
			buf << "\t\t\tcl::NDRange local_size(" << localsize << ");\n";
			buf << "\t\t\tqueue_" << queuenum << "_1.enqueueNDRangeKernel(kernel, offset, global_size, local_size);\n";
			buf << "\t\t}\n";

			thread2queue.insert(make_pair(Maflp->findPartitionNumForFlatNode(actor),queuenum));
			for (iter=actor->inFlatNodes.begin();iter!=actor->inFlatNodes.end();iter++)
			{
				if(actor->GPUPart != nGpu_ && (*iter)->GPUPart != nGpu_)
				{
					buf<<"\t\treadtag_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<" += count/com_num*"<<(*iter_inpopweight)<<";\n";
				}
				if(actor->GPUPart != nGpu_ && (*iter)->GPUPart == nGpu_)
				{
					buf<<"\t\treadtag_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<" += count/com_num*"<<(*iter_inpopweight)<<";\n";
				}
				iter_inpopweight++;
			}
			for (iter_1=actor->outFlatNodes.begin();iter_1!=actor->outFlatNodes.end();iter_1++)
			{
				if(actor->GPUPart != nGpu_ && (*iter_1)->GPUPart != nGpu_)
				{
					buf<<"\t\twritetag_"<<actor->name<<"_"<<(*iter_1)->name<<"_"<<i<<" += count/com_num*"<<(*iter_outpushweight)<<";\n";
				}
				if(actor->GPUPart != nGpu_ && (*iter_1)->GPUPart == nGpu_)
				{
					buf<<"\t\twritetag_"<<actor->name<<"_"<<(*iter_1)->name<<"_"<<i<<" += count/com_num*"<<(*iter_outpushweight)<<";\n";
				}
				iter_outpushweight++;
			}
			buf << "\t}\n"; // work方法结束
		}
	}
}
void GPUCodeGenerate::CGthis(FlatNode *actor, stringstream &buf,vector<string> inEdgeName,vector<string> outEdgeName)
{
	buf <<"\t// Constructor\n";
	buf << "\t"<<actor->name<<"(";
	multimap<FlatNode*,string>::iterator iter1,iter2;
	iter1 = mapActor2InEdge.find(actor);
	for(int index = 0;index< actor->inFlatNodes.size();index++)
	{
		string inputString = inEdgeName[index];
		if(DataDependence(actor->inFlatNodes[index], actor) && (actor->GPUPart == nGpu_ || (actor->GPUPart != nGpu_ && actor->inFlatNodes[index]->GPUPart == nGpu_)))
		{
			string edgetype = ReturnTypeOfEdge(actor->inFlatNodes[index], actor);
			buf << "Buffer<" << edgetype << ">&" << inputString << ",";
		}
		edge2name.insert(make_pair(iter1->second,inputString));
		++iter1;
	}
	iter2=mapActor2OutEdge.find(actor);
	for(int index = 0;index< actor->outFlatNodes.size();index++)
	{
		assert(iter2!=mapActor2OutEdge.end());
		string outputString = outEdgeName[index];
		if (DataDependence(actor, actor->outFlatNodes[index]) && (actor->outFlatNodes[index]->GPUPart == nGpu_ || (actor->outFlatNodes[index]->GPUPart != nGpu_ && actor->GPUPart == nGpu_)))
		{
			string edgetype = ReturnTypeOfEdge(actor, actor->outFlatNodes[index]);
			buf << "Buffer<" << edgetype << ">&" << outputString << ",";
		}
		edge2name.insert(make_pair(iter2->second,outputString));
		++iter2;
	}
	buf<<"void *lazy = NULL):";
	iter1=mapActor2InEdge.find(actor);
	iter2=mapActor2OutEdge.find(actor);
	for(int index = 0;index< actor->inFlatNodes.size();index++)
	{
		assert(iter1!=mapActor2InEdge.end());
		string inputString = inEdgeName[index];
		if (DataDependence(actor->inFlatNodes[index], actor) && (actor->GPUPart == nGpu_ || (actor->GPUPart != nGpu_ && actor->inFlatNodes[index]->GPUPart == nGpu_)))
		{
			string edgetype = ReturnTypeOfEdge(actor->inFlatNodes[index], actor);
			buf << inputString << "(Consumer<" << edgetype << ">(" << inputString << ")),";
		}
			
		++iter1;
	}
	for(int index = 0;index< actor->outFlatNodes.size();index++)
	{
		assert(iter2!=mapActor2OutEdge.end());
		string outputString = outEdgeName[index];
		//if(outputNode->typ == Id)outputString = outputNode->u.id.text;
		//else if (outputNode->typ == Decl) outputString = outputNode->u.decl.name;
		if (DataDependence(actor, actor->outFlatNodes[index]) && (actor->outFlatNodes[index]->GPUPart == nGpu_ || (actor->outFlatNodes[index]->GPUPart != nGpu_ && actor->GPUPart == nGpu_)))
		{
			string edgetype = ReturnTypeOfEdge(actor, actor->outFlatNodes[index]);
			buf << outputString << "(Producer<" << edgetype << ">(" << outputString << ")),";
		}
		++iter2;
	}
	buf.seekp((int)buf.tellp()-1);
	buf<<"{\n";
	buf << thisBuf.str();
	//buf << "\t\tRepeatCount = rc;\n ";
	//	buf<< "\t\tsteadyScheduleCount = "<<sssg_->GetSteadyCount(actor)*Maflp->MultiNum2FlatNode[actor]/NCpuThreads<<";\n";
	//buf<< "\t\tsteadyScheduleCount = "<<sssg_->GetSteadyCount(actor)*Maflp->MultiNum2FlatNode[actor]<<";\n";
	buf<< "\t\tsteadyScheduleCount = "<<sssg_->GetSteadyCount(actor)<<";\n";
	cout<<sssg_->GetSteadyCount(actor);
	buf<< "\t\tinitScheduleCount = "<<sssg_->GetInitCount(actor)<<";\n";
	CGdatataginit(actor,buf);
	buf << "\t}\n"; // init方法结束
}

void GPUCodeGenerate::CGAllKernel()
{
	stringstream buf;
	//?多了个分号  2018/7/2 陈名韬删除
	buf<<"#pragma OPENCL EXTENSION cl_khr_fp64 : enable\n\n";
	map<string,string>::iterator iter_nametype;
	for (iter_nametype = allnameandtype.begin();iter_nametype != allnameandtype.end();++iter_nametype)
	{
		buf<<"typedef struct\n";
		buf<<"{\n";
		buf<<"\t"<<iter_nametype->second<<"\n";
		buf<<"}mystruct_"<<iter_nametype->first<<";\n";
	}
	int stageminus,size;
	vector<FlatNode*>::iterator iter,iter_1;
	vector<int>::iterator iter_2;
	for (iter_1 = flatNodes_.begin();iter_1 != flatNodes_.end();iter_1 ++)
	{
		if((*iter_1)->GPUPart != nGpu_)
		{
			/*******************GPU拓展代码*************
			if (!(*iter_1)->NDrangeName.empty())
			{
				cout << "find Special GPU Actor:" << (*iter_1)->name << endl;
				//actorMarkGPUnode[*iter_1] = TRUE;
				CGSpeciallKernel(iter_1,buf);
				continue;
			}
			****************GPU拓展代码***************/

			buf<<"__kernel void kernel"<<(*iter_1)->name<<"(";
			map<map<FlatNode*,FlatNode*>,string>::iterator iter_buffername;
			map<FlatNode*,FlatNode*>searchmap;
			for (iter=(*iter_1)->inFlatNodes.begin();iter!=(*iter_1)->inFlatNodes.end();iter++)
			{
				if((*iter)->GPUPart == nGpu_)
				{
					searchmap.clear();
					searchmap.insert(make_pair(*iter,*iter_1));
					iter_buffername = BufferNameOfEdge.find(searchmap);
					buf << "__global const mystruct_" << ReturnTypeOfEdge(*iter, *iter_1);
					buf << " *" << iter_buffername->second << ", ";
					buf<<"int readtag_"<<(*iter)->name<<"_"<<(*iter_1)->name<<", ";
					//buf<<"\t\tkernel.setArg("<<argnum++<<",writedatatag_"<<(*iter)->name<<"_"<<actor->name<<");\n";
				}
				if((*iter)->GPUPart != nGpu_)
				{
					searchmap.clear();
					searchmap.insert(make_pair(*iter,*iter_1));
					iter_buffername = BufferNameOfEdge.find(searchmap);
					buf<<"__global const mystruct_"<<ReturnTypeOfEdge(*iter,*iter_1)<<" *"<<iter_buffername->second<<", ";
					buf<<"int readtag_"<<(*iter)->name<<"_"<<(*iter_1)->name<<", ";
				}
			}
			for (iter=(*iter_1)->outFlatNodes.begin();iter!=(*iter_1)->outFlatNodes.end();iter++)
			{	
				if((*iter)->GPUPart == nGpu_)
				{
					searchmap.clear();
					searchmap.insert(make_pair(*iter_1,*iter));
					iter_buffername = BufferNameOfEdge.find(searchmap);
					buf<<"__global mystruct_"<<ReturnTypeOfEdge(*iter_1,*iter)<<" *"<<iter_buffername->second<<", ";
					buf<<"int writetag_"<<(*iter_1)->name<<"_"<<(*iter)->name<<", ";
				}
				if((*iter)->GPUPart != nGpu_)
				{
					searchmap.clear();
					searchmap.insert(make_pair(*iter_1,*iter));
					iter_buffername = BufferNameOfEdge.find(searchmap);
					buf<<"__global mystruct_"<<ReturnTypeOfEdge(*iter_1,*iter)<<" *";
					buf<<iter_buffername->second<<", ";
					buf<<"int writetag_"<<(*iter_1)->name<<"_"<<(*iter)->name<<", ";
				}
			}
			map<FlatNode *,vector<string>>::iterator iter_param,iter_paramtype;
			vector<string>paramofactor,paramtypeofactor;
			vector<string>::iterator iter_paofac,iter_patyofac;
			iter_param = Actor2KernelPar.find((*iter_1));
			iter_paramtype = Actor2KernelParType.find(*iter_1);
			paramofactor = iter_param->second;
			paramtypeofactor = iter_paramtype->second;
			for (iter_paofac = paramofactor.begin(),iter_patyofac = paramtypeofactor.begin();(iter_paofac != paramofactor.end())&&(iter_patyofac != paramtypeofactor.end());iter_paofac++,iter_patyofac++)
			{
				buf<<(*iter_patyofac)<<" "<<(*iter_paofac)<<", ";
			}
			pair<multimap<FlatNode*,map<string,string>>::iterator,multimap<FlatNode*,map<string,string>>::iterator>pos_ptr = actor2ptr.equal_range(*iter_1);
			map<string,string>::iterator iter_ptr;
			while (pos_ptr.first != pos_ptr.second)
			{
				iter_ptr = pos_ptr.first->second.begin();
				buf<<"__global "<<(iter_ptr)->first<<" *"<<(iter_ptr)->second<<", ";
				pos_ptr.first++;
			}
			//declInitList.str("");
			//Node *work = (*iter_1)->contents->body->u.operBody.work;
			//SPL2GPU_Node(work, 2);
			auto oper = (*iter_1)->contents;
			operBodyNode *body = oper->operBody;
			Node *work = body->work;

			string kernelstring;
			if ((*iter_1)->outFlatNodes.size())
			{
				kernelstring =work->toString();
			}

			//cout<<kernelstring<<endl;
			//////////////////////////////////////////////////////////////////////////
			//全局变量以及静态变量参数
			int num = allstaticvariable.size();
			staticvariable.clear();
			staticvariablefinal.clear();
			map<string,map<string,string>>tempmapofstatic,tempmapofstaticfinal;
			pair<multimap<FlatNode *,map<string,map<string,string>>>::iterator,multimap<FlatNode *,map<string,map<string,string>>>::iterator>pos_static;
			map<string,map<string,string>>::iterator iter_substatic;
			pos_static = allstaticvariable.equal_range(*iter_1);
			while (pos_static.first != pos_static.second)
			{
				for(iter_substatic = pos_static.first->second.begin();iter_substatic != pos_static.first->second.end();++iter_substatic)
				{
					tempmapofstatic.clear();
					tempmapofstatic.insert(make_pair(iter_substatic->first,iter_substatic->second));
					staticvariable.push_back(tempmapofstatic);
					tempmapofstaticfinal.clear();
					tempmapofstaticfinal.insert(make_pair(iter_substatic->first,iter_substatic->second));
					staticvariablefinal.push_back(tempmapofstaticfinal);
				}
				pos_static.first++;
			}
			pair<multimap<FlatNode *,vector<string>>::iterator,multimap<FlatNode *,vector<string>>::iterator>pos_statictype;
			vector<string>::iterator iter_type;
			pos_statictype = allstatictype.equal_range(*iter_1);
			while (pos_statictype.first != pos_statictype.second)
			{
				for (iter_type = pos_statictype.first->second.begin();iter_type != pos_statictype.first->second.end();++iter_type)
				{
					staticvartype.push_back(*iter_type);
					staticvartypefinal.push_back(*iter_type);
				}
				pos_statictype.first++;
			}
			map<string,map<string,string>>::iterator iter_staticvar;
			map<string,string>::iterator iter_submapofstatic;
			vector<string>::iterator iter_typeofstatic = staticvartypefinal.begin();
			map<string,map<string,string>>::iterator iter_globalvar;
			map<string,string>::iterator iter_submapofglobal;
			vector<string>::iterator iter_typeofglobal = globalvartypefinal.begin();
			vector<map<string,map<string,string>>>::iterator iter_vecofstatic,iter_vecofglobal;
			for (iter_vecofstatic = staticvariablefinal.begin();iter_vecofstatic != staticvariablefinal.end();++iter_vecofstatic)
			{
				iter_staticvar = (*iter_vecofstatic).begin();
				{
					if (kernelstring.find(iter_staticvar->first) != -1)
					{
						if (array2Dname.count(iter_staticvar->first))//如果改变量是二维数组，则需要对其进行改造，变成一维数组
						{
							buf<<"__global "<<*iter_typeofstatic<<" *"<<iter_staticvar->first<<", ";
						}
						else if (array1Dname.count(iter_staticvar->first))//如果改变量是一维数组，则直接封装成buffer
						{
							buf<<"__global "<<*iter_typeofstatic<<" *"<<iter_staticvar->first<<", ";
						}
						else
						{
							buf<<"__global "<<*iter_typeofstatic<<" "<<iter_staticvar->first<<", ";
						}
					}
				}

			}
			for (iter_vecofglobal = globalvariablefinal.begin();iter_vecofglobal != globalvariablefinal.end();++iter_vecofglobal)
			{
				for (iter_globalvar = (*iter_vecofglobal).begin();iter_globalvar != (*iter_vecofglobal).end();iter_globalvar++,iter_typeofglobal++)
				{
					int pos;
					int startpos = 0;
					while((pos = kernelstring.find(iter_globalvar->first,startpos))!=-1)
					{
						if (!((((int)kernelstring[pos+(iter_globalvar->first).length()])>=65&&((int)kernelstring[pos+(iter_globalvar->first).length()])<=90)||(((int)kernelstring[pos+(iter_globalvar->first).length()])>=97&&((int)kernelstring[pos+(iter_globalvar->first).length()])<=122))&&!((((int)kernelstring[pos-1])>=65&&((int)kernelstring[pos-1])<=90)||(((int)kernelstring[pos-1])>=97&&((int)kernelstring[pos-1])<=122)))
						{
							if (array2Dname.count(iter_globalvar->first))//如果改变量是二维数组，则需要对其进行改造，变成一维数组
							{
								buf<<"__global "<<*iter_typeofglobal<<" *"<<iter_globalvar->first<<", ";
							}
							else if (array1Dname.count(iter_globalvar->first))//如果改变量是一维数组，则直接封装成buffer
							{
								buf<<"__global "<<*iter_typeofglobal<<" *"<<iter_globalvar->first<<", ";
							}
							else
							{
								buf<<"__global "<<*iter_typeofglobal<<" "<<iter_globalvar->first<<", ";
							}
						}
						startpos = pos+1;
					}
					
				}
			}
			//////////////////////////////////////////////////////////////////////////
			buf.seekp((int)buf.tellp()-2);
			buf<<")\n";
			buf<<"{\n";

			if ((*iter_1)->outFlatNodes.size())
			{
				buf << "\tint id = get_global_id(0);\n";
				int steadycount = sssg_->GetSteadyCount(*iter_1)*MultiNum / nGpu_;
				buf << "\tif(id < " << steadycount << ")\n";
			}
			//增加处理GPU actor cmt



			//
			//////////////////////////////////////////////////////////////////////////
			//字符串的处理
			vector<int>::iterator iter_pop,iter_push;//分别用来迭代actor的pop和push值
			iter_pop = (*iter_1)->inPopWeights.begin();
			iter_push = (*iter_1)->outPushWeights.begin();
			for (iter=(*iter_1)->inFlatNodes.begin();iter!=(*iter_1)->inFlatNodes.end();iter++)
			{
				searchmap.clear();
				searchmap.insert(make_pair(*iter,*iter_1));
				iter_buffername = BufferNameOfEdge.find(searchmap);
				int pos1,pos2,pos3;
				pos2 = 0;
				stringstream ss1,ss2;
				string buffersizestring;
				string popnumstring;
				string searchstring = iter_buffername->second+"[";
				while ((pos2!=-1)&&((pos1 = kernelstring.find(searchstring,pos2)) != -1))
				{
					if (pos1 != 0 && !(((kernelstring[pos1 - 1] >= 'A') && (kernelstring[pos1 - 1] <= 'Z')) || ((kernelstring[pos1 - 1] >= 97) && (kernelstring[pos1 - 1] <= 122))))
					{
						string substring;
						//处理括号匹配的问题
						stack<string>stk;
						pos2 = kernelstring.find("[",pos1);
						stk.push("[");
						int posstart = pos2+1;
						int poshere;
						string findstring = "[]";
						while ((poshere = kernelstring.find_first_of(findstring,posstart))!=-1)
						{
							if (kernelstring[poshere] == '[')
							{
								stk.push("[");
							}
							else
							{
								stk.pop();
							}
							if (stk.size() == 0)
							{
								break;
							}
							posstart = poshere+1;;
						}
						//	cout<<kernelstring[poshere]<<endl;
						pos3 = poshere;
						substring = kernelstring.substr(pos2+1,pos3-pos2-1);
						substring = "(" +substring+")"+"+readtag_"+(*iter)->name+"_"+(*iter_1)->name;
						int buffersize;
						if (ReturnBufferSize(*iter,*iter_1) == 0)
						{
							buffersize = (ReturnBufferSize(*iter,*iter_1)+1);
						}
						else
						{
							buffersize = ReturnBufferSize(*iter,*iter_1);
						}
						if (!IsMod)
						{
							buffersize = ReturnNumBiger(buffersize);
						}
						ss1<<buffersize;
						ss1>>buffersizestring;	
						ss2<<*iter_pop;
						popnumstring=ss2.str();
						if (IsMod)
						{
							substring = "(" +substring+"+id*"+popnumstring+")%"+buffersizestring;
						}
						else
						{
							substring = "(" +substring+"+id*"+popnumstring+")&("+buffersizestring+"-1)";
						}
						kernelstring.replace(pos2+1,pos3-pos2-1,substring);
						ss1.str("");
						ss2.str("");
						pos2 = pos3;
					}
					else if(pos1 == 0)
					{
						if (!(((kernelstring[pos1+iter_buffername->second.length()]>=65)&&(kernelstring[pos1+iter_buffername->second.length()]<=90))||((kernelstring[pos1+iter_buffername->second.length()]>=97)&&(kernelstring[pos1+iter_buffername->second.length()]<=122))))
						{
							string substring;
							//处理括号匹配的问题
							stack<string>stk;
							pos2 = kernelstring.find("[",pos1);
							stk.push("[");
							int posstart = pos2+1;
							int poshere;
							string findstring = "[]";
							while ((poshere = kernelstring.find_first_of(findstring,posstart))!=-1)
							{
								if (kernelstring[poshere] == '[')
								{
									stk.push("[");
								}
								else
								{
									stk.pop();
								}
								if (stk.size() == 0)
								{
									break;
								}
								posstart = poshere+1;;
							}
							//	cout<<kernelstring[poshere]<<endl;
							pos3 = poshere;
							substring = kernelstring.substr(pos2+1,pos3-pos2-1);
							substring = "(" +substring+")"+"+readtag_"+(*iter)->name+"_"+(*iter_1)->name;
							int buffersize;
							if (ReturnBufferSize(*iter,*iter_1) == 0)
							{
								buffersize = ReturnBufferSize(*iter,*iter_1)+1;
							}
							else
							{
								buffersize = ReturnBufferSize(*iter,*iter_1);
							}
							if (!IsMod)
							{
								buffersize = ReturnNumBiger(buffersize);
							}
							ss1<<buffersize;
							ss1>>buffersizestring;	
							ss2<<*iter_pop;
							popnumstring=ss2.str();
							if (IsMod)
							{
								substring = "(" +substring+"+id*"+popnumstring+")%"+buffersizestring;
							}
							else
							{
								substring = "(" +substring+"+id*"+popnumstring+")&("+buffersizestring+"-1)";
							}
							kernelstring.replace(pos2+1,pos3-pos2-1,substring);
							ss1.str("");
							ss2.str("");
							pos2 = pos3;
						}
					}
					else
					{
						pos2 = pos1+1;
					}
				}
				iter_pop++;
			}
			//cout<<"---------------------"<<endl;
			//cout<<kernelstring<<endl;
			for (iter=(*iter_1)->outFlatNodes.begin();iter!=(*iter_1)->outFlatNodes.end();iter++)
			{	
				searchmap.clear();
				searchmap.insert(make_pair(*iter_1,*iter));
				iter_buffername = BufferNameOfEdge.find(searchmap);
				int pos1,pos2,pos3;
				pos2 = 0;
				stringstream ss1,ss2;
				string buffersizestring;
				string pushnumstring;
				string searchstring = iter_buffername->second+"[";
				while ((pos1 = kernelstring.find(searchstring,pos2)) != -1)
				{
					if (pos1 != 0 &&!(((kernelstring[pos1-1]>=65)&&(kernelstring[pos1-1]<=90))||((kernelstring[pos1-1]>=97)&&(kernelstring[pos1-1]<=122))))
					{
						string substring;
						stack<string>stk;
						/*cout<<kernelstring<<endl;*/
						pos2 = kernelstring.find("[",pos1);
						stk.push("[");
						int posstart = pos2+1;
						int poshere;
						string findstring = "[]";
						while ((poshere = kernelstring.find_first_of(findstring,posstart))!=-1)
						{
							if (kernelstring[poshere] == '[')
							{
								stk.push("[");
							}
							else
							{
								stk.pop();
							}
							if (stk.size() == 0)
							{
								break;
							}
							posstart = poshere+1;;
						}
						pos3 = poshere;
						substring = kernelstring.substr(pos2+1,pos3-pos2-1);
						substring = "(" +substring+")"+"+writetag_"+(*iter_1)->name+"_"+(*iter)->name;
						int buffersize;
						if (ReturnBufferSize(*iter_1,*iter) == 0)
						{
							buffersize = ReturnBufferSize(*iter_1,*iter)+1;
						}
						else
						{
							buffersize = ReturnBufferSize(*iter_1,*iter);
						}
						if (!IsMod)
						{
							buffersize = ReturnNumBiger(buffersize);
						}
						//	ss1<<ReturnBufferSize(*iter_1,*iter);
						ss1<<buffersize;
						ss1>>buffersizestring;
						ss2<<(*iter_push);
						ss2>>pushnumstring;
						if (IsMod)
						{
							substring = "(" +substring+"+id*"+pushnumstring+")%"+buffersizestring;
						}
						else
						{
							substring = "(" +substring+"+id*"+pushnumstring+")&("+buffersizestring+"-1)";
						}
						kernelstring.replace(pos2+1,pos3-pos2-1,substring);
						ss1.str("");
						ss2.str("");
						pos2 = pos3;
					}
					else if(pos1 == 0)
					{
						if (!(((kernelstring[pos1+iter_buffername->second.length()]>=65)&&(kernelstring[pos1+iter_buffername->second.length()]<=90))||((kernelstring[pos1+iter_buffername->second.length()]>=97)&&(kernelstring[pos1+iter_buffername->second.length()]<=122))))
						{
							string substring;
							stack<string>stk;
							/*cout<<kernelstring<<endl;*/
							pos2 = kernelstring.find("[",pos1);
							stk.push("[");
							int posstart = pos2+1;
							int poshere;
							string findstring = "[]";
							while ((poshere = kernelstring.find_first_of(findstring,posstart))!=-1)
							{
								if (kernelstring[poshere] == '[')
								{
									stk.push("[");
								}
								else
								{
									stk.pop();
								}
								if (stk.size() == 0)
								{
									break;
								}
								posstart = poshere+1;;
							}
							pos3 = poshere;
							substring = kernelstring.substr(pos2+1,pos3-pos2-1);
							substring = "(" +substring+")"+"+writetag_"+(*iter_1)->name+"_"+(*iter)->name;
							int buffersize;
							if (ReturnBufferSize(*iter_1,*iter) == 0)
							{
								buffersize = ReturnBufferSize(*iter_1,*iter)+1;
							}
							else
							{
								buffersize = ReturnBufferSize(*iter_1,*iter);
							}
							if (!IsMod)
							{
								buffersize = ReturnNumBiger(buffersize);
							}
							//	ss1<<ReturnBufferSize(*iter_1,*iter);
							ss1<<buffersize;
							ss1>>buffersizestring;
							ss2<<(*iter_push);
							ss2>>pushnumstring;
							if (IsMod)
							{
								substring = "(" +substring+"+id*"+pushnumstring+")%"+buffersizestring;
							}
							else
							{
								substring = "(" +substring+"+id*"+pushnumstring+")&("+buffersizestring+"-1)";
							}
							kernelstring.replace(pos2+1,pos3-pos2-1,substring);
							ss1.str("");
							ss2.str("");
							pos2 = pos3;
						}
					}
					else
					{
						pos2 = pos1 + 1;
					}
				}
				iter_push++;
			}
			//////////////////////////////////////////////////////////////////////////
			//对kernel函数中的二维全局变量以及二维静态变量进行下标的修改，使之变为一维数组
			set<string>::iterator iter_2Darray;
			map<string,map<string,string>>::iterator iter_2Dimarray;
			map<string,string>::iterator iter_submap;
			string searchstring;
			for (iter_2Darray = array2Dname.begin();iter_2Darray != array2Dname.end();iter_2Darray++)
			{
				int pos1 = 0;
				int pos2,pos3,pos4,pos5,pos6;
				string num1string,num2string;
				searchstring = *iter_2Darray + "[";
				while(kernelstring.find(searchstring,pos1) != -1)
				{
					stringstream ss1,ss2;
					string substring;
					bool flag_global = false;
					bool flag_static = false;
					pos1 = kernelstring.find(searchstring,pos1);
					pos2 = kernelstring.find(*iter_2Darray,pos1);
					pos3 = kernelstring.find("[",pos2);
					pos4 = kernelstring.find("]",pos3);
					pos5 = kernelstring.find("[",pos4);
					pos6 = kernelstring.find("]",pos5);
					num1string = kernelstring.substr(pos3+1,pos4-pos3-1);
					num2string = kernelstring.substr(pos5+1,pos6-pos5-1);
					vector<map<string,map<string,string>>>::iterator iter_vecofglobal;
					map<string,map<string,string>>::iterator iter_subvecofglobal;
					for (iter_vecofglobal = globalvariablefinal.begin();iter_vecofglobal != globalvariablefinal.end();++iter_vecofglobal)
					{
						iter_subvecofglobal = (*iter_vecofglobal).begin();
						if (iter_subvecofglobal->first == (*iter_2Darray))
						{
							iter_2Dimarray = iter_subvecofglobal;
							flag_global = true;
							break;
						}
					}
					if (flag_global)
					{
						iter_submap = iter_2Dimarray->second.begin();
						substring = num1string+"*"+iter_submap->second+"+"+num2string;
						kernelstring.replace(pos3+1,pos6-pos3-1,substring);
						pos1 = kernelstring.rfind(substring) + 1;
					}
					else
					{
						vector<map<string,map<string,string>>>::iterator iter_vecofstatic;
						map<string,map<string,string>>::iterator iter_subvecofstatic;
						for (iter_vecofstatic = staticvariablefinal.begin();iter_vecofstatic != staticvariablefinal.end();iter_vecofstatic++)
						{
							iter_subvecofstatic = (*iter_vecofstatic).begin();
							if (iter_subvecofstatic->first == (*iter_2Darray))
							{
								iter_2Dimarray = iter_subvecofstatic;
								flag_static = true;
								break;
							}
						}
					}
					if (flag_static)
					{
						iter_submap = iter_2Dimarray->second.begin();
						substring = num1string+"*"+iter_submap->second+"+"+num2string;
						kernelstring.replace(pos3+1,pos6-pos3-1,substring);
						pos1 = kernelstring.rfind(substring) + 1;
					}
					/*cout<<kernelstring<<endl;*/
				}
			}
			/*cout<<"---------------"<<endl;
			cout<<kernelstring<<endl;*/
			//////////////////////////////////////////////////////////////////////////
			//对kernel中的局部二维数组的访问进行修改
			pair<multimap<FlatNode *,map<string,map<string,string>>>::iterator,multimap<FlatNode *,map<string,map<string,string>>>::iterator>pos_local;
			pos_local = alllocalvariable.equal_range(*iter_1);
			map<string,map<string,string>>localsubmap;
			map<string,map<string,string>>::iterator iter_local;
			while (pos_local.first != pos_local.second)
			{
				for (iter_local = pos_local.first->second.begin();iter_local != pos_local.first->second.end();iter_local++)
				{
					localsubmap.insert(make_pair(iter_local->first,iter_local->second));
				}
				pos_local.first++;
			}
			for (iter_2Darray = arraylocal2Dname.begin();iter_2Darray != arraylocal2Dname.end();iter_2Darray++)
			{
				int pos1 = 0;
				int pos2,pos3,pos4,pos5,pos6;
				string num1string,num2string;
				searchstring = *iter_2Darray + "[";
				while(kernelstring.find(searchstring,pos1) != -1)
				{
					stringstream ss1,ss2;
					string substring;
					pos1 = kernelstring.find(searchstring,pos1);
					pos2 = kernelstring.find(*iter_2Darray,pos1);
					pos3 = kernelstring.find("[",pos2);
					pos4 = kernelstring.find("]",pos3);
					pos5 = kernelstring.find("[",pos4);
					pos6 = kernelstring.find("]",pos5);
					num1string = kernelstring.substr(pos3+1,pos4-pos3-1);
					num2string = kernelstring.substr(pos5+1,pos6-pos5-1);
					iter_2Dimarray = localsubmap.find(*iter_2Darray);
					iter_submap = iter_2Dimarray->second.begin();
					substring = num1string+"*"+iter_submap->second+"+"+num2string;
					kernelstring.replace(pos3+1,pos6-pos3-1,substring);
					pos1 = kernelstring.rfind(substring) + 1;
					/*cout<<kernelstring<<endl;*/
				}
			}
			//////////////////////////////////////////////////////////////////////////
			buf << kernelstring;
			buf<<"\n}\n";
		}
	}
	//输出到文件
	string filename = "allkernel.cl";
	ofstream out(filename);
	out << buf.str();
}
void GPUCodeGenerate::CGdatataginit(FlatNode *actor,stringstream &buf)
{
	vector<FlatNode*>::iterator iter,iter_1;
	buf<<"\t//读写标志位的初始化\n";
	for (int i = 0;i < nGpu_; i++)
	{
		for (iter=actor->inFlatNodes.begin();iter!=actor->inFlatNodes.end();iter++)
		{
			//if (!DetectiveFilterState(actor)&&(Maflp->UporDownStatelessNode(actor)!=3)&&(Maflp->UporDownStatelessNode(*iter)!=3)&&!DetectiveFilterState(*iter)&&Maflp->findPartitionNumForFlatNode(actor)==Maflp->findPartitionNumForFlatNode(*iter))
			if(actor->GPUPart != nGpu_ && (*iter)->GPUPart != nGpu_ && Maflp->findPartitionNumForFlatNode(actor)==Maflp->findPartitionNumForFlatNode(*iter))
			{
				buf<<"\t\treadtag_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<" = 0;\n";
			}
			//if (!DetectiveFilterState(actor)&&(Maflp->UporDownStatelessNode(actor)!=3)&&(Maflp->UporDownStatelessNode(*iter)!=3)&&!DetectiveFilterState(*iter)&&Maflp->findPartitionNumForFlatNode(actor)!=Maflp->findPartitionNumForFlatNode(*iter))
			if(actor->GPUPart != nGpu_ && (*iter)->GPUPart != nGpu_ && Maflp->findPartitionNumForFlatNode(actor)!=Maflp->findPartitionNumForFlatNode(*iter))
			{
				buf<<"\t\treadtag_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<" = 0;\n";
				buf<<"\t\twritedatatag_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<" = 0;\n";
			}
			//if (!DetectiveFilterState(actor)&&(Maflp->UporDownStatelessNode(actor)!=3)&&(DetectiveFilterState(*iter)||(Maflp->UporDownStatelessNode(*iter)==3)))
			if(actor->GPUPart != nGpu_ && (*iter)->GPUPart == nGpu_)
			{
				buf<<"\t\treadtag_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<" = 0;\n";
				buf<<"\t\twritedatatag_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<" = 0;\n";
			}
		}
		for (iter_1=actor->outFlatNodes.begin();iter_1!=actor->outFlatNodes.end();iter_1++)
		{
			if(actor->GPUPart != nGpu_ && (*iter_1)->GPUPart != nGpu_ && Maflp->findPartitionNumForFlatNode(actor)==Maflp->findPartitionNumForFlatNode(*iter_1))
			{
				buf<<"\t\twritetag_"<<(actor)->name<<"_"<<(*iter_1)->name<<"_"<<i<<" = 0;\n";
			}
			if(actor->GPUPart != nGpu_ && (*iter_1)->GPUPart != nGpu_ && Maflp->findPartitionNumForFlatNode(actor)!=Maflp->findPartitionNumForFlatNode(*iter_1))
			{
				buf<<"\t\twritetag_"<<(actor)->name<<"_"<<(*iter_1)->name<<"_"<<i<<" = 0;\n";
				buf<<"\t\treaddatatag_"<<(actor)->name<<"_"<<(*iter_1)->name<<"_"<<i<<" = 0;\n";
			}
			if(actor->GPUPart != nGpu_ && (*iter_1)->GPUPart == nGpu_)
			{
				buf<<"\t\twritetag_"<<(actor)->name<<"_"<<(*iter_1)->name<<"_"<<i<<" = 0;\n";
				buf<<"\t\treaddatatag_"<<(actor)->name<<"_"<<(*iter_1)->name<<"_"<<i<<" = 0;\n";
			}
		}
	}
	
}

void GPUCodeGenerate::CGinitWork(stringstream &buf)
{
	buf << "\t// initWork\n";
	buf << "\tvoid initWork() {\n";
	buf << "\t\tinitVarAndState();\n";
	buf << "\t\tinit();\n";
	buf << "\t}\n"; // initWork方法结束
}

void GPUCodeGenerate::CGexternbuffer(FlatNode *actor,stringstream &buf)
{
	vector<FlatNode*>::iterator iter,iter_1;
	/*if (PrintFlag)
	{
	for (int i = 0; i < nGpu_; i++)
	{
	buf<<"extern bool printflag_"<<i<<";\n";
	}
	}*/
	buf<<"extern int com_num;\n";
	buf<<"extern vector<cl::Platform>platforms;\n";
	buf<<"extern vector<cl::Device>platformsDevices;\n";
	buf<<"extern cl::Context context;\n";
	for (int i = 0;i < nGpu_;i++)
	{
		buf<<"extern cl::CommandQueue queue_"<<i+1<<"_0;\n";
		buf<<"extern cl::CommandQueue queue_"<<i+1<<"_1;\n";
	}
	/*buf<<"extern cl::CommandQueue queue_1;\n";
	buf<<"extern cl::CommandQueue queue_2;\n";
	buf<<"extern cl::CommandQueue queue_3;\n";*/
	buf<<"extern cl::Program program;\n";
	//各个GPU存储cwb
	for (int i = 0; i < nGpu_; i++)
	{
		for (iter=actor->inFlatNodes.begin();iter!=actor->inFlatNodes.end();iter++)
		{
			//上端是GPU节点，当前节点为CPU节点
			//if ((!DetectiveFilterState(actor)&&DetectiveFilterState(*iter))||(!DetectiveFilterState(actor)&&!DetectiveFilterState(*iter)&&Maflp->findPartitionNumForFlatNode(actor)!=Maflp->findPartitionNumForFlatNode(*iter)))
			if((actor->GPUPart != nGpu_ && (*iter)->GPUPart == nGpu_) || (actor->GPUPart != nGpu_ && (*iter)->GPUPart != nGpu_ && Maflp->findPartitionNumForFlatNode(actor)!=Maflp->findPartitionNumForFlatNode(*iter)))
			{
				buf<<"extern cl::Buffer Inbuffer_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<";\n";
				buf<<"extern cl::Buffer cmPinnedBufIn_"<<(*iter)->name<<"_"<<actor->name<<"_init_"<<i<<";\n";
				buf<<"extern cl::Buffer cmDevBufIn_"<<(*iter)->name<<"_"<<actor->name<<"_init_"<<i<<";\n";
				buf<<"extern mystruct_"<<ReturnTypeOfEdge(*iter,actor)<<" *Inst_"<<(*iter)->name<<"_"<<actor->name<<"_init_"<<i<<";\n";
				buf<<"extern cl::Buffer cmPinnedBufIn_"<<(*iter)->name<<"_"<<actor->name<<"_steady_"<<i<<";\n";
				buf<<"extern cl::Buffer cmDevBufIn_"<<(*iter)->name<<"_"<<actor->name<<"_steady_"<<i<<";\n";
				buf<<"extern mystruct_"<<ReturnTypeOfEdge(*iter,actor)<<" *Inst_"<<(*iter)->name<<"_"<<actor->name<<"_steady_"<<i<<";\n";
			}
		}
		for (iter=actor->inFlatNodes.begin();iter!=actor->inFlatNodes.end();iter++)
		{
			//if (!DetectiveFilterState(actor)&&!DetectiveFilterState(*iter)&&Maflp->findPartitionNumForFlatNode(actor)==Maflp->findPartitionNumForFlatNode(*iter))
			if(actor->GPUPart != nGpu_ && (*iter)->GPUPart != nGpu_ && Maflp->findPartitionNumForFlatNode(actor)==Maflp->findPartitionNumForFlatNode(*iter))
			{
				buf<<"extern cl::Buffer Outbuffer_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<";\n";
			}
		}
		//输出边
		for (iter_1=actor->outFlatNodes.begin();iter_1!=actor->outFlatNodes.end();iter_1++)
		{	
			if((actor->GPUPart != nGpu_ && (*iter_1)->GPUPart == nGpu_) || (actor->GPUPart != nGpu_ && (*iter_1)->GPUPart != nGpu_ && Maflp->findPartitionNumForFlatNode(actor)!=Maflp->findPartitionNumForFlatNode(*iter_1)))
			{
				buf<<"extern cl::Buffer Outbuffer_"<<actor->name<<"_"<<(*iter_1)->name<<"_"<<i<<";\n";
				buf<<"extern cl::Buffer cmPinnedBufOut_"<<actor->name<<"_"<<(*iter_1)->name<<"_init_"<<i<<";\n";
				buf<<"extern cl::Buffer cmDevBufOut_"<<actor->name<<"_"<<(*iter_1)->name<<"_init_"<<i<<";\n";
				buf<<"extern mystruct_"<<ReturnTypeOfEdge(actor,*iter_1)<<" *Outst_"<<actor->name<<"_"<<(*iter_1)->name<<"_init_"<<i<<";\n";
				buf<<"extern cl::Buffer cmPinnedBufOut_"<<actor->name<<"_"<<(*iter_1)->name<<"_steady_"<<i<<";\n";
				buf<<"extern cl::Buffer cmDevBufOut_"<<actor->name<<"_"<<(*iter_1)->name<<"_steady_"<<i<<";\n";
				buf<<"extern mystruct_"<<ReturnTypeOfEdge(actor,*iter_1)<<" *Outst_"<<actor->name<<"_"<<(*iter_1)->name<<"_steady_"<<i<<";\n";
			}
		}
		for (iter_1=actor->outFlatNodes.begin();iter_1!=actor->outFlatNodes.end();iter_1++)
		{
			if(actor->GPUPart != nGpu_ && (*iter_1)->GPUPart != nGpu_ && Maflp->findPartitionNumForFlatNode(actor)==Maflp->findPartitionNumForFlatNode(*iter_1))
			{
				buf<<"extern cl::Buffer Outbuffer_"<<actor->name<<"_"<<(*iter_1)->name<<"_"<<i<<";\n";	
			}
		}
	}
}

void GPUCodeGenerate::CGdatatag(FlatNode *actor,stringstream &buf)
{
	vector<FlatNode*>::iterator iter,iter_1;
	buf<<"//读写的标志位\n";
	for (int i = 0; i < nGpu_; i++)
	{
		for (iter=actor->inFlatNodes.begin();iter!=actor->inFlatNodes.end();iter++)
		{
			//if (!DetectiveFilterState(actor)&&(Maflp->UporDownStatelessNode(actor)!=3)&&(Maflp->UporDownStatelessNode(*iter)!=3)&&!DetectiveFilterState(*iter)&&Maflp->findPartitionNumForFlatNode(actor)==Maflp->findPartitionNumForFlatNode(*iter))
			if(actor->GPUPart != nGpu_ && (*iter)->GPUPart != nGpu_ && Maflp->findPartitionNumForFlatNode(actor)==Maflp->findPartitionNumForFlatNode(*iter))
			{
				buf<<"\tint readtag_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<";\n";
			}
			//if ((!DetectiveFilterState(actor)&&(Maflp->UporDownStatelessNode(actor)!=3)&&(DetectiveFilterState(*iter)||(Maflp->UporDownStatelessNode(*iter)==3)))||(!DetectiveFilterState(actor)&&(Maflp->UporDownStatelessNode(actor)!=3)&&(Maflp->UporDownStatelessNode(*iter)!=3)&&!DetectiveFilterState(*iter)&&Maflp->findPartitionNumForFlatNode(actor)!=Maflp->findPartitionNumForFlatNode(*iter)))
			if((actor->GPUPart != nGpu_ && (*iter)->GPUPart == nGpu_) || (actor->GPUPart != nGpu_ && (*iter)->GPUPart != nGpu_ && Maflp->findPartitionNumForFlatNode(actor)!=Maflp->findPartitionNumForFlatNode(*iter)))
			{
				buf<<"\tint readtag_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<";\n";
				buf<<"\tint writedatatag_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<";\n";
			}
		}
		for (iter_1=actor->outFlatNodes.begin();iter_1!=actor->outFlatNodes.end();iter_1++)
		{
			//if (!DetectiveFilterState(actor)&&(Maflp->UporDownStatelessNode(actor)!=3)&&(Maflp->UporDownStatelessNode(*iter_1)!=3)&&!DetectiveFilterState(*iter_1)&&Maflp->findPartitionNumForFlatNode(actor)==Maflp->findPartitionNumForFlatNode(*iter_1))
			if(actor->GPUPart != nGpu_ && (*iter_1)->GPUPart != nGpu_ && Maflp->findPartitionNumForFlatNode(actor)==Maflp->findPartitionNumForFlatNode(*iter_1))
			{
				buf<<"\tint writetag_"<<(actor)->name<<"_"<<(*iter_1)->name<<"_"<<i<<";\n";
			}
			if((actor->GPUPart != nGpu_ && (*iter_1)->GPUPart == nGpu_) || (actor->GPUPart != nGpu_ && (*iter_1)->GPUPart != nGpu_ && Maflp->findPartitionNumForFlatNode(actor)!=Maflp->findPartitionNumForFlatNode(*iter_1)))
			{
				buf<<"\tint writetag_"<<(actor)->name<<"_"<<(*iter_1)->name<<"_"<<i<<";\n";
				buf<<"\tint readdatatag_"<<(actor)->name<<"_"<<(*iter_1)->name<<"_"<<i<<";\n";
			}
		}
	}
	
}

void GPUCodeGenerate::CGEdgeParam(FlatNode *actor,stringstream &buf,vector<string> inEdgeName,vector<string> outEdgeName)
{
	buf<<"\t//edge param\n\t";
	for(int index =0;index< actor->inFlatNodes.size();index++)
	{
		if (DataDependence(actor->inFlatNodes[index], actor) && (actor->GPUPart == nGpu_ || (actor->GPUPart != nGpu_ && actor->inFlatNodes[index]->GPUPart == nGpu_)))
		//if (DataDependence(actor->inFlatNodes[index], actor) &&IsDownBorder(actor))
		{
			string edgetype = ReturnTypeOfEdge(actor->inFlatNodes[index], actor);
			buf << "Consumer<" << edgetype << ">" << inEdgeName[index] << ";\n";
		}
	}
	for(int index =0;index< actor->outFlatNodes.size();index++)
	{
		if (DataDependence(actor, actor->outFlatNodes[index]) && (actor->outFlatNodes[index]->GPUPart == nGpu_ || (actor->outFlatNodes[index]->GPUPart != nGpu_ && actor->GPUPart == nGpu_)))
		//if (DataDependence(actor, actor->outFlatNodes[index]) && IsDownBorder(actor))
		{
			string edgetype = ReturnTypeOfEdge(actor, actor->outFlatNodes[index]);
			buf << "Producer<" << edgetype << ">" << outEdgeName[index] << ";\n";
		}
	}
}

void GPUCodeGenerate::CGactorsStmts(stringstream &buf, list<Node *> *stmts)
{
    if (stmts != NULL)
    {
        for (auto it : *stmts)
        {
            string str = it->toString()+";";
            /*解析等号类似int i=0,j=1形式变成int i,j的形式,变量定义不能初始化*/
            string temp = "";
            bool flag = 1;
            for (auto c : str)
            {
                if (c == ',' || c == ';')
                    flag = true;
                if (c != '=' && flag)
                    temp += c;
                else
                {
                    flag = false;
                }
            }
            buf << "\t" << temp << "\n";
        }
    }
}

void GPUCodeGenerate::CGactorsinitVarAndState(stringstream &buf, list<Node *> *stmts)
{
    buf << "void initVarAndState() {\n";
    if (stmts != NULL)
    {
        for (auto it : *stmts)
        {
            string str = it->toString()+";";
            vector<string> svec;
            /*解析逗号和等号,类似int i=0,j;形式变成i=0;的形式,初始化stmts*/
            string temp = "";
            for (auto c : str)
            {
                if (c != ',' && c != ';')
                    temp += c;
                else
                {
                    //过滤不含等号的字符串
                    temp += ';';
                    if (temp.find('=') != string::npos)
                        svec.push_back(temp);
                    temp = "";
                }
            }
            if (temp != "" && temp.find('=') != string::npos)
                svec.push_back(temp);
            //解析空格 去除类型 eg int i=1;==> i=1
            for (auto it : svec)
            {
                if (it.find(' ') != string::npos)
                {
                    it = it.substr(it.find(' '), it.size() - 1);
                    buf << "\t\t" << it << "\n";
                }
                else
                    buf << "\t\t" << it << "\n";
            }
        }
    }
    buf << "\t}\n";
}
void GPUCodeGenerate::CGactorsInit(stringstream &buf, Node *init)
{
    buf << "\tvoid init(){ \n";
    if (init != NULL)
        buf << init->toString();
    buf << "\t}\n";
}

void GPUCodeGenerate::CGpopToken(FlatNode *actor,stringstream &buf,vector<string> inEdgeName)
{
	vector<int>::iterator iter=actor->inPopWeights.begin();
	vector<FlatNode*>inflatnodes = actor->inFlatNodes;
	map<FlatNode*,FlatNode*>tempinedgename;
	tempinedgename.clear();
	if(ExistDependence(actor))
	{
		buf << "\t// popToken\n";
		buf << "\tvoid popToken() {";
	}

	for(int index = 0;index< actor->inFlatNodes.size();index++)
	{
		string inputString = inEdgeName[index];
		if(DataDependence(inflatnodes[index],actor))
			buf<<"\n\t"<<inputString<<".updatehead"<<"("<<(*iter)<<");\n";
		tempinedgename.insert(make_pair(inflatnodes[index],actor));
		BufferNameOfEdge.insert(make_pair(tempinedgename,inputString));
		iter++;
	}
	if (ExistDependence(actor))
	{
		buf << "\t}\n"; // popToken方法结束
	}
}
void GPUCodeGenerate::CGpushToken(FlatNode *actor,stringstream &buf, vector<string> outEdgeName)
{

	vector<int>::iterator iter=actor->outPushWeights.begin();
	vector<FlatNode*>outflatnodes = actor->outFlatNodes;
	vector<FlatNode*>::iterator iter_outflatnodes = outflatnodes.begin();
	map<FlatNode*,FlatNode*>tempoutedgename;
	tempoutedgename.clear();
	bool flag = false;
	for (;iter_outflatnodes != outflatnodes.end(); ++iter_outflatnodes)
	{
		if(ExistDependence(*iter_outflatnodes))
		{
			flag = true;
			break;
		}
	}
	if (flag)
	{
		buf << "\t// pushToken\n";
		buf << "\tvoid pushToken() {";
	}
	iter_outflatnodes = outflatnodes.begin();
	for(int index = 0;index< outflatnodes.size();index++)
	{
		string outputString = outEdgeName[index];
		if(DataDependence(actor,*iter_outflatnodes) && flag)
			buf<<"\n\t"<<outputString<<".updatetail"<<"("<<(*iter)<<");\n";
		tempoutedgename.insert(make_pair(actor,*iter_outflatnodes));
		BufferNameOfEdge.insert(make_pair(tempoutedgename,outputString));
		iter++;
		iter_outflatnodes++;
	}
	if (flag)
	{
		//buf << pushingBuf;
		buf << "\t}\n"; // pushToken方法结束
	}
}

void GPUCodeGenerate::CGpopTokenForGPU(FlatNode *actor,stringstream &buf, vector<string> inEdgeName)
{
	if (IsUpBorder(actor) && ExistDependence(actor))
	{
		buf << "\t// popToken\n";
		buf << "\tvoid popToken(int count) {\n\t";
		buf<<"if(count == initScheduleCount)\n\t{";
	}
	stringstream steadybuf;
	vector<int>::iterator iter=actor->inPopWeights.begin();
	vector<FlatNode*>inflatnodes = actor->inFlatNodes;
	vector<FlatNode*>::iterator iter_inflatnodes = inflatnodes.begin();
	map<FlatNode*,FlatNode*>tempinedgename;
	tempinedgename.clear();

	for(int index = 0;index< inflatnodes.size();index++)
	{
		string inputString = inEdgeName[index];
		if (IsUpBorder(actor) && ExistDependence(actor))
		{
			buf<<"\n\t\t"<<inputString<<".updatehead"<<"("<<sssg_->GetInitCount(*iter_inflatnodes)*ReturnPushWeight(*iter_inflatnodes,actor)<<");\n";
			steadybuf<<"\n\t\t"<<inputString<<".updatehead"<<"("<<sssg_->GetSteadyCount(*iter_inflatnodes)*Maflp->MultiNum2FlatNode[actor]*ReturnPushWeight(*iter_inflatnodes,actor)<<"/com_num);\n";
		}
		tempinedgename.insert(make_pair(*iter_inflatnodes,actor));
		BufferNameOfEdge.insert(make_pair(tempinedgename,inputString));
		iter++;
		iter_inflatnodes++;
	}
	if (IsUpBorder(actor) && ExistDependence(actor))
	{
		//buf << popingBuf;
		buf<<"\n\t}";
		buf<<"\n\telse\n\t{";
		buf<<steadybuf.str();
		buf<<"\n\t}\n";
		buf << "\t}\n"; // popToken方法结束
	}
}
void GPUCodeGenerate::CGpushTokenForGPU(FlatNode *actor,stringstream &buf, vector<string> outEdgeName)
{
	stringstream steadybuf;
	//List *outputList = actor->contents->decl->u.decl.type->u.operdcl.outputs;
	//ListMarker output_maker;
	//Node *outputNode = NULL;
	//IterateList(&output_maker,outputList);
	vector<int>::iterator iter=actor->outPushWeights.begin();
	vector<FlatNode*>outflatnodes = actor->outFlatNodes;
	vector<FlatNode*>::iterator iter_outflatnodes = outflatnodes.begin();
	map<FlatNode*,FlatNode*>tempoutedgename;
	tempoutedgename.clear();
	bool flag = false;
	for (; iter_outflatnodes != outflatnodes.end(); ++iter_outflatnodes)
	{
		if(ExistDependence(*iter_outflatnodes))
		{
			flag = true;
			break;
		}
	}
	if (IsDownBorder(actor) && flag)
	{
		buf << "\t// pushToken\n";
		buf << "\tvoid pushToken(int count) {\n\t";
		buf<<"if(count == initScheduleCount)\n\t{";
	}
	iter_outflatnodes = outflatnodes.begin();
	for(int index = 0;index< outflatnodes.size();index++)
	{
		string outputString = outEdgeName[index];
		if (IsDownBorder(actor) && flag && ((*iter_outflatnodes)->GPUPart == nGpu_ || ((*iter_outflatnodes)->GPUPart != nGpu_ && actor->GPUPart == nGpu_)))
		{
			buf<<"\n\t"<<outputString<<".updatetail"<<"("<<(*iter)*sssg_->GetInitCount(actor)<<");\n";
			steadybuf<<"\n\t"<<outputString<<".updatetail"<<"("<<(*iter)*sssg_->GetSteadyCount(actor)*Maflp->MultiNum2FlatNode[actor]<<"/com_num);\n";
		}
		tempoutedgename.insert(make_pair(actor,*iter_outflatnodes));
		BufferNameOfEdge.insert(make_pair(tempoutedgename,outputString));
		iter++;
		iter_outflatnodes++;
	}
	if (IsDownBorder(actor) && flag)
	{
		//buf << pushingBuf;
		buf<<"\n\t}";
		buf<<"\n\telse\n\t{";
		buf<<steadybuf.str();
		buf<<"\n\t}\n";
		buf << "\t}\n"; // pushToken方法结束
	}
}
//判断actora和actorb是否存在数据依赖关系
//其中actora是上游节点，actorb是下游节点
//如果actorb带有peek，返回true；
bool GPUCodeGenerate::DataDependence(FlatNode *actora,FlatNode *actorb)//a->b
{
	bool flag = false;
	vector<FlatNode *>::iterator iter;
	int index = 0;
	for (iter = actorb->inFlatNodes.begin();iter != actorb->inFlatNodes.end(); ++iter)
	{
		if ((*iter) == actora)
		{
			if(actorb->inPeekWeights[index] > actorb->inPopWeights[index])
			{
				flag = true;
				break;
			}
		}
		++index;
	}
	return flag;
}
bool GPUCodeGenerate::ExistDependence(FlatNode *actor)
{
	bool flag = false;
	int index = 0;
	for (;index < actor->inFlatNodes.size(); ++index)
	{
		if(actor->inPeekWeights[index] > actor->inPopWeights[index])
		{
			flag = true;
			break;
		}
	}
	return flag;
}
bool GPUCodeGenerate::IsUpBorder(FlatNode *actor)
{
	bool flag = false;   //cwb 如果是cpu与gpu的边界结点
	vector<FlatNode *>::iterator iter1;
	if (actor != sssg_->GetFlatNodes()[0])
	{
		for (iter1 = actor->inFlatNodes.begin(); iter1 != actor->inFlatNodes.end(); ++iter1)
		{
			if(actor->GPUPart != (*iter1)->GPUPart)
			{
				flag = true;
				break;
			}
		}
	}
	return flag;
}

bool GPUCodeGenerate::IsDownBorder(FlatNode *actor)
{
	bool flag = false;   //cwb 如果是cpu与gpu的边界结点
	vector<FlatNode *>::iterator iter1;
	if (actor != sssg_->GetFlatNodes()[sssg_->flatNodes.size()-1])
	{
		for (iter1 = actor->outFlatNodes.begin(); iter1 != actor->outFlatNodes.end(); ++iter1)
		{
			if(actor->GPUPart != (*iter1)->GPUPart)
			{
				flag = true;
				break;
			}
		}
	}
	return flag;
}


string GPUCodeGenerate::ReturnTypeOfEdge(FlatNode *actorA,FlatNode *actorB)
{
	/* 2019 修改 
	map<FlatNode *,FlatNode *>searchmap;
	searchmap.insert(make_pair(actorA,actorB));
	map<map<FlatNode *,FlatNode *>,map<string,string>>::iterator iter = edge2nameandtype.find(searchmap);
	map<string,string>::iterator iter_str;
	iter_str = iter->second.begin();
	return iter_str->first;*/
	string edgename = (actorA)->name + "_" + (actorB)->name;
	return EdgeNameToDefinetype[edgename];

}
string GPUCodeGenerate::ReturnNameOfEdge(FlatNode *actorA,FlatNode *actorB)
{
	map<FlatNode *,FlatNode *>searchmap;
	searchmap.insert(make_pair(actorA,actorB));
	map<map<FlatNode *,FlatNode *>,map<string,string>>::iterator iter = edge2nameandtype.find(searchmap);
	map<string,string>::iterator iter_str;
	iter_str = iter->second.begin();
	return iter_str->second;
}
string GPUCodeGenerate::ReturnTypeOfEdgestr(string typestr)
{
	map<string,map<string,string>>::iterator iter = edgestrnameandtype.find(typestr);
	map<string,string>::iterator iter_str;
	iter_str = iter->second.begin();
	return iter_str->first;
}
string GPUCodeGenerate::ReturnNameOfEdgestr(string namestr)
{
	map<string,map<string,string>>::iterator iter = edgestrnameandtype.find(namestr);
	map<string,string>::iterator iter_str;
	iter_str = iter->second.begin();
	return iter_str->second;
}

int GPUCodeGenerate::ReturnPushWeight(FlatNode *actorA,FlatNode *actorB)
{
	vector<FlatNode*>::iterator iter_actor;
	vector<int>PushWeight = actorA->outPushWeights;
	vector<int>::iterator iter_push = PushWeight.begin();
	for (iter_actor = actorA->outFlatNodes.begin();iter_actor != actorA->outFlatNodes.end();iter_actor++)
	{
		if ((*iter_actor)->name == (actorB)->name)
		{
			return *iter_push;
		}
		iter_push++;
	}
}

int GPUCodeGenerate::ReturnPeekWeight(FlatNode *actorA,FlatNode *actorB)
{
	vector<FlatNode*>::iterator iter_actor;
	vector<int>PeekWeight = actorB->inPeekWeights;
	vector<int>::iterator iter_peek = PeekWeight.begin();
	for (iter_actor = actorB->inFlatNodes.begin();iter_actor != actorB->inFlatNodes.end();iter_actor++)
	{
		if ((*iter_actor)->name == (actorA)->name)
		{
			return *iter_peek;
		}
		iter_peek++;
	}
}

void GPUCodeGenerate::CGdataGetforGPU(FlatNode *actor,stringstream &buf,vector<string> inEdgeName)
{
	for (int i = 0; i < nGpu_; i++)
	{
		buf<<"\tvoid dataget_"<<i<<"(int count,";
		vector<FlatNode*>::iterator iter;
		vector<string>InPutString;
		vector<string>::iterator iter_instring;
		for(int inflatnodeindex=0;inflatnodeindex < actor->inFlatNodes.size();inflatnodeindex++)
		{
			string inputString =inEdgeName[inflatnodeindex];
			if(IsDownBorder(actor->inFlatNodes[inflatnodeindex]) && !DataDependence(actor->inFlatNodes[inflatnodeindex], actor))
			{
				string edgetype = ReturnTypeOfEdge(actor->inFlatNodes[inflatnodeindex],actor);
				//buf <<edgetype<< " *" << inputString << ",";
				string edgename = actor->inFlatNodes[inflatnodeindex]->name + "_" + actor->name;
				buf << edgetype << " *" << ReturnNameOfTheEdge(edgename) << ",";
				
			}
			InPutString.push_back(inputString);
		}
		iter_instring = InPutString.begin();
		vector<int>::iterator iter_inpopweight = actor->inPopWeights.begin();
		multimap<string,string>::iterator iterxy=mapedge2xy2.begin();
		pair<multimap<string,string>::iterator,multimap<string,string>::iterator>pos;

		buf<<"void* p = NULL)\n\t{\n";
		buf<<"\t\tif(count == initScheduleCount)\n";
		buf<<"\t\t{\n";
		buf<<"\t\t\tif(count){\n";
		for (iter=actor->inFlatNodes.begin();iter!=actor->inFlatNodes.end();iter++)
		{
			int argnum = 0;
			if((actor->GPUPart != nGpu_ && (*iter)->GPUPart == nGpu_))
			{
				int queuenum = i + 1;
				pos = mapedge2xy2.equal_range(iterxy->first);
				string searchstring = (*iter)->name+"_"+actor->name;
				buf<<"\t\t\t\tqueue_"<<queuenum<<"_0.enqueueWriteBuffer(Inbuffer_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<", CL_FALSE, 0, "<<sssg_->GetInitCount(*iter)<<"*"<<ReturnPushWeight(*iter,actor)<<"*sizeof(mystruct_"<<ReturnTypeOfEdge(*iter,actor)<<"),&"<<ReturnNameOfTheEdge(searchstring)<<"[0], NULL,NULL);\n";

				buf<<"\t\t\t\twritedatatag_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<" += "<<sssg_->GetInitCount(*iter)<<"*"<<ReturnPushWeight(*iter,actor)<<";\n";
				//buf<<"\t\t\t}\n";
			}
			iter_inpopweight++;
			iter_instring++;
		}
		buf<<"\t\t\t}\n";
		buf<<"\t\t}\n";
		iter_inpopweight = actor->inPopWeights.begin();
		iterxy=mapedge2xy2.begin();
		iter_instring = InPutString.begin();
		buf<<"\t\telse if(count == steadyScheduleCount)\n";
		buf<<"\t\t{\n";
		buf<<"\t\t\tif(count){\n";
		for (iter=actor->inFlatNodes.begin();iter!=actor->inFlatNodes.end();iter++)
		{
			int argnum = 0;
			if((actor->GPUPart != nGpu_ && (*iter)->GPUPart == nGpu_) || (actor->GPUPart != nGpu_ && (*iter)->GPUPart != nGpu_ && Maflp->findPartitionNumForFlatNode(actor)!=Maflp->findPartitionNumForFlatNode(*iter)))
			{
				int queuenum = i + 1;
				int stagemins = pSa->FindStage(actor) - pSa->FindStage(*iter);
				pos = mapedge2xy2.equal_range(iterxy->first);
				string searchstring = (*iter)->name+"_"+actor->name;
				buf<<"\t\t\t\tif(writedatatag_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<"+"<<sssg_->GetSteadyCount(*iter)*Maflp->MultiNum2FlatNode[actor]*ReturnPushWeight(*iter,actor)<<" > ";
				buf<<(sssg_->GetInitCount(*iter)+sssg_->GetSteadyCount(*iter)*Maflp->MultiNum2FlatNode[actor]*stagemins)*ReturnPushWeight(*iter,actor)<<")\n";
				buf<<"\t\t\t\t{\n";
				buf<<"\t\t\t\t\tqueue_"<<queuenum<<"_0.enqueueWriteBuffer(Inbuffer_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<", CL_FALSE,writedatatag_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<"*sizeof(mystruct_"<<ReturnTypeOfEdge(*iter,actor)<<"), ("<<(sssg_->GetInitCount(*iter)+sssg_->GetSteadyCount(*iter)*Maflp->MultiNum2FlatNode[actor]*stagemins)*ReturnPushWeight(*iter,actor)<<"-writedatatag_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<")*sizeof(mystruct_"<<ReturnTypeOfEdge(*iter,actor)<<"),&"<<ReturnNameOfTheEdge(searchstring)<<"[0], NULL,NULL);\n";
				buf<<"\t\t\t\t\tqueue_"<<queuenum<<"_0.enqueueWriteBuffer(Inbuffer_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<", CL_FALSE,0, ("<<sssg_->GetSteadyCount(*iter)*Maflp->MultiNum2FlatNode[actor]*ReturnPushWeight(*iter,actor)<<"-("<<(sssg_->GetInitCount(*iter)+sssg_->GetSteadyCount(*iter)*Maflp->MultiNum2FlatNode[actor]*stagemins)*ReturnPushWeight(*iter,actor)<<"-writedatatag_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<"))*sizeof(mystruct_"<<ReturnTypeOfEdge(*iter,actor)<<"),&"<<ReturnNameOfTheEdge(searchstring)<<"[0+"<<(sssg_->GetInitCount(*iter)+sssg_->GetSteadyCount(*iter)*Maflp->MultiNum2FlatNode[actor]*stagemins)*ReturnPushWeight(*iter,actor)<<"-writedatatag_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<"], NULL,NULL);\n";
				buf<<"\t\t\t\t\twritedatatag_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<" = "<<sssg_->GetSteadyCount(*iter)*Maflp->MultiNum2FlatNode[actor]*ReturnPushWeight(*iter,actor)<<"-("<<(sssg_->GetInitCount(*iter)+sssg_->GetSteadyCount(*iter)*Maflp->MultiNum2FlatNode[actor]*stagemins)*ReturnPushWeight(*iter,actor)<<"-writedatatag_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<");";
				buf<<"\n\t\t\t\t}\n";
				buf<<"\t\t\t\telse{\n";
				buf<<"\t\t\t\t\tqueue_"<<queuenum<<"_0.enqueueWriteBuffer(Inbuffer_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<", CL_FALSE,writedatatag_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<"*sizeof(mystruct_"<<ReturnTypeOfEdge(*iter,actor)<<"), "<<sssg_->GetSteadyCount(*iter)*Maflp->MultiNum2FlatNode[actor]*ReturnPushWeight(*iter,actor)<<"*sizeof(mystruct_"<<ReturnTypeOfEdge(*iter,actor)<<"),&"<<ReturnNameOfTheEdge(searchstring)<<"[0], NULL,NULL);\n";
				buf<<"\t\t\t\t\twritedatatag_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<" += "<<sssg_->GetSteadyCount(*iter)*Maflp->MultiNum2FlatNode[actor]<<"*"<<ReturnPushWeight(*iter,actor)<<";\n";
				//buf<<"\t\t\t\t\tif(writedatatag_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<" == "<<(sssg_->GetInitCount(*iter)+sssg_->GetSteadyCount(*iter)*Maflp->MultiNum2FlatNode[actor]*stagemins)*ReturnPushWeight(*iter,actor)<<")\n";
				buf<<"\t\t\t\t\twritedatatag_"<<(*iter)->name<<"_"<<actor->name<<"_"<<i<<" %= "<<(sssg_->GetInitCount(*iter)+sssg_->GetSteadyCount(*iter)*Maflp->MultiNum2FlatNode[actor]*stagemins)*ReturnPushWeight(*iter,actor)<<";\n";
				buf<<"\t\t\t\t}\n";
			}
			iter_instring++;
			iter_inpopweight++;
		}
		buf<<"\t\t\t}\n";
		buf<<"\t\t}\n";
		if(ExistDependence(actor))
			buf<<"\t\tpopToken(count);\n";
		buf<<"\t}\n";
	}
	
}
void GPUCodeGenerate::CGdataSendforGPU(FlatNode *actor,stringstream &buf,vector<string> outEdgeName)
{
	for (int i = 0; i < nGpu_; i++)
	{
		buf<<"\tvoid datasend_"<<i<<"(int count,";
		vector<FlatNode*>::iterator iter;
		vector<string>OutPutString;
		vector<string>::iterator iter_outstring;

		for(int outflatnodeindex=0; outflatnodeindex < actor->outFlatNodes.size();outflatnodeindex++)
		{
			string outputString = outEdgeName[outflatnodeindex];
			if (IsUpBorder(actor->outFlatNodes[outflatnodeindex]) && !DataDependence(actor, actor->outFlatNodes[outflatnodeindex]))
			{
				string edgetype = ReturnTypeOfEdge(actor, actor->outFlatNodes[outflatnodeindex]);
				buf << edgetype << " *" << outputString << ",";
			}
			OutPutString.push_back(outputString);
		}
		iter_outstring = OutPutString.begin();
		vector<int>::iterator iter_outpushweight = actor->outPushWeights.begin();
		multimap<string,string>::iterator iterxy=mapedge2xy2.begin();
		pair<multimap<string,string>::iterator,multimap<string,string>::iterator>pos;

		buf<<"void* p = NULL)\n\t{\n";
		buf<<"\t\tif(count == initScheduleCount)\n";
		buf<<"\t\t{\n";
		buf<<"\t\t\tif(count){\n";
		iter_outpushweight = actor->outPushWeights.begin();
		for (iter=actor->outFlatNodes.begin();iter!=actor->outFlatNodes.end();iter++)
		{
			int argnum = 0;
			if((actor->GPUPart != nGpu_ && (*iter)->GPUPart == nGpu_))
			{
				int queuenum = i + 1;
				string searchstring = actor->name+"_"+(*iter)->name;
				buf<<"\t\t\t\tqueue_"<<queuenum<<"_0.enqueueReadBuffer(Outbuffer_"<<actor->name<<"_"<<(*iter)->name<<"_"<<i<<",CL_FALSE,0,"<<sssg_->GetInitCount(*iter)*(*iter_outpushweight)<<"*sizeof(mystruct_"<<ReturnTypeOfEdge(actor,*iter)<<"),&"<<ReturnNameOfTheEdge(searchstring)<<"[0],NULL,NULL);\n";
				buf<<"\t\t\t\treaddatatag_"<<actor->name<<"_"<<(*iter)->name<<"_"<<i<<" += "<<sssg_->GetInitCount(*iter)*(*iter_outpushweight)<<";\n";
			}
			iter_outpushweight++;
		}
		buf<<"\t\t\t}\n";
		buf<<"\t\t}\n";
		iter_outstring = OutPutString.begin();
		iter_outpushweight = actor->outPushWeights.begin();
		iterxy=mapedge2xy2.begin();
		buf<<"\t\telse\n";
		buf<<"\t\t{\n";
		for (iter=actor->outFlatNodes.begin();iter!=actor->outFlatNodes.end();iter++)
		{
			int argnum = 0;
			if((actor->GPUPart != nGpu_ && (*iter)->GPUPart == nGpu_))
			{
				int queuenum = i + 1;
				string searchstring = actor->name+"_"+(*iter)->name;
				int stageminus = pSa->FindStage(*iter)-pSa->FindStage(actor);
				buf<<"\t\t\tif(readdatatag_"<<actor->name<<"_"<<(*iter)->name<<"_"<<i<<"+"<<sssg_->GetSteadyCount(actor)*Maflp->MultiNum2FlatNode[actor]*ReturnPushWeight(actor,*iter)<<" > "<<(sssg_->GetInitCount(actor)+sssg_->GetSteadyCount(actor)*Maflp->MultiNum2FlatNode[actor]*stageminus)*ReturnPushWeight(actor,*iter)<<")\n";
				buf<<"\t\t\t{\n";
				buf<<"\t\t\t\tqueue_"<<queuenum<<"_0.enqueueReadBuffer(Outbuffer_"<<actor->name<<"_"<<(*iter)->name<<"_"<<i<<",CL_FALSE,readdatatag_"<<actor->name<<"_"<<(*iter)->name<<"_"<<i<<"*sizeof(mystruct_"<<ReturnTypeOfEdge(actor,*iter)<<"),("<<(sssg_->GetInitCount(actor)+sssg_->GetSteadyCount(actor)*Maflp->MultiNum2FlatNode[actor]*stageminus)*ReturnPushWeight(actor,*iter)<<"-readdatatag_"<<actor->name<<"_"<<(*iter)->name<<"_"<<i<<")*sizeof(mystruct_"<<ReturnTypeOfEdge(actor,*iter)<<"),&"<<ReturnNameOfTheEdge(searchstring)<<"[0],NULL,NULL);\n";
				buf<<"\t\t\t\tqueue_"<<queuenum<<"_0.enqueueReadBuffer(Outbuffer_"<<actor->name<<"_"<<(*iter)->name<<"_"<<i<<",CL_FALSE,0,("<<sssg_->GetSteadyCount(actor)*Maflp->MultiNum2FlatNode[actor]*ReturnPushWeight(actor,*iter)<<"-("<<(sssg_->GetInitCount(actor)+sssg_->GetSteadyCount(actor)*Maflp->MultiNum2FlatNode[actor]*stageminus)*ReturnPushWeight(actor,*iter)<<"-readdatatag_"<<actor->name<<"_"<<(*iter)->name<<"_"<<i<<"))*sizeof(mystruct_"<<ReturnTypeOfEdge(actor,*iter)<<"),&"<<ReturnNameOfTheEdge(searchstring)<<"[0+"<<(sssg_->GetInitCount(actor)+sssg_->GetSteadyCount(actor)*Maflp->MultiNum2FlatNode[actor]*stageminus)*ReturnPushWeight(actor,*iter)<<"-readdatatag_"<<actor->name<<"_"<<(*iter)->name<<"_"<<i<<"],NULL,NULL);\n";
				buf<<"\t\t\t\treaddatatag_"<<actor->name<<"_"<<(*iter)->name<<"_"<<i<<" = "<<sssg_->GetSteadyCount(actor)*Maflp->MultiNum2FlatNode[actor]*ReturnPushWeight(actor,*iter)<<"-("<<(sssg_->GetInitCount(actor)+sssg_->GetSteadyCount(actor)*Maflp->MultiNum2FlatNode[actor]*stageminus)*ReturnPushWeight(actor,*iter)<<"-readdatatag_"<<actor->name<<"_"<<(*iter)->name<<"_"<<i<<");\n";
				buf<<"\t\t\t}\n";
				buf<<"\t\t\telse{\n";
				buf<<"\t\t\t\tqueue_"<<queuenum<<"_0.enqueueReadBuffer(Outbuffer_"<<actor->name<<"_"<<(*iter)->name<<"_"<<i<<",CL_FALSE,readdatatag_"<<actor->name<<"_"<<(*iter)->name<<"_"<<i<<"*sizeof(mystruct_"<<ReturnTypeOfEdge(actor,*iter)<<"),"<<sssg_->GetSteadyCount(actor)*Maflp->MultiNum2FlatNode[actor]*ReturnPushWeight(actor,*iter)<<"*sizeof(mystruct_"<<ReturnTypeOfEdge(actor,*iter)<<"),&"<<ReturnNameOfTheEdge(searchstring)<<"[0],NULL,NULL);\n";
				buf<<"\t\t\t\treaddatatag_"<<actor->name<<"_"<<(*iter)->name<<"_"<<i<<" += "<<sssg_->GetSteadyCount(actor)*Maflp->MultiNum2FlatNode[actor]*ReturnPushWeight(actor,*iter)<<";\n";
				buf<<"\t\t\t\treaddatatag_"<<actor->name<<"_"<<(*iter)->name<<"_"<<i<<" %= "<<(sssg_->GetInitCount(actor)+sssg_->GetSteadyCount(actor)*Maflp->MultiNum2FlatNode[actor]*stageminus)*ReturnPushWeight(actor,*iter)<<";\n";
				buf<<"\t\t\t}\n";
			}
			iter_outpushweight++;
		}
		buf<<"\t\t}\n";
		bool pushflag = false;
		for (int outindex = 0; outindex < actor->outFlatNodes.size(); ++outindex)
		{
			if (DataDependence(actor,actor->outFlatNodes[outindex]))
			{
				pushflag = true;
				break;
			}
		}
		if(pushflag)
			buf<<"\t\t\tpushToken(count);\n";
		buf<<"\t}\n";
	}
}

void GPUCodeGenerate::CGrunInitScheduleWork(FlatNode *actor,stringstream &buf,vector<string> inEdgeName,vector<string> outEdgeName)
{
	buf <<"\t// runInitScheduleWork\n";
	buf << "\tvoid runInitScheduleWork(";
	string inputString;
	string outputString;
	for(int index = 0; index < actor->inFlatNodes.size();index++)
	{
		inputString = inEdgeName[index];
		if (!DataDependence(actor->inFlatNodes[index], actor))
		{
			string edgetype = ReturnTypeOfEdge(actor->inFlatNodes[index], actor);
			buf << edgetype << " *" << inputString << ",";
		}

	}
	for(int index = 0; index < actor->outFlatNodes.size();index++)
	{
		outputString = outEdgeName[index];
		if (!DataDependence(actor, actor->outFlatNodes[index]))
		{
			string edgetype = ReturnTypeOfEdge(actor, actor->outFlatNodes[index]);
			buf << edgetype << " *" << outputString << ",";
		}
	}
	buf<<"void * p = NULL){\n";
	buf << "\t\tinitWork();\n"; // 调用initwork方法
	buf<<"\t\tfor(int i=0;i<initScheduleCount;i++)\n\t\t\twork(";
	for(int index = 0; index < actor->inFlatNodes.size();index++)
	{
		if(!DataDependence(actor->inFlatNodes[index],actor))
			buf<<inputString<<"+i*"<<actor->inPopWeights[index]<<",";
	}
	for(int index = 0; index < actor->outFlatNodes.size();index++)
	{
		if(!DataDependence(actor,actor->outFlatNodes[index]))
			buf<<outputString<<"+i*"<<actor->outPushWeights[index]<<",";
	}
	buf<<"NULL);\n";
	buf << "\t}\n"; // CGrunInitScheduleWork方法结束
}
void GPUCodeGenerate::CGrunInitScheduleWorkforGPU(FlatNode *actor,stringstream &buf)
{
	buf <<"\t// runInitScheduleWork\n";
	for (int i = 0; i < nGpu_; i++)
	{
		buf << "\tvoid runInitScheduleWork_"<<i<<"() {\n";
		buf << "\t\tinitWork();\n"; // 调用initwork方法
		buf<<"\t\tif(initScheduleCount)\n";
		buf<<"\t\t\twork_"<<i<<"("<<sssg_->GetInitCount(actor)<<");\n";
		buf << "\t}\n"; // CGrunInitScheduleWork方法结束
	}
}
void GPUCodeGenerate::CGrunSteadyScheduleWork(FlatNode *actor,stringstream &buf,vector<string> inEdgeName,vector<string> outEdgeName)
{
	buf <<"\t// CGrunSteadyScheduleWork\n";
	buf << "\tvoid runSteadyScheduleWork(";
	//multimap<FlatNode*,string>::iterator iter1,iter2;
	//List *inputList =actor->contents->decl->u.decl.type->u.operdcl.inputs;
	//List *outputList = actor->contents->decl->u.decl.type->u.operdcl.outputs;
	//ListMarker input_maker;
	//ListMarker output_maker;
	//Node *inputNode = NULL;
	//Node *outputNode = NULL;
	string inputString;
	string outputString;
	//iter1=mapActor2InEdge.find(actor);
	for(int index = 0; index < actor->inFlatNodes.size();index++)
	{
		inputString = inEdgeName[index];
		if (!DataDependence(actor->inFlatNodes[index], actor))
		{
			string edgetype = ReturnTypeOfEdge(actor->inFlatNodes[index], actor);
			buf << edgetype << " *" << inputString << ",";
		}
	}
	for(int index = 0; index < actor->outFlatNodes.size();index++)
	{
		outputString = outEdgeName[index];
		if (!DataDependence(actor, actor->outFlatNodes[index]))
		{
			string edgetype = ReturnTypeOfEdge(actor, actor->outFlatNodes[index]);
			buf << edgetype << " *" << outputString << ",";
		}
	}
	buf<<"void * p = NULL){\n";
	buf<<"\t\tfor(int i=0;i<steadyScheduleCount;i++)\n\t\t\twork(";

	for(int index = 0; index < actor->inFlatNodes.size();index++)
	{
		if(!DataDependence(actor->inFlatNodes[index],actor))
			buf<<inputString<<"+i*"<<actor->inPopWeights[index]<<",";
	}
	for(int index = 0; index < actor->outFlatNodes.size();index++)
	{
		if(!DataDependence(actor,actor->outFlatNodes[index]))
			buf<<outputString<<"+i*"<<actor->outPushWeights[index]<<",";
	}
	buf<<"NULL);\n";
	buf << "\t}\n"; // CGrunSteadyScheduleWork方法结束
}
void GPUCodeGenerate::CGrunSteadyScheduleWorkforGPU(FlatNode *actor,stringstream &buf)
{
	buf <<"\t// CGrunSteadyScheduleWork\n";
	for (int i = 0; i < nGpu_; i++)
	{
		buf << "\tvoid runSteadyScheduleWork_"<<i<<"() {\n";
		buf<<"\t\twork_"<<i<<"("<<sssg_->GetSteadyCount(actor)*Maflp->MultiNum2FlatNode[actor]<<");\n";
		buf << "\t}\n"; // CGrunSteadyScheduleWork方法结束
	}
}

string GPUCodeGenerate::ReturnNameOfTheEdge(string searchstring)//此处的参数形如A->name_B->name
{
	map<string,string>::iterator iter = edge2name.find(searchstring);
	return iter->second;
}

int GPUCodeGenerate::ReturnBufferSize(FlatNode *actora,FlatNode *actorb)//a->b
{
	vector<FlatNode *>::iterator iter_nodes;
	vector<int>::iterator iter_push = actora->outPushWeights.begin();
	int stageminus,size;
	for (iter_nodes = actora->outFlatNodes.begin();iter_nodes != actora->outFlatNodes.end();++iter_nodes)
	{
		if ((*iter_nodes)->name == actorb->name)
		{
			stageminus = pSa->FindStage(actorb) - pSa->FindStage(actora);
			if(actora->GPUPart != nGpu_ && actorb->GPUPart == nGpu_)
				size=(sssg_->GetInitCount(actora)+sssg_->GetSteadyCount(actora)*Maflp->MultiNum2FlatNode[actora]*stageminus)*(*iter_push);
			else if(actora->GPUPart == nGpu_ && actorb->GPUPart != nGpu_)
				size=(sssg_->GetInitCount(actora)+sssg_->GetSteadyCount(actora)*Maflp->MultiNum2FlatNode[actora]/nGpu_*stageminus)*(*iter_push);
			else
				size=(sssg_->GetInitCount(actora)+sssg_->GetSteadyCount(actora)*Maflp->MultiNum2FlatNode[actora]*(stageminus+1))*(*iter_push);
			break;
		}
		iter_push++;
	}
	return size;
}

int GPUCodeGenerate::ReturnNumBiger(int size)
{
	int num = 1;
	while (num < size)
	{
		num *= 2;
	}
	return num;
}
