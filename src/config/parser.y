%{
//#define DEBUG
#include "defines.h"
#include "node.h"
#include "symbol.h"
//#include <list>

extern SymbolTable S;
extern list<Node*> *Program;
extern int yylex ();
extern void yyerror (const char *msg);

%}

/*在 union 里声明 %token 可能有的类型*/
%union{
    long long  num;
    double doubleNum;
    std::string *str;
    Node * node;
    std::list<Node*> *list;
}
/* A. 下面是从词法分析器传进来的 token ,其中大部分都是换名字符串*/
%token integerConstant  stringConstant      doubleConstant  IDENTIFIER
%token STRING     INT   DOUBLE  FLOAT       LONG    CONST   DEFINE
%token WHILE      FOR   BREAK   CONTINUE    SWITCH  CASE DEFAULT IF ELSE DO RETURN
%token POUNDPOUND ICR   DECR    ANDAND      OROR    LS  RS LE GE EQ NE
%token MULTassign DIVassign     PLUSassign  MINUSassign MODassign
%token LSassign   RSassign ANDassign ERassign ORassign
    /* A.1 ----------------- COStream 特有关键字 ---------------*/
%token COMPOSITE  INPUT OUTPUT  STREAM    FILEREADER  FILEWRITER  ADD
%token PARAM      INIT  WORK    WINDOW    TUMBLING    SLIDING
%token SPLITJOIN  PIPELINE      SPLIT     JOIN        DUPLICATE ROUNDROBIN

/* B.下面是语法分析器自己拥有的文法结构和类型声明 */

/* 语法分析器自己的结构 1. 文法一级入口*/
%type<list> translation.unit 
%type<node> external.definition
/* 语法分析器自己的结构   1.1.declaration */

%type<node> declaration declaring.list stream.declaring.list
%type<node> stream.type.specifier
%type<node> stream.declaration.list 
/* 语法分析器自己的结构     1.1.3.array */
%type<node> array.declarator
/* 语法分析器自己的结构     1.1.4.initializer */
%type<node> initializer.opt initializer initializer.list
/* 语法分析器自己的结构   1.2 function.definition 函数声明 */
%type<node> function.definition function.body  parameter.declaration
%type<list> statement.list  parameter.list
/* 语法分析器自己的结构   1.3 composite.definition 数据流计算单元声明 */
%type<node> composite.definition  composite.head  composite.head.inout
%type<node> composite.head.inout.member
%type<list> composite.head.inout.member.list
%type<node> operator.pipeline
/* 语法分析器自己的结构      1.3.2 composite.body */
%type<node> composite.body  composite.body.param.opt
%type<node> costream.composite.statement
%type<list> composite.body.statement.list
/* 语法分析器自己的结构 2. composite.body.operator  composite体内的init work window等组件  */
%type<node> composite.body.operator   operator.file.writer  operator.add
%type<node> operator.splitjoin  split.statement
%type<list> splitjoinPipeline.statement.list
%type<node> roundrobin.statement   duplicate.statement  join.statement  operator.default.call
%type<list> argument.expression.list
/* 语法分析器自己的结构 3.statement 花括号内以';'结尾的结构是statement  */
%type<node> statement labeled.statement compound.statement
%type<node> expression.statement  selection.statement   iteration.statement jump.statement
/* 语法分析器自己的结构 4.exp 计算表达式头节点  */
%type<str>  assignment.operator
%type<node> exp exp.assignable
%type<node> operator.selfdefine.body  operator.selfdefine.body.init operator.selfdefine.body.work
%type<node> operator.selfdefine.body.window.list operator.selfdefine.window
%type<list> operator.selfdefine.window.list 
%type<node> window.type
/* 语法分析器自己的结构 5.basic 从词法TOKEN直接归约得到的节点 */
%type<node>constant type.specifier basic.type.name  
%type<num> integerConstant
%type<doubleNum> doubleConstant
%type<str> stringConstant IDENTIFIER



/* C. 优先级标记,从上至下优先级从低到高排列 */
%right '='
%left OROR
%left ANDAND
%left '|'
%left '^'
%left '&'
%left EQ NE
%left '<' '>' LE GE
%left LS RS
%left '-' '+'
%left '*' '/' '%'
%left '.'
%left ')' ']'
%left '(' '['

/* D. 语法分析器的起点和坐标声明 */
%start prog.start
%locations

%%
/************************************************************************/
/*              1. 文法一级入口,由下面三种文法组成                           */
/*                 1.1. declaration 声明                                 */
/*                 1.2. function.definition 函数声明                      */
/*                 1.3. composite.definition 数据流计算单元声明             */
/*************************************************************************/
prog.start: translation.unit { Program = $1 ; };

translation.unit:
          external.definition   {
                                      line("Line:%-3d",@1.first_line);
                                      debug ("translation.unit ::= external.definition\n");
                                      list<Node*> *ext_List=new list<Node*>();
                                      ext_List->push_back($1);
                                      $$ = ext_List ;
                                }
        | translation.unit external.definition  {
                                                      line("Line:%-3d",@1.first_line);
                                                      debug ("translation.unit ::= translation.unit external.definition\n");
                                                      $1->push_back($2);
                                                      $$ = $1 ;
                                                }
        ;
external.definition:
          declaration           {
                                      line("Line:%-3d",@1.first_line);
                                      debug ("external.definition ::= declaration\n");
                                      $$ = $1;
                                }
        | function.definition   {
                                      line("Line:%-3d",@1.first_line);
                                      debug ("external.definition ::= function.definition\n");
                                      $$ = $1 ;
                                }
        | composite.definition  {
                                      line("Line:%-3d",@1.first_line);
                                      debug ("external.definition ::= composite.definition\n");
                                      $$ = $1 ;
                                }
        ;
/*************************************************************************/
/*              1.1 declaration 由下面2种文法+2个基础组件组成                */
/*                      1.1.1 declaring.list                             */
/*                      1.1.2 stream.declaring.list                      */
/*                      1.1.3 array                                      */
/*                      1.1.4 initializer                                */
/*************************************************************************/
declaration:
      declaring.list ';'        {
                                      line("Line:%-3d",@1.first_line);
                                      debug ("declaration ::= declaring.list ';' \n");
                                      $$ = $1 ;
                                }
    | stream.declaring.list ';' {
                                      line("Line:%-3d",@1.first_line);
                                      debug ("declaration ::= stream.declaring.list ';' \n");
                                      $$ = $1 ;
                                }
    ;
