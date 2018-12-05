#include "symbol.h"
int current_version[MAX_SCOPE_DEPTH];
int Level;
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

void SymbolTable::InsertSymbol(identifierNode* node,map<string,list<identifierNode*> > mp){
    if(mp.find(node->name)==mp.end()){
        mp[node->name].push_back(node);
    }else{
        /* 暂时保留 可能需要添加内容 */
         mp[node->name].push_back(node);
    }
    return ;
}

bool SymbolTable::LookupSymbol(identifierNode* node,map<string,list<identifierNode*> > mp){
    auto iter=mp.find(node->name);
    if(iter==mp.end())
        return false;
    for(auto it=mp[node->name].begin();it!=mp[node->name].end();it++){
        if((*it)->level==Level && (*it)->version==current_version[Level])
            return true;
    }
    return false;
}