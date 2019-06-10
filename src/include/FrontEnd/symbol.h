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


class SymbolTable
{
  public:
    SymbolTable() {}
    SymbolTable(SymbolTable *p);
    ~SymbolTable() {}
    bool LookupSymbol(string name);
    void InsertSymbol(idNode *node);
    /*put和get表示变量的插入和查找*/
    void put(string s, idNode *);
    idNode *get(string s);
    
    SymbolTable *getPrev()
    {
        return prev;
    }
    idNode *operator[](string str);
    map<string, bool> firstScanFuncTable;
    map<string, bool> firstScanCompTable;

    void InsertCompositeSymbol(string name, compositeNode *);
    compositeNode *LookupCompositeSymbol(string name);

    void InserIdentifySymbol(Node *node);
    Node* LookupIdentifySymbol(string name);

    void InsertFunctionSymbol(funcDclNode *func);
    funcDclNode* LookupFunctionSymbol(string name);

   // void SymbolTable::InserInoutSymbol(Node *node);

    void InsertOperatorSymbol(string name, operatorNode *opt);

    void printSymbolTables();

  private:
    SymbolTable *prev;

    map<string, idNode *> table;
    map<string, list<idNode *>> idTable;

    map<string, funcDclNode *> funcTable;
    //map<string, Node *> inoutTable;
    map<string, Node *> identifyTable; //变量
    map<string, compositeNode *> compTable; // composite
    map<string, operatorNode *> optTable; //operator
    map<string, Node *> arrTable; // 数组 

    
};
#endif