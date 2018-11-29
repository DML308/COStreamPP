%{
#define DEBUG
#include "defines.h"
extern int yylex ();
extern void yyerror (const char *msg);
%}

/*在 union 里声明 %token 可能有的类型*/
%union{
    long long  num;
    double doubleNum;
    std::string *str;
    Node * node;
}
/* A. 下面是从词法分析器传进来的 token ,其中大部分都是换名字符串*/
%token intConstant      stringConstant      err_tok
%token STRING     INT   DOUBLE  FLOAT       LONG    CONST   DEFINE
%token WHILE      FOR   BREAK   CONTINUE    SWITCH  CASE DEFAULT IF ELSE
%token POUNDPOUND ICR   DECR    ANDAND      OROR    LS  RS LE GE EQ NE
%token MULTassign DIVassign     PLUSassign  MINUSassign
%token LSassign   RSassign ANDassign ERassign ORassign
    /* A.1 ----------------- COStream 特有关键字 ---------------*/
%token COMPOSITE  INPUT OUTPUT  STREAM    FILEREADER  FILEWRITER  ADD
%token PARAM      INIT  WORK    WINDOW    TUMBLING    SLIDING
%token SPLITJOIN  PIPELINE      SPLIT     JOIN        DUPLICATE ROUNDROBIN

/* B.下面是语法分析器自己拥有的文法结构和类型声明 */

/* 语法分析器自己的结构 1. 文法一级入口*/
%type<num>  expression.constant
%type<node> prog.start translation.unit external.definition
%type<node> declaration function.definition composite.definition
/* 语法分析器自己的结构 1.1.declaration */
%type<node> declaring.list  stream.declaring.list stream.type.specifier stream.declaration.list
/* 语法分析器自己的结构   1.1.3.array */
%type<node> array.declarator
/* 语法分析器自己的结构   1.1.4.initializer */
%type<node> initializer.opt initializer initializer.list
/* 语法分析器自己的结构 1.2 function.definition 函数声明 */
%type<node> function.body   parameter.list  parameter.declaration
/* 语法分析器自己的结构 1.3 composite.definition 数据流计算单元声明 */
%type<node> composite.head composite.body.no.new.scope
/* 语法分析器自己的结构 2.statement 花括号内以';'结尾的结构是statement  */
%type<node> statement.list
/* 语法分析器自己的结构 3.assignment.expression 计算表达式头节点  */
%type<node> assignment.expression constant.expression
/* 语法分析器自己的结构 4.basic 从词法TOKEN直接归约得到的节点 */
%type<node>constant type.specifier basic.type.name
%type<num> integerConstant
%type<doubleNum> doubleConstant
%type<str> err_tok stringConstant



/* C. 优先级标记,从上至下优先级从低到高排列 */
%left '='

/* D. 语法分析器的起点和坐标声明 */
%start prog.start
%locations

%%
/************************************************************************/
/*              1. 文法一级入口,由下面三种文法组成                           */
/*                 1.1. decalration 声明                                 */
/*                 1.2. function.definition 函数声明                      */
/*                 1.3. composite.definition 数据流计算单元声明             */
/************************************************************************ /
prog.start: translation.unit ;

translation.unit:
          external.definition   {
                                      line("Line:%-3d",@1.first_line);
                                      debug ("translation.unit ::= external.definition\n");
                                      $$ = NULL;
                                }
        | translation.unit external.definition
        ;
external.definition:
          declaration           {
                                      line("Line:%-3d",@1.first_line);
                                      debug ("external.definition ::= declaration\n");
                                      $$ = NULL;
                                }
        | function.definition   {
                                      line("Line:%-3d",@1.first_line);
                                      debug ("external.definition ::= function.definition\n");
                                      $$ = NULL;
                                }
        //| composite.definition
        ;
/*************************************************************************/
/*              1.1 decalration 由下面2种文法+2个基础组件组成                */
/*                      1.1.1 declaring.list                             */
/*                      1.1.2 stream.declaring.list                      */
/*                      1.1.3 array                                      */
/*                      1.1.4 initializer                                */
/*************************************************************************/
declaration:
      declaring.list ';'        {
                                      line("Line:%-3d",@1.first_line);
                                      debug ("declaration ::= declaring.list ';' \n");
                                      $$ = NULL;
                                }
    | stream.declaring.list ';' {
                                      line("Line:%-3d",@1.first_line);
                                      debug ("declaration ::= stream.declaring.list ';' \n");
                                      $$ = NULL;

                                }
    ;
