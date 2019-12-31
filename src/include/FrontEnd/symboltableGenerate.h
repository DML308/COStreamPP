#include "node.h"
#include "defines.h"
#include "nodetype.h"
#include "symbol.h"

void generateSymbolTable(list<Node *> *program,SymbolTable *symbol_tables[][MAX_SCOPE_DEPTH]);
void generateSymbolTableDeclare(declareNode* dlcNode);
void genrateStmt(Node *stmt);
Constant* getOperationResult(Node* exp);
SymbolTable* generateCompositeRunningContext(compositeNode *composite,list<Constant *> paramList,list<Node *> *inputs,list<Node *> *outputs);

list<Constant *> generateStreamList(list<Node *> *stream_List,int target);
list<Constant *> generateStreamList(list<Node *> *stream_List,SymbolTable *s);

void printSymbolTable(SymbolTable *symbol_tables[][MAX_SCOPE_DEPTH]);