declaring.list:
          type.specifier      IDENTIFIER       initializer.opt  {
              line("Line:%-3d",@1.first_line);
              debug ("declaring.list ::= type.specifier %s initializer.opt \n",$2->c_str());
              idNode *id=new idNode(*($2),(Loc*)&(@2));
              //if(S[*($2)]==NULL) S.InsertSymbol(id);
              $$ = new declareNode((primNode*)$1,id,NULL,(initNode*)$3,(Loc*)&(@2)) ;
              //error ("%s\n",name.c_str());
        }
        | type.specifier 	IDENTIFIER array.declarator initializer.opt{
              line("Line:%-3d",@1.first_line);
              debug ("declaring.list ::= type.specifier %s array.declarator initializer.opt \n",$2->c_str());
              idNode *id=new idNode(*($2),(Loc*)&(@2));
              //if(S[*($2)]==NULL) S.InsertSymbol(id);
              $$ = new declareNode((primNode*)$1,id,(adclNode*)$3,(initNode*)$4,(Loc*)&(@2));
              
        }
        | declaring.list 	',' 	IDENTIFIER initializer.opt{
              line("Line:%-3d",@1.first_line);
              debug ("declaring.list ::= declaring.list 	',' 	%s initializer.opt \n",$3->c_str());
              idNode *id=new idNode(*($3),(Loc*)&(@2));
              //if(S[*($3)]==NULL) S.InsertSymbol(id);
              ((declareNode*)$1)->append(id,NULL,(initNode*)$4);
              $$=$1;
        }
        | declaring.list 	',' 	IDENTIFIER array.declarator initializer.opt{
              line("Line:%-3d",@1.first_line);
              debug ("declaring.list ::= declaring.list 	',' 	%s array.declarator initializer.opt \n",$3->c_str());
              idNode *id=new idNode(*($3),(Loc*)&(@2));
              //if(S[*($3)]==NULL) S.InsertSymbol(id);
              ((declareNode*)$1)->append(id,(adclNode*)$4,(initNode*)$5);
              $$=$1;
        }
        ;
stream.declaring.list:
          stream.type.specifier IDENTIFIER    {
                                                  line("Line:%-3d",@1.first_line);
                                                  debug ("stream.declaring.list ::= stream.type.specifier %s \n",$2->c_str());
                                                  idNode *id=new idNode(*($2),(Loc*)&(@2));
                                                  /* 需要添加符号表插入操作 */
                                                  ((strdclNode*)($1))->insert(id);
                                                  $$ = $1 ;
                                              }
        | stream.declaring.list ',' IDENTIFIER{
                                                  line("Line:%-3d",@1.first_line);
                                                  debug ("stream.declaring.list ::= stream.declaring.list ',' %s \n",$3->c_str());
                                                  idNode *id=new idNode(*($3),(Loc*)&(@3));
                                                  /* 需要添加符号表插入操作 */
                                                  ((strdclNode*)($1))->insert(id);
                                                  $$ = $1 ;
                                              }
        ;
stream.type.specifier:
          STREAM '<' stream.declaration.list '>'{
                                                    line("Line:%-3d",@1.first_line);
                                                    debug ("stream.type.specifier ::=  STREAM '<' stream.declaration.list '>' \n");
                                                    $$ = $3 ;
                                                }
        ;
stream.declaration.list:
          type.specifier IDENTIFIER {
                                        line("Line:%-3d",@1.first_line);
                                        debug ("stream.declaration.list ::=  type.specifier %s \n",$2->c_str());
                                        /* 需要添加符号表查找操作*/
                                        $$ = new strdclNode((primNode*)$1,(idNode*)$2,NULL,(Loc*)&(@2)) ;
                                    }
        | type.specifier IDENTIFIER array.declarator{
                                        line("Line:%-3d",@1.first_line);
                                        debug ("stream.declaration.list ::=  type.specifier %s array.declarator \n",$2->c_str());
                                        $$ = new strdclNode((primNode*)$1,(idNode*)$2,(adclNode*)$3,(Loc*)&(@2)) ;
                                    }
        | stream.declaration.list ',' type.specifier IDENTIFIER {
                                        line("Line:%-3d",@1.first_line);
                                        debug ("stream.declaration.list ::=  stream.declaration.list ',' type.specifier %s \n",$4->c_str());
                                        ((strdclNode*)($1))->append((primNode*)$3,(idNode*)$4,NULL);
                                        $$ = $1 ;
                                    }
        | stream.declaration.list ',' type.specifier IDENTIFIER array.declarator{
                                        line("Line:%-3d",@1.first_line);
                                        debug ("stream.declaration.list ::=  stream.declaration.list ',' type.specifier %s array.declarator \n",$4->c_str());
                                        ((strdclNode*)($1))->append((primNode*)$3,(idNode*)$4,(adclNode*)$5);
                                        $$ = $1 ;
                                    }
        ;

/*************************************************************************/
/*                      1.1.3 array ( int a[] )                          */
/*************************************************************************/
array.declarator:
          '[' ']'   {
                            line("Line:%-3d",@1.first_line);
                            debug ("array.declarator ::= '[' ']' \n");
                            $$ = new adclNode(basic,NULL, (Loc *)&(@1)) ;
                    }
        | '[' exp ']' {
                            line("Line:%-3d",@1.first_line);
                            debug ("array.declarator ::= '[' exp ']' \n");
                            $$ = new adclNode(basic, (expNode*)$2, (Loc *)&(@1)) ;
                    }
        | array.declarator '[' exp ']'  {
                            line("Line:%-3d",@1.first_line);
                            debug ("array.declarator ::= array.declarator '[' exp ']' \n");
                            $$ =new adclNode(Arr,NULL,(Loc*)&(@2));

                    }
        | array.declarator '[' ']'  {
                            error ("Line:%-3d array declaration with illegal empty dimension\n",@1.first_line);
                            exit(-1);
                    }
        ;
