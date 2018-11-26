%{
#define DEBUG
#include "defines.h"
extern int yylex ();
extern void yyerror (const char *msg);
%}

/*在 union 里声明 %token 可能有的类型*/
%union{
    long long  num;
    std::string *str;
}
/* 1. 下面是从词法分析器传进来的 token ,其中大部分都是换名字符串*/
%token intConstant      stringConstant      err_tok
%token STRING     INT   DOUBLE  LONG        CONST   DEFINE
%token WHILE      FOR   BREAK   CONTINUE    SWITCH  CASE DEFAULT IF ELSE
%token POUNDPOUND ICR   DECR    ANDAND      OROR    LS  RS LE GE EQ NE
%token MULTassign DIVassign     PLUSassign  MINUSassign 
%token LSassign   RSassign ANDassign ERassign ORassign
    /* 1.1 ----------------- COStream 特有关键字 ---------------*/
%token COMPOSITE  INPUT OUTPUT  STREAM    FILEREADER  FILEWRITER  ADD
%token PARAM      INIT  WORK    WINDOW    TUMBLING    SLIDING 
%token SPLITJOIN  PIPELINE      SPLIT     JOIN        DUPLICATE ROUNDROBIN

/* 2.下面是语法分析器自己拥有的文法结构和类型声明 */
/* 2.1 词法分析器传进来的带 value 的文法单元,主要是常量*/
%type<num> intConstant
%type<str> err_tok stringConstant
/* 2.2 语法分析器自己的结构 */
%type<num> expression.constant

/* 3. 优先级标记,从上至下优先级从低到高排列 */
%left '='

/* 4. 语法分析器的起点和坐标声明 */
%start prog.start
%locations

%% 
/********************************************************************************
*                            顶级    节点                                        *
********************************************************************************/
prog.start: translation.unit ;

translation.unit:              
          expression.constant
        | translation.unit expression.constant
        | translation.unit err_tok  {
                                      error("Line:%-3d Unexpected character: %s \n",@2.first_line, $2->c_str());  
                                    } 
        ;

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


%%
/* ----语法树结束----*/
void yyerror (const char *msg)
{
    error ("%s", msg);
}
