#include "symbol.h"
int Level=0;
int current_version[MAX_SCOPE_DEPTH]={0};
list<SymbolTable *> symbol_tables;
bool isSorted = false;
extern SymbolTable *symboltables[MAX_SCOPE_DEPTH][MAX_SCOPE_DEPTH];

SymbolTable::SymbolTable(SymbolTable *p,YYLTYPE *loc){
    this->loc = loc;
    prev = p;
    symbol_tables.push_back(this);
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
                cout << (*it)->loc->first_line<<": idNode had been declared! ";
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
    CompositeSymbol *comp_s = new CompositeSymbol(comp);
    compTable.insert(make_pair(name, comp_s));
    static_cast<compositeNode *>(comp)->level = Level;
    static_cast<compositeNode *>(comp)->version = current_version[Level];
}

CompositeSymbol *SymbolTable::LookupCompositeSymbol(string name)
{
    SymbolTable *right_pre;
    auto iter = compTable.find(name);
    if(!(iter == compTable.end())){
        return iter->second;
    }else{ // 往上层作用域查找
        if(prev == NULL) return NULL;
        right_pre = prev;
        do{
            iter = right_pre->compTable.find(name);
            if(right_pre->prev != NULL){
                right_pre = right_pre->prev;
            }else{
                break;
            }
        }while(iter == right_pre->compTable.end());
        if(iter == right_pre->compTable.end()){
            return NULL;
        }else{
            return iter->second;
        }
    }
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

void SymbolTable::InsertIdentifySymbol(Node *node){
    string name;
    Variable *variable = NULL;
    switch(node->type){
        case Id:{
            idNode * id = static_cast<idNode *>(node);
            name = id->name;
            string type = id->valType;
            variable = new Variable(type,name);
            static_cast<idNode *>(node)->level = Level;
            static_cast<idNode *>(node)->version = current_version[Level];
            variable->level = Level;
            variable->version = current_version[Level];
            break;
        }
        case InOutdcl:{
            name = static_cast<inOutdeclNode *>(node)->id->name;
            // 是否需要设置 level version ?
            break;
        }
    }
    auto iter = variableTable.find(name);
    if (iter == variableTable.end())
    {
        variableTable.insert(make_pair(name, variable));
    }
    else
    {
        cout <<node->loc->first_line<<": "<<name<<" had been declared!";
        exit(-1);
    }
}

void SymbolTable::InsertIdentifySymbol(Variable *variable){

    string name = variable->name;
    variable->level = Level;
    variable->version = current_version[Level];

    auto iter = variableTable.find(name);
    if (iter == variableTable.end())
    {
        variableTable.insert(make_pair(name, variable));
    }
    else
    {
        cout << name<<" had been declared!";
        exit(-1);cout << name<<" had been declared!";
    }
}

void SymbolTable::InsertStreamSymbol(inOutdeclNode* inOutNode){
    string name = ((idNode *)(inOutNode->id))->name;
    auto iter = streamTable.find(name);
    if(iter == streamTable.end()){
        streamTable.insert(make_pair(name,inOutNode));
    }else{
        cout << inOutNode->loc->first_line << ": " << "stream " << name <<" had been declared!";
        exit(-1);
    }
}

inOutdeclNode* SymbolTable::LookUpStreamSymbol(string name){
    SymbolTable *right_pre;
    auto iter = streamTable.find(name);
    if(!(iter == streamTable.end())){
        return iter->second;
    }else{ // 往上层作用域查找
        if(prev == NULL) return NULL;
        right_pre = prev;
        while(right_pre!=NULL){
            iter = right_pre->streamTable.find(name);
            if(iter != right_pre->streamTable.end()){
                break;
            }
            right_pre = right_pre->prev;
        }
        if(right_pre == NULL){
            return NULL;
        }else{
            return iter->second;
        }
    }
}
void SymbolTable::InsertIdentifySymbol(Node *node,Constant *constant){
    string name;
    Variable *variable = NULL;
    switch(node->type){
        case Id:{
            idNode * id = static_cast<idNode *>(node);
            name = id->name;
            string type = id->valType;
            variable = new Variable(type,name,constant);
            static_cast<idNode *>(node)->level = Level;
            static_cast<idNode *>(node)->version = current_version[Level];
            variable->level = Level;
            variable->version = current_version[Level];
            break;
        }
        case InOutdcl:{
            name = static_cast<inOutdeclNode *>(node)->id->name;
            // 是否需要设置 level version ?
            break;
        }
    }
    auto iter = variableTable.find(name);
    if (iter == variableTable.end())
    {
        variableTable.insert(make_pair(name, variable));
    }
    else
    {
        cout <<node->loc->first_line << ": " << name<<" had been declared!";
        exit(-1);
    }
}

Variable* SymbolTable::LookupIdentifySymbol(string name){
    SymbolTable *right_pre;
    auto iter = variableTable.find(name);
    if(!(iter == variableTable.end())){
        return iter->second;
    }else{ // 往上层作用域查找
        if(prev == NULL) return NULL;
        right_pre = prev;
        while(right_pre!=NULL){
            iter = right_pre->variableTable.find(name);
            if(iter != right_pre->variableTable.end()){
                break;
            }
            right_pre = right_pre->prev;
        }
        if(right_pre == NULL){
            return NULL;
        }else{
            return iter->second;
        }
    }
}

/*Node* SymbolTable::LookupIdentifySymbol(string name){
    SymbolTable *right_pre;
    auto iter = identifyTable.find(name);
    if(!(iter == identifyTable.end())){
        return iter->second;
    }else{ // 往上层作用域查找
        if(prev == NULL) return NULL;
        right_pre = prev;
        while(right_pre!=NULL){
            iter = right_pre->identifyTable.find(name);
            if(iter != right_pre->identifyTable.end()){
                break;
            }
            right_pre = right_pre->prev;
        }
        if(right_pre == NULL){
            return NULL;
        }else{
            return iter->second;
        }
    }
}*/

void SymbolTable::InsertFunctionSymbol(funcDclNode *func){
    auto iter = funcTable.find(func->name);
    if (iter == funcTable.end())
    {
        funcTable.insert(make_pair(func->name, func));
    }
    else
    {
        cout << func->loc->first_line << ": " << func->name<<" had been declared!";
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
        right_pre = prev;
        do{
            iter = right_pre->funcTable.find(name);
            if(right_pre->prev != NULL){
                right_pre = right_pre->prev;
            }else{
                break;
            }
        }while(iter == right_pre->funcTable.end());
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

void SymbolTable::InsertParamSymbol(Variable *variable){
    paramTable.insert(make_pair(variable->name,variable));
}

constantNode* SymbolTable::fromVariableToConstant(Variable *value){
      string type = value->value->type;
      string name = value->name;
      if(type.compare("int") == 0){
        return new constantNode(type,value->value->ival);
      }
      if(type.compare("long") == 0){
        return new constantNode(type,value->value->lval);
      }
      if(type.compare("long long") == 0){
        return new constantNode(type,value->value->llval);
      }
      if(type.compare("double") == 0){
        return new constantNode(type,value->value->dval);
      }
      if(type.compare("float") == 0){
        return new constantNode(type,value->value->fval);
      }
      if(type.compare("bool") == 0){
        return new constantNode(type,value->value->bval);
      }
      if(type.compare("string") == 0){
        return new constantNode(type,value->value->sval);
      }
      return NULL;
    }

string SymbolTable::toParamString(){
    string params_str = "";
    unordered_map<string,Variable*>::iterator it;
    for(it=variableTable.begin();it!=variableTable.end();it++){
        Variable *variable = (Variable *)(it->second);
        string param_str ="\t" + variable->type + " " + variable->name + ";" +"\n";
        params_str += param_str;
    }
    return params_str;
}

string SymbolTable::toParamValueString(){
    string params_str = "";
    unordered_map<string,Variable*>::iterator it;
    for(it=variableTable.begin();it!=variableTable.end();it++){
        Variable *variable = (Variable *)(it->second);
        string param_str;
        if(variable->value){
            param_str ="\t" + variable->name + "=" + variable->value->printStr(false) + ";" +"\n";
        }
        params_str += param_str;
    }
    return params_str;
}

void SymbolTable::printSymbolTables(){
    cout<<"---------- Identify Table: ----------\n";
    for(auto it = variableTable.begin();it!=variableTable.end();it++){
        cout<<it->first<<":\t";
        string type = it->first;
        if(it->second != NULL){
            string type = it->second->type;
            if(type.compare("array") == 0){
                it->second->array->print();
            }else{
                if(!it->second->value){
                    cout<<"undefined"<<endl;
                }else{
                    it->second->value->print(false);
                }
                
            }  
        }else{
            cout<<endl;
        }   
    }
    cout<<"---------- Composite Table: ----------\n";
    for(auto it = compTable.begin();it!=compTable.end();it++){
        cout<<it->first<<endl;  
    }

}

// 查找第一个大于 target 的 值
int getFirstBigger(int target,list<SymbolTable *> symbol_tables) {
    int count = 0;
    int left = 0,
        right = symbol_tables.size() - 1,
        middle = 0;
    while (left <= right) {
        middle = (left + right) / 2;
        list<SymbolTable*>::iterator it = symbol_tables.begin();
        advance(it,middle-1);
        if ((*it)->loc->last_line > target)
            right = middle - 1;
        else if ((*it)->loc->last_line < target)
            left = middle + 1;
        else
            return middle;
    }
    return left;
}

// 查找最后 一个小于 target 的 值
int getLastSmaller(int target,list<SymbolTable *> symbol_tables) {
    int left = 0,
    right = symbol_tables.size() - 1,
        middle = 0;
    while (left <= right) {
        middle = (left + right) / 2;
        list<SymbolTable*>::iterator it = symbol_tables.begin();
        advance(it,middle-1);
        if ((*it)->loc->first_line > target)
            right = middle - 1;
        else if ((*it)->loc->first_line < target)
            left = middle + 1;
        else
            return middle;
    }
   return right;
}

list<SymbolTable *> first_symbol_tables(symbol_tables.size()),last_symbol_tables(symbol_tables.size());

bool compareFirst(SymbolTable *a,SymbolTable *b){
    return a->loc->first_line - b->loc->first_line;
}
bool compareLast(SymbolTable *a,SymbolTable *b){
    return a->loc->last_line - b->loc->last_line;
}

SymbolTable* FindRightSymbolTable(int target) {
    if(!isSorted){
        copy(symbol_tables.begin(),symbol_tables.end(),last_symbol_tables.begin()); 
        copy(symbol_tables.begin(),symbol_tables.end(),first_symbol_tables.begin()); 
        first_symbol_tables.sort(compareFirst);
        last_symbol_tables.sort(compareLast);
        isSorted = true;
    }
    int line_start, line_end;
    line_end = getFirstBigger(target,last_symbol_tables);
    line_start = getLastSmaller(target,first_symbol_tables);
    
    list<SymbolTable*>::iterator itLast = last_symbol_tables.begin();
    advance(itLast,line_end-1);
    YYLTYPE *last_loc = (*itLast)->loc;

    list<SymbolTable*>::iterator itFirst = first_symbol_tables.begin();
    advance(itFirst,line_start-1);
    YYLTYPE *first_loc = (*itFirst)->loc;

    if(last_loc->first_line<= target && last_loc->last_line >= target){
        return (*itLast);
    }else {
        return (*itFirst);
    }
}

void ArrayConstant::print(){
    int count = 0;
      for(auto it : values){
        if(count != 0){
          cout<<",";
        }
        count++;
        (it)->print(true);
      }
      cout<<endl;
}