/*************************************************************************/
/*                      1.1.4 initializer                                */
/*************************************************************************/
initializer.opt:
          /* nothing */         {
                                    line("Line:%-3d",@$.first_line);
                                    debug ("initializer.opt ::= nothing \n");
                                    $$ = NULL ;
                                }
        | '=' initializer       {
                                    line("Line:%-3d",@1.first_line);
                                    debug ("initializer.opt ::= '=' initializer \n");
                                    $$ = $2 ;
                                }
        ;
initializer:
          '{' initializer.list '}'      {
                                            line("Line:%-3d",@1.first_line);
                                            debug ("initializer ::= '{' initializer.list '}' \n");
                                            $$ = $2 ;
                                        }
        | '{' initializer.list ',' '}'  {
                                            /* 本条规约规则有用吗？有用!现在js支持列表最后多个逗号 */
                                            line("Line:%-3d",@1.first_line);
                                            debug ("initializer ::= '{' initializer.list ',' '}' \n");
                                            $$ = $2 ;
                                        }
        | exp                           {
                                            line("Line:%-3d",@1.first_line);
                                            debug ("initializer ::= exp \n");
                                            $$ = $1 ;
                                        }
        ;

initializer.list:
          initializer   {
                            line("Line:%-3d",@1.first_line);
                            debug ("initializer.list ::= initializer \n");
                            $$ = new initNode((Loc*)&(@1));
                        }
        | initializer.list ',' initializer  {
                            line("Line:%-3d",@1.first_line);
                            debug ("initializer.list ::= initializer.list ',' initializer \n");
                            static_cast<initNode*>$1->value.push_back($3);
                            $$ = $1 ;
                        }
        ;
/*************************************************************************/
/*              1.2 function.definition 函数声明                          */
/*                      1.2.1 parameter.list                             */
/*                      1.2.1 function.body {函数体}                      */
/*************************************************************************/
function.definition:
          type.specifier IDENTIFIER '(' ')' function.body {
                line("Line:%-3d",@1.first_line);
                debug ("function.definition ::= type.specifier %s '(' ')' function.body \n",$2->c_str());
                idNode *id = new idNode(*($2),(Loc*)&(@2));
                $$ = new funcDclNode((primNode*)$1,id,NULL,(funcBodyNode*)$5) ;
        }
        | type.specifier IDENTIFIER '(' parameter.list ')' function.body  {
                line("Line:%-3d",@1.first_line);
                debug ("function.definition ::= type.specifier %s '(' parameter.list ')' function.body \n",$2->c_str());
                idNode *id = new idNode(*($2),(Loc*)&(@2));
                $$ = new funcDclNode((primNode*)$1,id,$4,(funcBodyNode*)$6) ;
        }
        ;

parameter.list:
          parameter.declaration   {
                line("Line:%-3d",@1.first_line);
                debug ("parameter.list ::= parameter.declaration \n");
                list<Node*> *param_List = new list<Node*>();
                param_List->push_back($1);
                $$=param_List;
          }
        | parameter.list ',' parameter.declaration {
                line("Line:%-3d",@1.first_line);
                debug ("parameter.list ::= parameter.list ',' parameter.declaration \n");
                $1->push_back($3);
                $$ = $1 ;
          }
        | parameter.declaration '=' initializer {
                //函数参数里不支持初始化
                error( "Line:%-3d parameter.list in function definations cannot have initializers\n",@1.first_line);
                exit(-1);
          }
        | parameter.list ',' error
        ;
parameter.declaration:
          type.specifier IDENTIFIER {
                                          line("Line:%-3d",@1.first_line);
                                          debug ("parameter.declaration ::= type.specifier %s \n",$2->c_str());
                                          idNode*id = new idNode(*($2),(Loc*)&(@2));
                                          $$ = new paramDeclNode((primNode*)$1,id,NULL,(Loc*)&(@2) );
                                    }
        | type.specifier IDENTIFIER array.declarator  {
                                          line("Line:%-3d",@1.first_line);
                                          debug ("parameter.declaration ::= type.specifier %s array.declarator \n",$2->c_str());
                                          idNode*id = new idNode(*($2),(Loc*)&(@2));
                                          $$ = new paramDeclNode((primNode*)$1,id,(adclNode*)$3,(Loc*)&(@2) );
                                    }
        ;
function.body:
          lblock rblock                   {
                                          line("Line:%-3d",@1.first_line);
                                          debug ("function.body ::= '{' '}' \n");
                                          $$ = new funcBodyNode(NULL) ;
                                          }
        | lblock statement.list rblock    {
                                          line("Line:%-3d",@1.first_line);
                                          debug ("function.body ::= '{' statement.list '}' \n");
                                          $$ = new funcBodyNode($2) ;
                                    }
        ;

statement.list:
          statement                   { 
                                          list<Node*> *stmt_list=new list<Node*> ();
                                          stmt_list->push_back($1);
                                          $$ = stmt_list ;
                                      }
        | statement.list statement    { 
                                          $1->push_back($2);
                                          $$=$1;
                                      }
        ;


/*************************************************************************/
/*              1.3 composite.definition 数据流计算单元声明                */
/*                      1.3.1 composite.head                             */
/*                      1.3.2 composite.body                             */
/*************************************************************************/
composite.definition:
      composite.head composite.body {
                                          line("Line:%-3d",@1.first_line);
                                          debug ("composite.definition ::= composite.head composite.body \n");
                                          $$ = new compositeNode((compHeadNode*)$1,(compBodyNode*)$2) ;
                                    }
    ;
composite.head:
      COMPOSITE IDENTIFIER '(' composite.head.inout ')'   {
            line("Line:%-3d",@1.first_line);
            debug ("composite.head ::= COMPOSITE %s '(' composite.head.inout ')' \n",$2->c_str());
            idNode *id = new idNode(*($2),(Loc*)&(@2));
            $$ = new compHeadNode(id,(ComInOutNode*)$4) ;
      }
    ;
