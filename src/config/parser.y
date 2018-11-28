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
/* 1. 下面是从词法分析器传进来的 token ,其中大部分都是换名字符串*/
%token intConstant      stringConstant      err_tok
%token STRING     INT   DOUBLE  FLOAT       LONG    CONST   DEFINE  
%token WHILE      FOR   BREAK   CONTINUE    SWITCH  CASE DEFAULT IF ELSE
%token POUNDPOUND ICR   DECR    ANDAND      OROR    LS  RS LE GE EQ NE
%token MULTassign DIVassign     PLUSassign  MINUSassign 
%token LSassign   RSassign ANDassign ERassign ORassign
    /* 1.1 ----------------- COStream 特有关键字 ---------------*/
%token COMPOSITE  INPUT OUTPUT  STREAM    FILEREADER  FILEWRITER  ADD
%token PARAM      INIT  WORK    WINDOW    TUMBLING    SLIDING 
%token SPLITJOIN  PIPELINE      SPLIT     JOIN        DUPLICATE ROUNDROBIN

/* 2.下面是语法分析器自己拥有的文法结构和类型声明 */

/* 2.2 语法分析器自己的结构 1. 文法一级入口*/
%type<num>  expression.constant
%type<node> prog.start translation.unit external.definition declaration
/* 2.2 语法分析器自己的结构 3.basic */
%type<node>constant type.specifier basic.type.name
%type<num> integerConstant
%type<doubleNum> doubleConstant 
%type<str> err_tok stringConstant

/* 3. 优先级标记,从上至下优先级从低到高排列 */
%left '='

/* 4. 语法分析器的起点和坐标声明 */
%start prog.start
%locations

%% 
/*************************************************************************/
/*        1. 文法一级入口,由下面三种文法组成                                  */
/*              1.1. decalration 声明                                    */
/*              1.2. function.definition 函数声明                         */
/*              1.3. composite.definition 数据流计算单元声明                */
/*************************************************************************/
prog.start: translation.unit ;

translation.unit:              
          external.definition 
        | translation.unit external.definition
        ;
external.definition:           
          declaration
        //| function.definition
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
		  declaring.list ';'
    | stream.declaring.list ';'
		;
declaring.list: 
		      type.specifier 			  IDENTIFIER initializer.opt 
        | type.specifier 			  IDENTIFIER array.declarator initializer.opt  
		    | declaring.list 	',' 	IDENTIFIER initializer.opt 
		    | declaring.list 	',' 	IDENTIFIER array.declarator initializer.opt
        ;
stream.declaring.list:      
          stream.type.specifier IDENTIFIER
        | stream.declaring.list ',' IDENTIFIER
        ;
stream.type.specifier:
		      STREAM '<' stream.declaration.list '>'
		    ;
stream.declaration.list:
		      type.specifier IDENTIFIER 
		    | type.specifier IDENTIFIER array.declarator
		    | stream.declaration.list ',' type.specifier IDENTIFIER
		    | stream.declaration.list ',' type.specifier IDENTIFIER array.declarator
		    ;

/*************************************************************************/
/*                      1.1.3 array ( int a[] )                          */
/*************************************************************************/
array.declarator:
          '[' ']'               
        | '[' constant.expression ']'
        | array.declarator '[' constant.expression ']'      
        | array.declarator '[' ']' { 
                                              line("Line:%-3d",@1.first_line);
                                              error ("array declaration with illegal empty dimension\n");
                                              exit(0);
                                            }
        ;
/*************************************************************************/
/*                      1.1.4 initializer                                */
/*************************************************************************/
initializer.opt:
          /* nothing */                  { $$ = NULL; }
        | '=' initializer                { $$ = $2; }
        ;
initializer: 
          '{' initializer.list '}'       { $$ = $2; $$->coord = $1; }
        | '{' initializer.list ',' '}'   { $$ = $2; $$->coord = $1; }   /* 本条规约规则有用吗？有用!现在js支持列表最后多个逗号 */
        | assignment.expression          { $$ = $1;}		
        ;
initializer.list: /
          initializer  { $$ =NULL; }
        | initializer.list ',' initializer  { $$ =NULL; }
        ;
/*************************************************************************/
/*              1.2 function.definition 函数声明                          */
/*                      1.2.1 parameter.list                             */
/*                      1.2.1 function.body {函数体}                      */
/*************************************************************************/
function.definition:
          type.specifier IDENTIFIER '(' ')' function.body
        | type.specifier IDENTIFIER '(' parameter.list ')' function.body
        ;
parameter.list: 
          parameter.declaration
        | parameter.list ',' parameter.declaration      /* 含有多个参数 */
        | parameter.declaration '=' initializer         { 
                                                          //函数参数里不支持初始化
                                                          line("Line:%-3d",@1.first_line);
                                                          error( "parameter.list in function definations cannot have initializers");
                                                          exit(121);
                                                        }
        | parameter.list ',' err_tok
        ;
parameter.declaration: 
		      type.specifier IDENTIFIER 
        | type.specifier IDENTIFIER array.declarator
        ;
function.body:             
          '{' '}'
        | '{' statement.list '}'
        ;

/*************************************************************************/
/*              1.3 composite.definition 数据流计算单元声明                 */
/*************************************************************************/

/*************************************************************************/
/*        2. assignment.expression 计算表达式头节点                         */
/*************************************************************************/
expression.constant: 
          intConstant {
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
/*        3. basic 从词法TOKEN直接归约得到的节点,自底向上接入头部文法结构    */
/*************************************************************************/
constant: 
          doubleConstant     { $$ = $$; }
        | integerConstant    { $$ = $$; }
        | stringConstant     { $$ = $$; }
        ;
type.specifier:
          basic.type.name
        | CONST basic.type.name
        ;  
basic.type.name:
          INT
        | LONG 
        | LONG LONG
        | FLOAT
        | DOUBLE
        | STRING
        ;
%%
/* ----语法树结束----*/
void yyerror (const char *msg)
{
    error ("%s", msg);
}
