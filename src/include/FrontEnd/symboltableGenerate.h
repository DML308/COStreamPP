#include "node.h"
#include "defines.h"
#include "nodetype.h"
#include "symbol.h"

void generateSymbolTable(list<Node *> *program,SymbolTable *symbol_tables[][MAX_SCOPE_DEPTH]);
void generateSymbolTableDeclare(declareNode* dlcNode);
Constant* getOperationResult(Node* exp);
SymbolTable* generateCompositeRunningContext(compositeNode *composite,list<Constant *> paramList);

list<Constant *> generateStreamList(list<Node *> *stream_List,int target);
list<Constant *> generateStreamList(list<Node *> *stream_List,SymbolTable *s);

void printSymbolTable(SymbolTable *symbol_tables[][MAX_SCOPE_DEPTH]);
