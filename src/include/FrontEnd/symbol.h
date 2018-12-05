#ifndef _SYMBOL_H_
#define _SYMBOL_H_
#define MAX_SCOPE_DEPTH 100 //定义最大嵌套深度为100
/*level表示当前嵌套深度，version表示嵌套域计数器 */
#include "defines.h"
#include <map>
#include <list>
static int current_version[MAX_SCOPE_DEPTH];
static int Level;

typedef struct {
    int level;
    int version; 
}scope;
void EnterScope(void);
void ExitScope(void);
class SymbolTable{
public:
    
    SymbolTable(string name,string type){
        name=name;
        if(type=="flat") InsertSymbol("flat",map<string,list<scope> >());
        else InsertSymbol("Nested",map<string,list<scope> >());
    }
    ~SymbolTable(){
    }
    bool LookupSymbol(string id,map<string,list<scope> > mp);
    void InsertSymbol(string id,map<string,list<scope> > mp);
private:
    string name;
    map<string,list<scope> > table;
};
#endif