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



class Constant{
  public:
    int ival;
    long lval;
    long long llval;
    float fval;
    double dval;
    bool bval;
    string sval;
    string type;
    Constant(string type, int i) :type(type), ival(i){}
    Constant(string type, long l) :type(type), lval(l){}
    Constant(string type, long long l) :type(type), llval(l){}
    Constant(string type, float f) :type(type), fval(f){}
    Constant(string type, double d) :type(type), dval(d){}
    Constant(string type, string str) :type(type), sval(str){}
    Constant(string type, bool b) :type(type), bval(b){}
    ~Constant() {}

    void print(bool isArray){
      if(type.compare("int") == 0){
            cout<<ival;
        }
      if(type.compare("long") == 0){
          cout<<lval;
      }
      if(type.compare("long long") == 0){
          cout<<llval;
        }
      if(type.compare("double") == 0){
          cout<<dval;
      }
      if(type.compare("float") == 0){
          cout<<fval;
      }
      if(type.compare("string") == 0){
          cout<<sval;
      }
      if(type.compare("bool") == 0){
          cout<<bval;
      }
      if(!isArray){
        cout<<endl;
      }
  }

};

class ArrayConstant {
  public:
    list<Constant *> values;
    string type;
    ArrayConstant(string type) : type(type){}

    void print();
};

class Variable {
  public:
    string type; // int,long,long long,float,double,string,array 
    string name;
    constantNode *cvalue;
    Constant *value;
    ArrayConstant *array;
    int level = 0,version = 0;
    Variable(string type,string name,Constant* constant):type(type),name(name){
      this->value = constant;
    }
    Variable(string type,string name):type(type),name(name){
    }
    Variable(string type,string name,int i):type(type),name(name){
      this->value = new Constant(type,i);
    }
    Variable(string type,string name,long i):type(type),name(name){
      this->value = new Constant(type,i);
    }
    Variable(string type,string name,long long i):type(type),name(name){
      this->value = new Constant(type,i);
    }
    Variable(string type,string name,float i):type(type),name(name){
      this->value = new Constant(type,i);
    }
    Variable(string type,string name,double i):type(type),name(name){
      this->value = new Constant(type,i);
    }
    Variable(string type,string name,string i):type(type),name(name){
      this->value = new Constant(type,i);
    }
    Variable(string type,string name,ArrayConstant* array):type(type),name(name){
      this->array = array;
    }
    ~Variable(){
  };
};

class SymbolTable
{
  public:
    YYLTYPE *loc;
    SymbolTable() {}
    SymbolTable(SymbolTable *p,YYLTYPE *loc);
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

    void InsertIdentifySymbol(Node *node,Constant *constant);
    
    void InsertIdentifySymbol(Node *node);

    void InsertIdentifySymbol(Variable *variable);

    void InsertStreamSymbol(string name,inOutdeclNode* inOutNode);

    inOutdeclNode* LookUpStreamSymbol(string name);
    //old
    //Node* LookupIdentifySymbol(string name);
    // new
    Variable* LookupIdentifySymbol(string name);

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

    map<string, inOutdeclNode *> streamTable;

    map<string, Node *> identifyTable; //变量
    map<string, Variable *> variableTable; //变量
    map<string, compositeNode *> compTable; // composite
    map<string, operatorNode *> optTable; //operator

    
};

SymbolTable* FindRightSymbolTable(int target);

#endif