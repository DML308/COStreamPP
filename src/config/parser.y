%{
#define DEBUG
#include "defines.h"
extern int yylex ();
extern void yyerror (const char *msg);
%}

/*在 union 里声明 %token 可能有的类型*/
%union{
    int num;
    std::string *str;
}
%token intConstant stringConstant err_tok

%type<num> expression.constant intConstant
%type<str> stringConstant err_tok
/*优先级标记*/
%left '='

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
                        debug ("expression.constant ::= intConstant | value:=%d\n",$1);
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