composite.head.inout:
      /*empty*/                                                                           { $$ = NULL ;}
    | INPUT composite.head.inout.member.list                                              { $$ = new ComInOutNode($2,NULL, (Loc*)&(@1))  ; }
    | INPUT composite.head.inout.member.list ',' OUTPUT composite.head.inout.member.list  { $$ = new ComInOutNode($2,$5,   (Loc*)&(@1))  ; }
    | OUTPUT composite.head.inout.member.list                                             { $$ = new ComInOutNode(NULL,$2, (Loc*)&(@1))  ; }
    | OUTPUT composite.head.inout.member.list ',' INPUT composite.head.inout.member.list  { $$ = new ComInOutNode($5,$2,   (Loc*)&(@1))  ; }
    ;
composite.head.inout.member.list:
      composite.head.inout.member         { 
                  list<Node*> *inout_List = new list<Node*>();
                  inout_List->push_back($1);
                  $$ = inout_List ; 
            }
    | composite.head.inout.member.list ',' composite.head.inout.member     { 
                  $1->push_back($3);
                  $$ = $1; 
            }
    ;
composite.head.inout.member:
      stream.type.specifier IDENTIFIER    { 
                  line("Line:%-3d",@1.first_line);
                  debug ("composite.head.inout.member ::= stream.type.specifier IDENTIFIER  \n");
                  idNode *id = new idNode(*($2),(Loc*)&(@2));
                  $$ = new inOutdeclNode($1,id,(Loc*)&(@2)) ; 
            }
    ;
/*************************************************************************/
/*                      1.3.2 composite.body                             */
/*                        1.3.2.1 composite.body.param.opt               */
/*                        1.3.2.2 composite.body.declaration.list        */
/*                        1.3.2.3 composite.body.statement.list          */
/*************************************************************************/
composite.body:
          lblock composite.body.param.opt composite.body.statement.list rblock     { $$ = new compBodyNode((paramNode*)$2,$3) ; }
        ;
composite.body.param.opt:
          /*empty*/                 { $$ = NULL ; }
        | PARAM parameter.list ';'  {
                                          line("Line:%-3d",@1.first_line);
                                          debug ("composite.body.param.opt ::= PARAM parameter.list \n");
                                          $$ = new paramNode($2) ;
                                    }
        ;
composite.body.statement.list:
          costream.composite.statement                                { 
                                                                        list<Node *> *body_List=new list<Node *>();
                                                                        if(!$1) body_List->push_back($1);
                                                                        $$ = body_List ; 
                                                                      }
        | composite.body.statement.list costream.composite.statement  { 
                                                                        $1->push_back($2);
                                                                        $$ = $1 ; 
                                                                      }
        ;
costream.composite.statement:
          composite.body.operator   {
                                          line("Line:%-3d",@1.first_line);
                                          debug ("costream.composite.statement ::= composite.body.operator \n");
                                          $$ = $1 ;
                                    }
        | statement                 {
                                          line("Line:%-3d",@1.first_line);
                                          debug ("costream.composite.statement ::= statement \n");
                                          $$ = $1 ;
                                    }
        ;

/*****************************************************************************/
/*        2. composite.body.operator  composite体内的init work window等组件   */
/*             2.1   ADD operator.pipeline                                   */
/*             2.2   ADD operator.splitjoin                                  */
/*             2.3   ADD operator.default.call                               */
/*****************************************************************************/
composite.body.operator:
          operator.file.writer      {
                                          line("Line:%-3d",@1.first_line);
                                          debug ("composite.body.operator ::= operator.file.writer \n");
                                          $$ = $1 ;
                                    }
        | operator.add              {
                                          line("Line:%-3d",@1.first_line);
                                          debug ("composite.body.operator ::= operator.add \n");
                                          $$ = $1 ;
                                    }
        ;
operator.file.writer:
          FILEWRITER '(' IDENTIFIER ')' '(' stringConstant ')' ';' { $$ = NULL ; }
        | FILEWRITER '(' IDENTIFIER ')' '(' ')' ';' {
                                                       error("Line:%d FILEWRITER must have the filename of the output file.\n",@1.first_line);
                                                    }
        ;
operator.add:
          ADD operator.pipeline     {
                                          line("Line:%-3d",@1.first_line);
                                          debug ("operator.add ::= ADD operator.pipeline \n");
                                          $$ = new addNode((pipelineNode*)$2,(Loc*)&(@1)) ;
                                    }
        | ADD operator.splitjoin    {
                                          line("Line:%-3d",@1.first_line);
                                          debug ("operator.add ::= ADD operator.splitjoin \n");
                                          $$ = new addNode((splitNode*)$2,(Loc*)&(@1)) ;
                                    }
        | ADD operator.default.call {
                                          line("Line:%-3d",@1.first_line);
                                          debug ("operator.add ::= ADD operator.default.call \n");
                                          $$ = new addNode((OperdclNode*)$2,(Loc*)&(@1)) ;
                                    }
        ;
operator.pipeline:
          PIPELINE lblock  splitjoinPipeline.statement.list rblock     { $$ = new pipelineNode($3,(Loc*)&(@1)) ; }
        ;
splitjoinPipeline.statement.list:
          statement                                       {
                                                                line("Line:%-3d",@1.first_line);
                                                                debug ("splitjoinPipeline.statement.list ::= statement \n");
                                                                list<Node*> *split_pipe=new list<Node*>();
                                                                split_pipe->push_back($1);
                                                                $$ = split_pipe ;
                                                          }
        | operator.add                                    {
                                                                line("Line:%-3d",@1.first_line);
                                                                debug ("splitjoinPipeline.statement.list ::= operator.add \n");
                                                                list<Node*> *add_list=new list<Node*>();
                                                                add_list->push_back($1);
                                                                $$ = add_list ;
                                                          }
        | splitjoinPipeline.statement.list statement      {
                                                                line("Line:%-3d",@1.first_line);
                                                                debug ("splitjoinPipeline.statement.list ::= splitjoinPipeline.statement.list statement \n");
                                                                $1->push_back($2);
                                                                $$ = $1;
                                                          }
        | splitjoinPipeline.statement.list operator.add   {
                                                                line("Line:%-3d",@1.first_line);
                                                                debug ("splitjoinPipeline.statement.list ::= splitjoinPipeline.statement.list operator.add \n");
                                                                $1->push_back($2);
                                                                $$ = $1;
                                                          }
        ;
