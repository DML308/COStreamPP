#ifndef _HexPrinter_Filter_774_
#define _HexPrinter_Filter_774_
#include <string>
#include <iostream>
#include "Buffer.h"
#include "Consumer.h"
#include "Producer.h"
#include "Global.h"
#include "GlobalVar.h"
using namespace std;
class HexPrinter_Filter_774{
public:
	HexPrinter_Filter_774(Buffer<streamData>& In):In(In){
		steadyScheduleCount = 1;
		initScheduleCount = 0;
	}
	void runInitScheduleWork() {
		initVarAndState();
		init();
		for(int i=0;i<initScheduleCount;i++)
			work();
		In.resetHead();
	}
	void runSteadyScheduleWork() {
		for(int i=0;i<steadyScheduleCount;i++)
			work();
		In.resetHead();
	}
private:
	Consumer<streamData> In;
	int steadyScheduleCount;	//稳态时一次迭代的执行次数
	int initScheduleCount;
	int descriptor;
	int n;
	int bits;
	int bytes;
	void popToken() {
		In.updatehead(16);
	}
	void pushToken() {
	}
	void initVarAndState() {
	descriptor=2;
	n=64;
		 bits=n;
		 bytes=bits/4;
	}
	void init(){ 
	}
	void work(){ 
	{
		int i;
		if(PRINTINFO!=0)		{
		if(descriptor==PLAINTEXT)	cout<<"P: ";else	if(descriptor==USERKEY)	cout<<"K: ";else	if(descriptor==CIPHERTEXT)	cout<<"C: ";
	}
		for(i=bytes-1;i>=0;i--)			{
		int v=In[i].x;
		if(v<10)	cout<<v;else	if(v==10)	cout<<"A";else	if(v==11)	cout<<"B";else	if(v==12)	cout<<"C";else	if(v==13)	cout<<"D";else	if(v==14)	cout<<"E";else	if(v==15)	cout<<"F";else		{
		cout<<"ERROR: ";
		cout<<v<<endl;
	}
	}
		cout<<""<<endl;
	}
		pushToken();
		popToken();
	}
};
#endif