#ifndef _SYMBOL_H_
#define _SYMBOL_H_
#define MAX_SCOPE_DEPTH 100 //定义最大嵌套深度为100
/*level表示当前嵌套深度，version表示嵌套域计数器 */

#include "defines.h"
#include "node.h"
#include <map>
#include <list>

void EnterScope(void);
void ExitScope(void);

class SymbolTable{
public:
    
    SymbolTable(){}
    ~SymbolTable(){}
    bool LookupSymbol(string  name);
    void InsertSymbol(identifierNode* node);
    identifierNode* operator[](string str);
    map<string,bool>firstScanFuncTable;
    map<string,bool>firstScanCompTable;

private:
    map<string,list<identifierNode*> >idTable;
    map<string,functionNode*> funcTable;
    map<string,compositeNode*> compTable;
    
};
#endif