operator.splitjoin:
          SPLITJOIN lblock split.statement  splitjoinPipeline.statement.list  join.statement rblock     {
                   $$=new splitjoinNode((splitNode*)$3,NULL,$4,(joinNode*)$5,(Loc*)&(@1));
            }
        | SPLITJOIN lblock statement.list split.statement splitjoinPipeline.statement.list join.statement rblock  {
                   $$=new splitjoinNode((splitNode*)$4,$3,$5,(joinNode*)$6,(Loc*)&(@1));
            }
        ;
split.statement:
          SPLIT duplicate.statement                        { $$ = new splitNode("duplicate" ,(duplicateNode*)$2,(Loc*)&(@1)) ; }
        | SPLIT roundrobin.statement                       { $$ = new splitNode("roundrobin",(duplicateNode*)$2,(Loc*)&(@1)) ; }
        ;
roundrobin.statement:
          ROUNDROBIN '(' ')' ';'                            { $$ = new roundrobinNode(NULL,(Loc*)&(@1)) ; }
        | ROUNDROBIN '(' argument.expression.list ')' ';'   { $$ = new roundrobinNode($3,(Loc*)&(@1)) ; }
        ;
duplicate.statement:
          DUPLICATE '('  ')' ';'                            { $$ = new duplicateNode(NULL,(Loc*)&(@1)) ; }
        | DUPLICATE '(' exp ')'  ';'                        { $$ = new duplicateNode((expNode*)$3,(Loc*)&(@1))  ;}
        ;
join.statement:
          JOIN roundrobin.statement                         { $$ = new joinNode((roundrobinNode*)$2,(Loc*)&(@1)) ;}
        ;
argument.expression.list:
          exp                                               { 
                                                              list<Node*> *arg_List=new list<Node*>();
                                                              arg_List->push_back($1);
                                                              $$ = arg_List ; 
                                                            }
        | argument.expression.list ',' exp                  { 
                                                              $1->push_back($3);
                                                              $$ = $1 ; 
                                                            }
        ;
operator.default.call:
          IDENTIFIER  '(' ')' ';'                           { 
                                                              $$ = new OperdclNode(*($1),NULL,(Loc*)&(@1)); 
                                                              /*需要查找符号表*/
                                                            }
        | IDENTIFIER  '(' argument.expression.list ')' ';'  {
                                                              /*composite call(StreamIt style)*///operator.param.list 不能为空以区分函数调用/*composite call*/
                                                              ///*DEBUG*/printf("have found operator.default.call\n");
                                                              $$ = new OperdclNode(*($1),$3,(Loc*)&(@1));
                                                              /* 需要查找符号表 */
                                                            }
        ;

/*************************************************************************/
/*        3. statement 花括号内以';'结尾的结构是statement                  */
/*************************************************************************/
statement:
          labeled.statement
        | compound.statement            /* 复合类型声明  */
        | expression.statement
        | selection.statement
        | iteration.statement
        | jump.statement
        | declaration
        | ';'       {
                      $$ = NULL ;
                    }
        | error ';' {  $$ = NULL ; }
        ;

labeled.statement:
          CASE exp ':' statement                    {
                                                          line("Line:%-3d",@1.first_line);
                                                          debug ("labeled.statement ::= CASE exp ':' statement \n");
                                                          $$ = new caseNode((expNode*)$2,(statNode*)$4,(Loc*)&(@3)) ;
                                                    }
        | DEFAULT ':' statement                     {
                                                          line("Line:%-3d",@1.first_line);
                                                          debug ("labeled.statement ::= DEFAULT ':' statement \n");
                                                          $$ = new defaultNode((statNode*)$3,(Loc*)&(@2)) ;
                                                    }
        ;
compound.statement:
          lblock rblock                                     {  $$ = new blockNode(NULL,(Loc*)&(@1),(Loc*)&(@2)); }
        | lblock composite.body.statement.list rblock       {  $$ = new blockNode($2,(Loc*)&(@1),(Loc*)&(@3)) ; }
        ;

expression.statement:
          exp ';'  {  $$ = $1 ; }
        ;

selection.statement:
          IF '(' exp ')' costream.composite.statement   {  $$ = new ifNode((expNode*)$3,$5,(Loc*)&(@1)) ; }
        | IF '(' exp ')' costream.composite.statement 
          ELSE costream.composite.statement             {
                                                          line("Line:%-3d",@1.first_line);
                                                          debug ("selection.statement ::= if(exp) costream.composite.statement else ...\n");
                                                          $$ = new ifElseNode((expNode*)$3,$5,$7,(Loc*)&(@1));
                                                        }
        | SWITCH '(' exp ')' statement                  {  $$ = new switchNode((expNode*)$3,(statNode*)$5,(Loc*)&(@1)); }
        ;
iteration.statement:
          WHILE '(' exp ')' costream.composite.statement                          {  $$ = new whileNode((expNode*)$3,$5,(Loc*)&(@1)) ; }
        | DO  costream.composite.statement WHILE '(' exp ')' ';'                  {  $$ = new doNode($2,(expNode*)$5,(Loc*)&(@1)) ; }
        | FOR '(' exp   ';' exp ';' exp ')'  costream.composite.statement     {
                  line("Line:%-3d",@1.first_line);
                  debug ("iteration.statement ::= for(...)  costream.composite.statement \n");
                  $$ = new forNode($3,(expNode*)$5,(expNode*)$7,$9,(Loc*)&(@1)) ;
            }
        | FOR '(' declaration  ';' exp ';' exp ')'  costream.composite.statement  {  
                  $$ = new forNode($3,(expNode*)$5,(expNode*)$7,$9,(Loc*)&(@1)) ; 
            }
        | FOR '(' error ')' costream.composite.statement                          {  $$ = NULL ; }
        ;
jump.statement:
          CONTINUE ';'        {  $$ = new continueNode((Loc*)&(@1)) ; }
        | BREAK ';'           {  $$ = new breakNode((Loc*)&(@1)) ; }
        | RETURN exp ';'      {  $$ = new returnNode((expNode*)$2,(Loc*)&(@1)) ; }
        ;

