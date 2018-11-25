%{
    #define DEBUG
    #include "defines.h"
    extern int yylex ();
    extern void yyerror (const char *msg);
%}

/*在 union 里声明 %token 可能有的类型*/
%union{
    int num;
}
%token intConstant

%type<num> expression.constant intConstant
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
        ;

expression.constant: 
        intConstant {
                        line("Line:%-3d",@1.first_line);
                        debug ("expression.constant ::= intConstant\n");
                        $$ = $1;
                    } 
        ;


%%
/* ----语法树结束----*/
void yyerror (const char *msg)
{
    error ("%s", msg);
}