declaring.list:
          type.specifier 			  IDENTIFIER initializer.opt  {
              line("Line:%-3d",@1.first_line);
              debug ("declaring.list ::= type.specifier IDENTIFIER initializer.opt \n");
              $$ = NULL;
        }
        | type.specifier 			  IDENTIFIER array.declarator initializer.opt{
              line("Line:%-3d",@1.first_line);
              debug ("declaring.list ::= type.specifier IDENTIFIER array.declarator initializer.opt \n");
              $$ = NULL;
        }
        | declaring.list 	',' 	IDENTIFIER initializer.opt{
              line("Line:%-3d",@1.first_line);
              debug ("declaring.list ::= declaring.list 	',' 	IDENTIFIER initializer.opt \n");
              $$ = NULL;
        }
        | declaring.list 	',' 	IDENTIFIER array.declarator initializer.opt{
              line("Line:%-3d",@1.first_line);
              debug ("declaring.list ::= declaring.list 	',' 	IDENTIFIER array.declarator initializer.opt \n");
              $$ = NULL;
        }
        ;
stream.declaring.list:
          stream.type.specifier IDENTIFIER    {
                                                  line("Line:%-3d",@1.first_line);
                                                  debug ("stream.declaring.list ::= stream.type.specifier IDENTIFIER \n");
                                                  $$ = NULL;
                                              }
        | stream.declaring.list ',' IDENTIFIER{
                                                  line("Line:%-3d",@1.first_line);
                                                  debug ("stream.declaring.list ::= stream.declaring.list ',' IDENTIFIER \n");
                                                  $$ = NULL;
                                              }
        ;
stream.type.specifier:
          STREAM '<' stream.declaration.list '>'{
                                                    line("Line:%-3d",@1.first_line);
                                                    debug ("stream.type.specifier ::=  STREAM '<' stream.declaration.list '>' \n");
                                                    $$ = NULL;
                                                }
        ;
stream.declaration.list:
          type.specifier IDENTIFIER {
                                        line("Line:%-3d",@1.first_line);
                                        debug ("stream.declaration.list ::=  type.specifier IDENTIFIER \n");
                                        $$ = NULL;
                                    }
        | type.specifier IDENTIFIER array.declarator{
                                        line("Line:%-3d",@1.first_line);
                                        debug ("stream.declaration.list ::=  type.specifier IDENTIFIER array.declarator \n");
                                        $$ = NULL;
                                    }
        | stream.declaration.list ',' type.specifier IDENTIFIER {
                                        line("Line:%-3d",@1.first_line);
                                        debug ("stream.declaration.list ::=  stream.declaration.list ',' type.specifier IDENTIFIER \n");
                                        $$ = NULL;
                                    }
        | stream.declaration.list ',' type.specifier IDENTIFIER array.declarator{
                                        line("Line:%-3d",@1.first_line);
                                        debug ("stream.declaration.list ::=  stream.declaration.list ',' type.specifier IDENTIFIER array.declarator \n");
                                        $$ = NULL;
                                    }
        ;

/*************************************************************************/
/*                      1.1.3 array ( int a[] )                          */
/*************************************************************************/
array.declarator:
          '[' ']'   {
                            line("Line:%-3d",@1.first_line);
                            debug ("array.declarator ::= '[' ']' \n");
                            $$ = NULL;
                    }
        | '[' constant.expression ']' {
                            line("Line:%-3d",@1.first_line);
                            debug ("array.declarator ::= '[' constant.expression ']' \n");
                            $$ = NULL;
                    }
        | array.declarator '[' constant.expression ']'  {
                            line("Line:%-3d",@1.first_line);
                            debug ("array.declarator ::= array.declarator '[' constant.expression ']' \n");
                            $$ = NULL;
                    }
        | array.declarator '[' ']'  {
                            line("Line:%-3d",@1.first_line);
                            error ("array declaration with illegal empty dimension\n");
                            exit(113);
                    }
        ;
/*************************************************************************/
/*                      1.1.4 initializer                                */
/*************************************************************************/
initializer.opt:
          /* nothing */         {
                                    //line("Line:%-3d",@1.first_line);
                                    debug ("initializer.opt ::= nothing \n");
                                    $$ = NULL;
                                }
        | '=' initializer       {
                                    line("Line:%-3d",@1.first_line);
                                    debug ("initializer.opt ::= '=' initializer \n");
                                    $$ = $2;
                                }
        ;
