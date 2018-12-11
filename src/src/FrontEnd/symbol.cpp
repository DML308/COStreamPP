#include "symbol.h"
int Level;
int current_version[MAX_SCOPE_DEPTH];
/* enterScope */
void EnterScope()
{
    Level++;
    if (Level == MAX_SCOPE_DEPTH) {
	    cout<<"Internal Error: out of nesting levels!\n";
	    exit(-1);
    }
    current_version[Level]++;
}

void ExitScope()
{
    if (Level == 0) {
		cout<<"missing '{' detected"<<endl;
    } 
	Level--;
}

void SymbolTable::InsertSymbol(idNode* node){
    auto iter=idTable.find(node->name);
    if(iter==idTable.end()){
        idTable[node->name].push_back(node);
    
    }else{
        /* 遍历节点 查找是否定义过，同level和version不能重新定义 */
        // 此处不同于LookupSymbol,所以代码未重用
        //idTable[node->name].push_back(node);
        for(auto it=idTable[node->name].begin();it!=idTable[node->name].end();it++){
            if((*it)->level==Level && (*it)->version==current_version[Level]){
                cout<<"idNode had been declared!";
                exit(-1);
            }
        }
        idTable[node->name].push_back(node);
    }
}

/* 必须查找上层作用域名 还未修改*/
bool SymbolTable::LookupSymbol(string  name){
    auto iter=idTable.find(name);
    if(iter==idTable.end())
        return false;
    for(auto it=idTable[name].begin();it!=idTable[name].end();it++){
        if((*it)->level==Level && (*it)->version==current_version[Level])
            return true;
    }
    return false;
}

idNode* SymbolTable::operator[](string str){
    auto iter=idTable.find(str);
    if(iter!=idTable.end()){
        for(auto it=idTable[str].begin();it!=idTable[str].end();it++){
        if((*it)->level==Level && (*it)->version==current_version[Level])
            return (*it);
        }
    }
    /*
    if(functionTable.find(str)!=functionTable.end())
        return functionTable[str];
    if(compositeTable.find(str)!=compositeTable.end())  
        return compositeTable[str];
    */
    return NULL;
}