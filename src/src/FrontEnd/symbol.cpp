#include "symbol.h"
int Level=0;
int current_version[MAX_SCOPE_DEPTH]={0};

extern SymbolTable *symboltables[MAX_SCOPE_DEPTH][MAX_SCOPE_DEPTH];

SymbolTable::SymbolTable(SymbolTable *p){
    prev = p;
    symboltables[Level][current_version[Level]] = this;
    //current_version[Level]++; //创建了新的符号表,当前层的 version + 1 
}

/* enterScope */
void EnterScope()
{
    Level++;
    if (Level == MAX_SCOPE_DEPTH)
    {
        cout << "Internal Error: out of nesting levels!\n";
        exit(-1);
    }
    //
}

void ExitScope()
{
    if (Level == 0)
    {
        cout << "missing '{' detected" << endl;
    }
    current_version[Level]++; //创建了新的符号表,当前层的 version + 1 
    Level--;
}

void SymbolTable::InsertSymbol(idNode *node)
{
    auto iter = idTable.find(node->name);
    if (iter == idTable.end())
    {
        idTable[node->name].push_back(node);
    }
    else
    {
        /* 遍历节点 查找是否定义过，同level和version不能重新定义 */
        // 此处不同于LookupSymbol,所以代码未重用
        //idTable[node->name].push_back(node);
        for (auto it = idTable[node->name].begin(); it != idTable[node->name].end(); it++)
        {
            if ((*it)->level == Level && (*it)->version == current_version[Level])
            {
                cout << "idNode had been declared!";
                exit(-1);
            }
        }
        idTable[node->name].push_back(node);
    }
}

/* 必须查找上层作用域名 还未修改*/
bool SymbolTable::LookupSymbol(string name)
{
    auto iter = idTable.find(name);
    if (iter == idTable.end())
        return false;
    for (auto it = idTable[name].begin(); it != idTable[name].end(); it++)
    {
        if ((*it)->level == Level && (*it)->version == current_version[Level])
            return true;
    }
    return false;
}

void SymbolTable::InsertCompositeSymbol(string name, compositeNode *comp)
{
    compTable.insert(make_pair(name, comp));
    static_cast<compositeNode *>(comp)->level = Level;
    static_cast<compositeNode *>(comp)->version = current_version[Level];
}

compositeNode *SymbolTable::LookupCompositeSymbol(string name)
{
    if (compTable.find(name) != compTable.end())
        return compTable[name];
    else
        return NULL;
}

idNode *SymbolTable::operator[](string str)
{
    auto iter = idTable.find(str);
    if (iter != idTable.end())
    {
        for (auto it = idTable[str].begin(); it != idTable[str].end(); it++)
        {
            if ((*it)->level == Level && (*it)->version == current_version[Level])
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

void SymbolTable::put(string s, idNode *id)
{
    table.insert((make_pair(s, id)));
}

idNode *SymbolTable::get(string s)
{
    
    for (SymbolTable *e = this; e != NULL; e = e->getPrev())
    {
        for(auto it:e->table)
        cout<<it.first<<" ";
        cout<<endl;
        auto found = e->table.find(s);
        if (found != e->table.end())
            return found->second;
        
    }
    string mesg = s + " has not been defined!";
    cout << mesg << endl;
    exit(-1);
    return NULL;
}


void SymbolTable::InserIdentifySymbol(Node *node){
    string name;
    switch(node->type){
        case Id:{
            name = static_cast<idNode *>(node)->name;
            static_cast<idNode *>(node)->level = Level;
            static_cast<idNode *>(node)->version = current_version[Level];
            break;
        }
        case InOutdcl:{
            name = static_cast<inOutdeclNode *>(node)->id->name;
            // 是否需要设置 level version ?
            break;
        }
    }
    auto iter = identifyTable.find(name);
    if (iter == identifyTable.end())
    {
        identifyTable.insert(make_pair(name, node));
    }
    else
    {
        cout << name<<" had been declared!";
        exit(-1);
    }
}

Node* SymbolTable::LookupIdentifySymbol(string name){
    SymbolTable *right_pre;
    auto iter = identifyTable.find(name);
    if(!(iter == identifyTable.end())){
        return iter->second;
    }else{
        if(prev == NULL) return NULL;
        do{
            right_pre = prev;
            iter = right_pre->identifyTable.find(name);
        }while(iter == right_pre->identifyTable.end() && right_pre->prev != NULL);
        if(iter == right_pre->identifyTable.end()){
            return NULL;
        }else{
            return iter->second;
        }
    }
    

}

void SymbolTable::InsertFunctionSymbol(funcDclNode *func){
    auto iter = funcTable.find(func->name);
    if (iter == funcTable.end())
    {
        funcTable.insert(make_pair(func->name, func));
    }
    else
    {
        cout << func->name<<" had been declared!";
        exit(-1);
    }
}

funcDclNode* SymbolTable::LookupFunctionSymbol(string name){
    SymbolTable *right_pre;
    auto iter = funcTable.find(name);
    if(!(iter == funcTable.end())){
        return iter->second;
    }else{
        if(prev == NULL) return NULL;
        do{
            right_pre = prev;
            iter = right_pre->funcTable.find(name);
        }while(iter == right_pre->funcTable.end() && right_pre->prev != NULL);
        if(iter == right_pre->funcTable.end()){
            return NULL;
        }else{
            return iter->second;
        }
    }
}

/*void SymbolTable::InserInoutSymbol(Node *node){
    string name;
    name = static_cast<inOutdeclNode *>(node)->id->name;

    auto iter = inoutTable.find(name);
    if (iter == inoutTable.end())
    {
        inoutTable.insert(make_pair(name, node));
    }
    else
    {
        cout << name<<" had been declared!";
        exit(-1);
    }
}*/



void SymbolTable::InsertOperatorSymbol(string name, operatorNode *opt)
{
    optTable.insert(make_pair(name, opt));
    static_cast<operatorNode *>(opt)->level = Level;
    static_cast<operatorNode *>(opt)->version = current_version[Level];
}

void SymbolTable::printSymbolTables(){
    cout<<"---------- Identify Table: ----------\n";
    for(auto it = identifyTable.begin();it!=identifyTable.end();it++){
        cout<<it->first<<endl;
    }
    cout<<"---------- Composite Table: ----------\n";
    for(auto it = compTable.begin();it!=compTable.end();it++){
        cout<<it->first<<endl;
    }

}

