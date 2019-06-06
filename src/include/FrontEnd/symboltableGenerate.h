#include "node.h"
#include "defines.h"
#include "nodetype.h"
#include "symbol.h"

void generateSymbolTable(list<Node *> *program,SymbolTable *symbol_tables[][MAX_SCOPE_DEPTH]);
void generateSymbolTableDeclare(declareNode* dlcNode);
void generateSymbolTableComposite(compositeNode* composite);

void printSymbolTable(SymbolTable *symbol_tables[][MAX_SCOPE_DEPTH]);
