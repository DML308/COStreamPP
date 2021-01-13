#ifndef _Annoy_0_
#define _Annoy_0_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class Annoy_0{
public:
	Annoy_0(Buffer<streamData>& OutT):OutT(OutT){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		OutT.resetTail();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		OutT.resetTail();
	}
private:
	Producer<streamData> OutT;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	int vector;
	static int TEXT[34][2];
	void popToken() {
	}
	void pushToken() {
		OutT.updatetail(2);
	}
	void initVarAndState() {
	vector=7;
	}
	void init(){ 
	}
	void work(){ 
	{
		OutT[0].x=TEXT[vector][1];
		OutT[1].x=TEXT[vector][0];
	}
		pushToken();
		popToken();
	}
};
int Annoy_0::TEXT[34][2]={{0,0},{4294967295,4294967295},{268435456,1},{286331153,286331153},{286331153,286331153},{19088743,2309737967},{0,0},{19088743,2309737967},{27383504,964126530},{1557482664,1039095770},{38327352,116814194},{1363495768,769606666},{1123894320,1498906530},{94068232,1372525626},{123132128,2001166802},{1982141624,700401770},{1004343696,1228351490},{647323496,900685978},{374169152,1327977010},{1795517976,1973378250},{4970223,152526946},{1208826112,1860657906},{1131757768,1770994938},{120406944,1996968594},{50222455,2165829930},{496852048,418851010},{810889768,1836001626},{19088743,2309737967},{19088743,2309737967},{19088743,2309737967},{4294967295,4294967295},{0,0},{0,0},{4294967295,4294967295}};
#endif