initializer:
          '{' initializer.list '}'      {
                                            line("Line:%-3d",@1.first_line);
                                            debug ("initializer ::= '{' initializer.list '}' \n");
                                            $$ = $2;
                                            //$$->coord = $1;
                                        }
        | '{' initializer.list ',' '}'  {
                                            /* 本条规约规则有用吗？有用!现在js支持列表最后多个逗号 */
                                            line("Line:%-3d",@1.first_line);
                                            debug ("initializer ::= '{' initializer.list ',' '}' \n");
                                            $$ = $2;
                                            //$$->coord = $1;
                                        }
        | assignment.expression         {

                                            line("Line:%-3d",@1.first_line);
                                            debug ("initializer ::= assignment.expression \n");
                                            $$ = $1;
                                        }
        ;

initializer.list:
          initializer   {
                            line("Line:%-3d",@1.first_line);
                            debug ("initializer.list ::= initializer \n");
                            $$ =NULL;
                        }
        | initializer.list ',' initializer  {
                            line("Line:%-3d",@1.first_line);
                            debug ("initializer.list ::= initializer.list ',' initializer \n");
                            $$ =NULL;
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
                debug ("function.definition ::= type.specifier IDENTIFIER '(' ')' function.body \n");
                $$ =NULL;
        }
        | type.specifier IDENTIFIER '(' parameter.list ')' function.body  {
                line("Line:%-3d",@1.first_line);
                debug ("function.definition ::= type.specifier IDENTIFIER '(' parameter.list ')' function.body \n");
                $$ =NULL;
        }
        ;

parameter.list:
          parameter.declaration   {
                line("Line:%-3d",@1.first_line);
                debug ("parameter.list ::= parameter.declaration \n");
                $$ =NULL;
          }
        | parameter.list ',' parameter.declaration      /* 含有多个参数 */{
                line("Line:%-3d",@1.first_line);
                debug ("parameter.list ::= parameter.list ',' parameter.declaration \n");
                $$ =NULL;
        }
        | parameter.declaration '=' initializer {
                //函数参数里不支持初始化
                line("Line:%-3d",@1.first_line);
                error( "parameter.list in function definations cannot have initializers");
                exit(121);
        }
        | parameter.list ',' err_tok
        ;
parameter.declaration:
          type.specifier IDENTIFIER {
                                          line("Line:%-3d",@1.first_line);
                                          debug ("parameter.declaration ::= type.specifier IDENTIFIER \n");
                                          $$ =NULL;
                                    }
        | type.specifier IDENTIFIER array.declarator  {
                                          line("Line:%-3d",@1.first_line);
                                          debug ("parameter.declaration ::= type.specifier IDENTIFIER array.declarator \n");
                                          $$ =NULL;
                                    }
        ;
function.body:
          '{' '}'                   {
                                          line("Line:%-3d",@1.first_line);
                                          debug ("function.body ::= '{' '}' \n");
                                          $$ =NULL;
                                    }
        | '{' statement.list '}'    {
                                          line("Line:%-3d",@1.first_line);
                                          debug ("function.body ::= '{' statement.list '}' \n");
                                          $$ =NULL;
                                    }
        ;

/*************************************************************************/
/*              1.3 composite.definition 数据流计算单元声明                */
/*                      1.3.1 composite.head                             */
/*                      1.3.2 composite.body                             */
/*************************************************************************/
composite.definition:
      composite.head composite.body.no.new.scope
    ;
composite.head:
      COMPOSITE IDENTIFIER '(' composite.head.inout ')'
    ;
composite.head.inout:
      /*empty*/             { $$ = NULL; }
    | INPUT composite.head.inout.member.list
    | INPUT composite.head.inout.member.list ',' OUTPUT composite.head.inout.member.list
    | OUTPUT composite.head.inout.member.list
    | OUTPUT composite.head.inout.member.list ',' INPUT composite.head.inout.member.list
    ;
composite.head.inout.member.list:
      composite.head.inout.member
    | composite.head.inout.member.list ',' composite.head.inout.member
    ;
composite.head.inout.member:
      stream.type.specifier IDENTIFIER
    ;
/*************************************************************************/
/*                      1.3.2 composite.body                             */
/*                        1.3.2.1 composite.body.param.opt               */
/*                        1.3.2.2 composite.body.declaration.list        */
/*                        1.3.2.3 composite.body.statement.list          */
/*************************************************************************/
composite.body.no.new.scope:
       '{' composite.body.param.opt composite.body.statement.list '}'
    |  '{' composite.body.param.opt composite.declaration.list composite.body.statement.list '}'
    ;
composite.body.param.opt:
      /*empty*/               { $$ = NULL; }
    | PARAM parameter.list ';'
    ;
composite.declaration.list:
          declaration
        | composite.declaration.list declaration
        ;
composite.body.statement.list:
          costream.composite.statement
        | composite.body.statement.list costream.composite.statement
        ;