/*************************************************************************/
/*        4. exp 计算表达式头节点                        */
/*************************************************************************/
assignment.operator:
          '='             { $$ = new string("=")  ; }
        | MULTassign      { $$ = new string("*=") ; }
        | DIVassign       { $$ = new string("/=") ; }
        | MODassign       { $$ = new string("%=") ; }
        | PLUSassign      { $$ = new string("+=") ; }
        | MINUSassign     { $$ = new string("-=") ; }
        | LSassign        { $$ = new string("<=") ; }
        | RSassign        { $$ = new string(">=") ; }
        | ANDassign       { $$ = new string("&=") ; }
        | ERassign        { $$ = new string("^=") ; }
        | ORassign        { $$ = new string("|=") ; }
        ;
exp.assignable:
          IDENTIFIER                        
            { 
                  line("Line:%-3d",@1.first_line);debug ("exp.assignable ::= %s\n",$1->c_str());
                  //if(S[*($1)]==NULL) error("IDENTIFIER undeclared");
                  $$ = new idNode(*($1),(Loc*)&(@1));

            }
        | IDENTIFIER  array.declarator      
            { 
                  line("Line:%-3d",@1.first_line);debug ("exp.assignable ::= %s array.declarator\n",$1->c_str());
                  ((adclNode*)$2)->name= *($1);
                  $$=$2;
            }  
        ; 
