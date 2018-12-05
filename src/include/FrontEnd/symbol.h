#ifndef _SYMBOL_H_
#define _SYMBOL_H_
#define MAX_SCOPE_DEPTH 100 //定义最大嵌套深度为100
/*level表示当前嵌套深度，version表示嵌套域计数器 */
extern int Level;
extern int current_version[MAX_SCOPE_DEPTH];

#include "defines.h"
#include "node.h"
#include <map>
#include <list>
/*
typedef struct {
    int level;
    int version; 

}scope;
*/
void EnterScope(void);
void ExitScope(void);
class SymbolTable{
public:
    
    SymbolTable(string name){
        name=name;
    }
    ~SymbolTable(){
    }
    bool LookupSymbol(identifierNode* node,map<string,list<identifierNode*> > mp);
    void InsertSymbol(identifierNode* node,map<string,list<identifierNode*> > mp);
private:
    string name;
    map<string,list<identifierNode*> >identifierTable;
    map<string,functionNode*> functionTable;
    map<string,compositeNode*> compositeTable;
};
#endif