costream.composite.statement:
          composite.body.operator
        | statement
        ;
/*************************************************************************/
/*        2. statement 花括号内以';'结尾的结构是statement                  */
/*************************************************************************/
statement:
          labeled.statement
        | compound.statement            /* 复合类型声明  */
        | expression.statement
        | selection.statement
        | iteration.statement
        | jump.statement
        | error ';'
           {  $$ = NULL; }
        ;

labeled.statement:
        | CASE constant.expression ':' statement
        | DEFAULT ':' statement
        ;
compound.statement:
          '{' '}'
        | '{' declaration.list '}'
        | '{' composite.body.statement.list '}'
        | '{' declaration.list composite.body.statement.list '}'
        ;

expression.statement:
          expression.opt ';'
        ;

selection.statement:
          IF '(' expression ')' costream.composite.statement
        | IF '(' expression ')' costream.composite.statement ELSE costream.composite.statement  /* 可以为普通表达式也可以为流声明 */
        | SWITCH '(' expression ')' statement
        ;
iteration.statement:
          WHILE '(' expression ')' costream.composite.statement
        | DO  costream.composite.statement WHILE '(' expression ')' ';'
        | FOR '(' expression.opt ';' expression.opt ';' expression.opt ')'  costream.composite.statement
        | FOR '(' error ';' expression.opt ';' expression.opt ')'  costream.composite.statement
        | FOR '(' expression.opt ';' expression.opt ';' error ')'  costream.composite.statement
        | FOR '(' expression.opt ';' error ';' expression.opt ')'  costream.composite.statement
        | FOR '(' error ')' { ;}  costream.composite.statement
        ;
jump.statement:
        | CONTINUE ';'
        | BREAK ';'
        | RETURN expression.opt ';'
        ;

/*************************************************************************/
/*        3. assignment.expression 计算表达式头节点                         */
/*************************************************************************/
expression.constant:
          intConstant     {
                            line("Line:%-3d",@1.first_line);
                            debug ("expression.constant ::= intConstant | value:=%lld\n",$1);
                            $$ = $1;
                          }
        | stringConstant  {
                            line("Line:%-3d",@1.first_line);
                            debug ("expression.constant ::= stringConstant | value:=%s\n",$1->c_str());
                            $$ = $$;
                          }
        ;
/*************************************************************************/
/*        4. basic 从词法TOKEN直接归约得到的节点,自底向上接入头部文法结构    */
/*************************************************************************/
constant:
          doubleConstant    {
                                line("Line:%-3d",@1.first_line);
                                debug ("constant ::= doubleConstant | value:=%s\n",$1->c_str());
                                $$ = $$;
                            }
        | integerConstant   {
                                line("Line:%-3d",@1.first_line);
                                debug ("constant ::= integerConstant | value:=%s\n",$1->c_str());
                                $$ = $$;
                            }
        | stringConstant    {
                                line("Line:%-3d",@1.first_line);
                                debug ("constant ::= stringConstant | value:=%s\n",$1->c_str());
                                $$ = $$;
                            }
        ;
type.specifier:
          basic.type.name       {
                                    line("Line:%-3d",@1.first_line);
                                    debug ("type.specifier ::=  basic.type.name \n");
                                    $$ =NULL;
                                }
        | CONST basic.type.name {
                                    line("Line:%-3d",@1.first_line);
                                    debug ("type.specifier ::=  CONST basic.type.name \n");
                                    $$ =NULL;
                                }
        ;
basic.type.name:
          INT   {
                      line("Line:%-3d",@1.first_line);
                      debug ("basic.type.name ::=  INT \n");
                      $$ =NULL;
                }
        | LONG  {
                      line("Line:%-3d",@1.first_line);
                      debug ("basic.type.name ::=  LONG \n");
                      $$ =NULL;
                }
        | LONG LONG{
                      line("Line:%-3d",@1.first_line);
                      debug ("basic.type.name ::=  LONG LONG  \n");
                      $$ =NULL;
                    }
        | FLOAT {
                      line("Line:%-3d",@1.first_line);
                      debug ("basic.type.name ::=  FLOAT \n");
                      $$ =NULL;
                }
        | DOUBLE{
                      line("Line:%-3d",@1.first_line);
                      debug ("basic.type.name ::=  DOUBLE \n");
                      $$ =NULL;
                }
        | STRING{
                      line("Line:%-3d",@1.first_line);
                      debug ("basic.type.name ::=  STRING \n");
                      $$ =NULL;
                }
        ;
%%
/* ----语法树结束----*/
void yyerror (const char *msg)
{
    error ("%s", msg);
}