exp:      exp.assignable                    {
              line("Line:%-3d",@1.first_line);debug ("exp ::= exp.assignable\n"); $$ = $1 ; 
              }
        | exp.assignable '.' IDENTIFIER     { 
              line("Line:%-3d",@1.first_line);debug ("exp ::= exp.assignable '.' %s\n",$3->c_str()); 
              idNode *id=new idNode(*($3),(Loc*)&(@3));
              $$ = new pointNode($1,id,(Loc*)&(@2)) ; 
            }
        | exp.assignable '.' IDENTIFIER array.declarator 
            { 
              line("Line:%-3d",@1.first_line);debug ("exp ::= exp.assignable '.' %s array.declarator\n",$3->c_str()); 
              ((adclNode*)$4)->name= *($3);
              $$ = new pointNode($1,$4,(Loc*)&(@2)) ; 
            }
        | constant        { line("Line:%-3d",@1.first_line);debug ("exp ::= constant\n"); $$ = $1 ; }
        | exp '+' exp   { 
                              line("Line:%-3d",@1.first_line);debug ("exp ::= exp + exp\n"); 
                              $$ = new binopNode((expNode*)$1,"+",(expNode*)$3,(Loc*)&(@2)) ; 
                        }
        | exp '-' exp   {
                              line("Line:%-3d",@1.first_line);debug ("exp ::= exp - exp\n");  
                              $$ = new binopNode((expNode*)$1,"-",(expNode*)$3,(Loc*)&(@2)) ; 
                        }
        | exp '*' exp   {    
                              line("Line:%-3d",@1.first_line);debug ("exp ::= exp * exp\n"); 
                              $$ = new binopNode((expNode*)$1,"*",(expNode*)$3,(Loc*)&(@2)) ; 
                        }
        | exp '/' exp   { 
                              line("Line:%-3d",@1.first_line);debug ("exp ::= exp / exp\n"); 
                              $$ = new binopNode((expNode*)$1,"/",(expNode*)$3,(Loc*)&(@2)) ; 
                        }
        | exp '%' exp   { 
                              line("Line:%-3d",@1.first_line);debug ("exp ::= exp %% exp\n");
                              $$ = new binopNode((expNode*)$1,"%",(expNode*)$3,(Loc*)&(@2)) ;
                        }
        | exp OROR exp  { 
                              line("Line:%-3d",@1.first_line);debug ("exp ::= exp || exp\n"); 
                              $$ = new binopNode((expNode*)$1,"||",(expNode*)$3,(Loc*)&(@2)) ;
                        }

        | exp ANDAND exp{ 
                              line("Line:%-3d",@1.first_line);debug ("exp ::= exp && exp\n"); 
                              $$ = new binopNode((expNode*)$1,"&&",(expNode*)$3,(Loc*)&(@2)) ;
                        }
        | exp '|' exp   {     
                              line("Line:%-3d",@1.first_line);debug ("exp ::= exp | exp\n"); 
                              $$ = new binopNode((expNode*)$1,"|",(expNode*)$3,(Loc*)&(@2)) ;
                        }
        | exp '&' exp   { 
                              line("Line:%-3d",@1.first_line);debug ("exp ::= exp & exp\n"); 
                              $$ = new binopNode((expNode*)$1,"&",(expNode*)$3,(Loc*)&(@2)) ;
                        }
        | exp '^' exp   { 
                              line("Line:%-3d",@1.first_line);debug ("exp ::= exp ^ exp\n");
                              $$ = new binopNode((expNode*)$1,"^",(expNode*)$3,(Loc*)&(@2)) ;
                        }
        | exp LS exp    { 
                              line("Line:%-3d",@1.first_line);debug ("exp ::= exp << exp\n"); 
                              $$ = new binopNode((expNode*)$1,"<<",(expNode*)$3,(Loc*)&(@2)) ;
                        }
        | exp RS exp    {     
                              line("Line:%-3d",@1.first_line);debug ("exp ::= exp >> exp\n"); 
                              $$ = new binopNode((expNode*)$1,">>",(expNode*)$3,(Loc*)&(@2)) ;
                        }
        | exp '<' exp   { 
                              line("Line:%-3d",@1.first_line);debug ("exp ::= exp < exp\n");
                              $$ = new binopNode((expNode*)$1,"<",(expNode*)$3,(Loc*)&(@2)) ;
                        }
        | exp '>' exp   { 
                              line("Line:%-3d",@1.first_line);debug ("exp ::= exp > exp\n");
                              $$ = new binopNode((expNode*)$1,">",(expNode*)$3,(Loc*)&(@2)) ;
                        }
        | exp LE exp    { 
                              line("Line:%-3d",@1.first_line);debug ("exp ::= exp <= exp\n"); 
                              $$ = new binopNode((expNode*)$1,"<=",(expNode*)$3,(Loc*)&(@2)) ;
                        }
        | exp GE exp    { 
                              line("Line:%-3d",@1.first_line);debug ("exp ::= exp >= exp\n");
                              $$ = new binopNode((expNode*)$1,">=",(expNode*)$3,(Loc*)&(@2)) ;
                        }
        | exp EQ exp    { 
                              line("Line:%-3d",@1.first_line);debug ("exp ::= exp == exp\n");
                              $$ = new binopNode((expNode*)$1,"==",(expNode*)$3,(Loc*)&(@2)) ;
                        }
        | exp NE exp    { 
                              line("Line:%-3d",@1.first_line);debug ("exp ::= exp != exp\n"); 
                              $$ = new binopNode((expNode*)$1,"!=",(expNode*)$3,(Loc*)&(@2)) ;
                        }
        | exp '?' exp ':' exp { 
                              line("Line:%-3d",@1.first_line);debug ("exp ::= exp ? exp : exp\n"); 
                              $$ = new ternaryNode((expNode*)$1,(expNode*)$3,(expNode*)$5,(Loc*)&(@4));
                        }
        | '+' exp       { 
                              line("Line:%-3d",@1.first_line);debug ("exp ::= + exp\n"); 
                              $$ = new unaryNode("+",(expNode*)$2,(Loc*)&(@2)) ; 
                        }
        | '-' exp       { 
                              line("Line:%-3d",@1.first_line);debug ("exp ::= - exp\n"); 
                              $$ = new unaryNode("-",(expNode*)$2,(Loc*)&(@2)) ; 
                        }
        | '~' exp       {     
                              line("Line:%-3d",@1.first_line);debug ("exp ::= ~ exp\n");
                              $$ = new unaryNode("~",(expNode*)$2,(Loc*)&(@2)) ; 
                        }
        | '!' exp       { 
                              line("Line:%-3d",@1.first_line);debug ("exp ::= ! exp\n"); 
                              $$ = new unaryNode("!",(expNode*)$2,(Loc*)&(@2)) ; 
                        }
        |  ICR exp      {     
                              line("Line:%-3d",@1.first_line);debug ("exp ::= ++ exp\n");
                              $$ = new unaryNode("PREINC",(expNode*)$2,(Loc*)&(@2)) ; 
                        }
        |  DECR exp     {
                              line("Line:%-3d",@1.first_line);debug ("exp ::= -- exp\n");
                              $$ = new unaryNode("PREDEC",(expNode*)$2,(Loc*)&(@2)) ; 
                        }
        |  exp ICR      { 
                              line("Line:%-3d",@1.first_line);debug ("exp ::= exp ++ \n");
                              $$ = new unaryNode("POSTINC",(expNode*)$1,(Loc*)&(@2)) ; 
                        }
        |  exp DECR     {     
                              line("Line:%-3d",@1.first_line);debug ("exp ::= exp --\n");
                              $$ = new unaryNode("POSTDEC",(expNode*)$1,(Loc*)&(@2)) ; 
                        }
        |  '(' exp ')'  {     
                              line("Line:%-3d",@1.first_line);debug ("exp ::= ( exp )\n"); 
                              ((expNode*)$2)->parenthesized=true;
                              $$=$2;
                        }
        | '(' basic.type.name ')' exp     { 
                              line("Line:%-3d",@1.first_line);debug ("exp ::= ( type ) exp\n");
                              $$ = new castNode((primNode*)$2,(expNode*)$4,(Loc*)&(@3)); 
                        }
        | exp assignment.operator exp     { 
                              line("Line:%-3d",@1.first_line);
                              debug ("exp ::= exp.assignable assignment.operator exp\n"); 
                              $$ = new binopNode((expNode*)$1,*($2),(expNode*)$3,(Loc*)&(@2) ) ;
                        }
        | IDENTIFIER '(' argument.expression.list ')'         {   line("Line:%-3d",@1.first_line);debug ("exp ::= function ( exp.list )\n"); 
                                                                  $$ = new callNode(*($1),$3,(Loc*)&(@1)) ; 
                                                              }
        | FILEREADER '(' ')' '(' stringConstant ')'   { 
                  line("Line:%-3d",@1.first_line);
                  debug ("exp ::= FILEREADER()( stringConstant )\n"); 
                  $$ = NULL ; 
            }
        | IDENTIFIER '('  ')' operator.selfdefine.body   { 
                  line("Line:%-3d",@1.first_line);
                  debug ("exp ::= %s() operator.selfdefine.body\n",$1->c_str());
                  /* 这里处理不知道会不会有问题  暂时先这么处理 */
                  $$ = new operatorNode(*($1),NULL,(operBodyNode*)$4) ; 
            }
        | IDENTIFIER '(' argument.expression.list ')' operator.selfdefine.body   { 
                  $$ = new operatorNode(*($1),$3,(operBodyNode*)$5) ; 
            }
        | IDENTIFIER '('  ')'  '(' ')'  { 
                  line("Line:%-3d",@1.first_line);
                  debug ("exp ::= %s()()\n",$1->c_str()); 
                  $$ = new compsiteCallNode(*($1),NULL,NULL,(Loc*)&(@1)) ; 
            }
        | IDENTIFIER '('  ')'  '(' argument.expression.list ')' { 
                  $$ = new compsiteCallNode(*($1),NULL,$5,(Loc*)&(@1)) ; 
            }
        | IDENTIFIER '(' argument.expression.list ')'  '(' ')'  { 
                  $$ = new compsiteCallNode(*($1),$3,NULL,(Loc*)&(@1)) ; 
            }
        | IDENTIFIER '(' argument.expression.list ')'  '(' argument.expression.list ')'    { 
                  $$ = new compsiteCallNode(*($1),$3,$6,(Loc*)&(@1)) ; 
            }
        |  SPLITJOIN '(' argument.expression.list ')'  lblock split.statement  splitjoinPipeline.statement.list  join.statement rblock { 
            /*    1.argument.expression.list是一个identifier
                  2.查找符号表 identifier是否出现过 */
                  $$ = new splitjoinNode((splitNode*)$6,NULL,$7,(joinNode*)$8,(Loc*)&(@1))  ; 
            }
        |  SPLITJOIN '(' argument.expression.list ')'  lblock statement.list split.statement splitjoinPipeline.statement.list  join.statement rblock  { 
                  /*    1.argument.expression.list是一个identifier
                  2.查找符号表 identifier是否出现过 */
                  $$ = new splitjoinNode((splitNode*)$7,$6,$8,(joinNode*)$9,(Loc*)&(@1))  ;  
            }
        |   PIPELINE '(' argument.expression.list ')'  lblock splitjoinPipeline.statement.list rblock                                                 {
                   /*    1.argument.expression.list是一个identifier
                  2.查找符号表 identifier是否出现过 */
                  $$ = new pipelineNode($6,(Loc*)&(@1)) ; 
            }
        ;

