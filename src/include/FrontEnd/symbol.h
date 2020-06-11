#ifndef _SYMBOL_H_
#define _SYMBOL_H_
#define MAX_SCOPE_DEPTH 100 //定义最大嵌套深度为100
/*level表示当前嵌套深度，version表示嵌套域计数器 */

#include "defines.h"
#include "node.h"
#include <map>
#include <list>
#include <algorithm>
#include <vector>

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

    bool isArray;
    string sval;
    string type;
    Constant(string type, int i) :type(type), ival(i),isArray(false){}
    Constant(string type, long l) :type(type), lval(l),isArray(false){}
    Constant(string type, long long l) :type(type), llval(l),isArray(false){}
    Constant(string type, float f) :type(type), fval(f),isArray(false){}
    Constant(string type, double d) :type(type), dval(d),isArray(false){}
    Constant(string type, string str) :type(type), sval(str),isArray(false){}
    Constant(string type, bool b) :type(type), bval(b),isArray(false){}

    Constant(int i) :type("int"), ival(i),isArray(false){}
    Constant(long l) :type("long"), lval(l),isArray(false){}
    Constant(long long l) :type("long long"), llval(l),isArray(false){}
    Constant(float f) :type("float"), fval(f),isArray(false){}
    Constant(double d) :type("double"), dval(d),isArray(false){}
    //Constant(string str) :type("string"), sval(str),isArray(false){}
    Constant(bool b) :type("bool"), bval(b),isArray(false){}

    Constant(string type) :type(type){}
    ~Constant() {}

    void print(){
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
  }

  string printStr(){
      string str = "";
      if(type.compare("int") == 0){
            str = to_string(ival);
        }
      if(type.compare("long") == 0){
          str = to_string(lval);
      }
      if(type.compare("long long") == 0){
          str = to_string(llval);
      }
      if(type.compare("double") == 0){
          str = to_string(dval);
      }
      if(type.compare("float") == 0){
          str = to_string(fval);
      }
      if(type.compare("string") == 0){
          str = sval;
      }
      if(type.compare("bool") == 0){
          str = to_string(bval);
      }
      return str;
  }

};

class ArrayConstant : public Constant{
  public:
    vector<Constant *> values;
    //string type;
    vector<int> arg_size;
    int index;//用于遍历
    ArrayConstant(string type) : Constant(type){this->isArray = true;}
    void print();
    string printEachLevel(int level);
    string printStr();
};

constantNode *copyConstantNode(Constant *value);
Constant *copyConstant(Constant *value);

class Variable {
  public:
    string type; // int,long,long long,float,double,string
    string name;
    constantNode *cvalue;
    Constant *value;
    //ArrayConstant *array;
    bool isArray;
    int level = 0,version = 0;
    Variable(string type,string name,Constant* constant):type(type),name(name),isArray(false){
      this->value = constant;
    }
    Variable(string type,string name):type(type),name(name),isArray(false){
    }
    Variable(string type,string name,int i):type(type),name(name),isArray(false){
      this->value = new Constant(type,i);
    }
    Variable(string type,string name,long i):type(type),name(name),isArray(false){
      this->value = new Constant(type,i);
    }
    Variable(string type,string name,long long i):type(type),name(name),isArray(false){
      this->value = new Constant(type,i);
    }
    Variable(string type,string name,float i):type(type),name(name),isArray(false){
      this->value = new Constant(type,i);
    }
    Variable(string type,string name,double i):type(type),name(name),isArray(false){
      this->value = new Constant(type,i);
    }
    Variable(string type,string name,string i):type(type),name(name),isArray(false){
      this->value = new Constant(type,i);
    }
    Variable(string type,string name,ArrayConstant* array):type(type),name(name){
      this->value = array;
      this->isArray = true;

    }
    ~Variable(){
  };
};

class CompositeSymbol
{ 
    public:
      compositeNode *composite;
      int count;
      CompositeSymbol(compositeNode *comp){
        this->composite = comp;
        this->count = 0;
      }   
};

//哈希函数
struct str_hash{
        size_t operator()(const string& str) const
        {
                unsigned long __h = 0;
                for (size_t i = 0 ; i < str.size() ; i ++)
                  __h = 131*__h + str[i];
                return size_t(__h);
        }
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

    int count = 0;//用于区分同一个composite

    void InsertCompositeSymbol(string name, compositeNode *);
    CompositeSymbol *LookupCompositeSymbol(string name);

    void InsertIdentifySymbol(Node *node,Constant *constant);//√
    
    void InsertIdentifySymbol(Node *node);

    void InsertIdentifySymbol(Variable *variable);//√

    void InsertStreamSymbol(inOutdeclNode* inOutNode);

    inOutdeclNode* LookUpStreamSymbol(string name);
    //old
    //Node* LookupIdentifySymbol(string name);
    // new
    Variable* LookupIdentifySymbol(string name);

    void InsertFunctionSymbol(funcDclNode *func);
    funcDclNode* LookupFunctionSymbol(string name);

   // void SymbolTable::InserInoutSymbol(Node *node);

    void InsertOperatorSymbol(string name, operatorNode *opt);

    void InsertParamSymbol(Variable *variable);//√
    
    string toParamString(SymbolTable *opt_table);

    string toParamValueString(SymbolTable *opt_table);

    void printSymbolTables();
    
    map<string, inOutdeclNode *> getStreamTable(){
      return this->streamTable;
    }

    constantNode *fromVariableToConstant(Variable *value);

  private:
    
    SymbolTable *prev;

    map<string, idNode *> table;
    map<string, list<idNode *>> idTable;

    map<string, funcDclNode *> funcTable; 

    map<string, inOutdeclNode *> streamTable; //stream √

    //map<string, Node *> identifyTable; 
    //map<string, Variable *> variableTable; 
    map<string,Variable *>variableTable;//变量 √
    map<string,Variable *> paramTable;//参数变量 用于代码生成时在operator中添加该参数变量 √
    map<string, CompositeSymbol *> compTable; // composite √
    map<string, operatorNode *> optTable; //operator

    
};

SymbolTable* FindRightSymbolTable(int target);

#endif