operator.selfdefine.body:
       lblock operator.selfdefine.body.init operator.selfdefine.body.work operator.selfdefine.body.window.list rblock
        {
            line("Line:%-3d",@1.first_line);
            debug ("operator.selfdefine.body ::=  { init work window.list }\n");
            $$ = new operBodyNode(NULL,$2,$3,(windowNode*)$4) ;
        }
     | lblock statement.list operator.selfdefine.body.init  operator.selfdefine.body.work operator.selfdefine.body.window.list rblock
        {
            line("Line:%-3d",@1.first_line);
            debug ("operator.selfdefine.body ::=  { statement.list init work window.list }\n");
            $$ = new operBodyNode($2,$3,$4,(windowNode*)$5);
        }
     ;

operator.selfdefine.body.init:
      /*empty*/{ $$ = NULL ; }
    | INIT compound.statement { $$ = $2 ; }
    ;

operator.selfdefine.body.work:
      WORK compound.statement { $$ = $2 ; }
    ;

operator.selfdefine.body.window.list:
      /*empty*/                                         { $$ = NULL ; }
      | WINDOW lblock operator.selfdefine.window.list rblock  {
                                                            line("Line:%-3d",@1.first_line);
                                                            debug ("operator.selfdefine.body.window.list ::= WINDOW { operator.selfdefine.window.list }\n");
                                                            $$ = new windowNode($3) ;
                                                        }
    ;

operator.selfdefine.window.list:
      operator.selfdefine.window                {
                                                      list<Node*> *win_List=new list<Node*>();
                                                      win_List->push_back($1);
                                                      $$=win_List;
      }
    | operator.selfdefine.window.list operator.selfdefine.window{
                                                      $1->push_back($2);
                                                      $$=$1;
                                                }
    ;

operator.selfdefine.window:
      IDENTIFIER window.type ';'                {
                                                    line("Line:%-3d",@1.first_line);
                                                    debug ("operator.selfdefine.window ::= %s window.type (sliding? (arg_list?))\n",$1->c_str());
                                                    $$ = new winStmtNode(*($1),$2,(Loc*)&(@1)) ;
                                                }
    ;

window.type:
      SLIDING '('  ')'                          {
                                                    $$ = new slidingNode(NULL,(Loc*)&(@1)) ;
                                                }
    | TUMBLING '('  ')'                         {
                                                    $$ = new tumblingNode(NULL,(Loc*)&(@1)) ;
                                                }
    | SLIDING '(' argument.expression.list ')'  {
                                                    $$ = new slidingNode($3,(Loc*)&(@1)) ;
                                                }
    | TUMBLING '(' argument.expression.list ')' {
                                                    $$ = new tumblingNode($3,(Loc*)&(@1)) ;
                                                }
    ;


/*************************************************************************/
/*        5. basic 从词法TOKEN直接归约得到的节点,自底向上接入头部文法结构    */
/*************************************************************************/
/* 设置变量作用域相关 */
lblock: '{'  { EnterScope();  }  
rblock: '}'  { ExitScope();   }

constant:
          doubleConstant    {
                                line("Line:%-3d",@1.first_line);
                                debug ("constant ::= doubleConstant | value:=%lf\n",$1);
                                $$ = new constantNode("double",$1,(Loc*)&(@1)) ;
                            }
        | integerConstant   {
                                line("Line:%-3d",@1.first_line);
                                debug ("constant ::= integerConstant | value:=%d\n",$1);
                                $$ = new constantNode("interger",$1,(Loc*)&(@1)) ;
                            }
        | stringConstant    {
                                line("Line:%-3d",@1.first_line);
                                debug ("constant ::= stringConstant | value:=%s\n",$1->c_str());
                                $$ = new constantNode("string",*($1),(Loc*)&(@1)) ;
                            }
        ;
type.specifier:
          basic.type.name       {
                                    line("Line:%-3d",@1.first_line);
                                    debug ("type.specifier ::=  basic.type.name \n");
                                    $$ = $1 ;
                                }
        | CONST basic.type.name {
                                    line("Line:%-3d",@2.first_line);
                                    debug ("type.specifier ::=  CONST basic.type.name \n");
                                    (static_cast<primNode*>$2)->isConst=true;
                                    $$ = $2 ; /* const 暂时还未处理*/
                                }
        ;
basic.type.name:
          INT   {
                      line("Line:%-3d",@1.first_line);
                      debug ("basic.type.name ::=  INT \n");
                      $$ = new primNode("int",(Loc*)&(@1) );
                }
        | LONG  {
                      line("Line:%-3d",@1.first_line);
                      debug ("basic.type.name ::=  LONG \n");
                      $$ = new primNode("LONG",(Loc*)&(@1) ) ;
                }
        | LONG LONG {
                      line("Line:%-3d",@1.first_line);
                      debug ("basic.type.name ::=  LONG LONG  \n");
                      $$ = new primNode("LONG LONG",(Loc*)&(@1) ) ;
                    }
        | FLOAT {
                      line("Line:%-3d",@1.first_line);
                      debug ("basic.type.name ::=  FLOAT \n");
                      $$ = new primNode("FLOAT",(Loc*)&(@1) ) ;
                }
        | DOUBLE{
                      line("Line:%-3d",@1.first_line);
                      debug ("basic.type.name ::=  DOUBLE \n");
                      $$ = new primNode("DOUBLE",(Loc*)&(@1) ) ;
                }
        | STRING{
                      line("Line:%-3d",@1.first_line);
                      debug ("basic.type.name ::=  STRING \n");
                      $$ = new primNode("STRING",(Loc*)&(@1) ) ;
                }
        ;
%%
/* ----语法树结束----*/
void yyerror (const char *msg)
{
    error ("Line:%-4d%s\n",yylloc.first_line, msg);
}
