%{
    #include "token.hh"
    #define DEBUG
    #include "defines.h"
    int yycolumn = 1;
    #define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \
        yylloc.first_column = yycolumn; \
        yylloc.last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng;
    
%}

/*标识符(变量名) 以字母开头,后面跟着字母或数字或下划线*/
identifier ([a-zA-Z_][0-9a-zA-Z_]*)   

/*指数部分,例如 e3 或 E-3*/
exponent_part ([eE][-+]?[0-9]+)
/*小数部分,例如123.456 或123.或.123 注意 '123.'和'.123'这两种写法是符合标准的,但是看起来很奇怪所以不推荐后来这样写,建议替换为为'123.0'和'0.123'*/
fractional_constant ([0-9]*"."[0-9]+)|([0-9]+".")
/*浮点字面值可以用十进制或科学计数法（指数用E或e）表示，默认为double，在数值后加F或f表示单精度，加L或l表示扩展精度*/
floating_constant ((({fractional_constant}{exponent_part}?)|([0-9]+{exponent_part}))[FfLl]?)

/*10进制数字,例如123456 */
decimal_constant ([1-9][0-9]*)
/* 8进制数字 */
octal_constant ("0"[0-7]*{integer_suffix_opt}) 
/* 16进制数字 */
hex_constant ("0"[xX][0-9a-fA-F]+{integer_suffix_opt})

c_char ([^'\\\n]|{escape_sequence})
s_char ([^"\\\n]|{escape_sequence})

/*空白字符的代码意义不明,应修改.*/
any_white ([ \011\013\014\015])

Annotations      \/\*(\/\/|[^(\*\/)]|\n)*\*\/
Annotation       \/\/[^\n]*\n

%option noyywrap
%option yylineno
%%
{Annotation}            {debug("单行注释\n");}
{Annotations}           {debug("多行注释\n");}
{decimal_constant}      {
                            yylval.num = atoi (yytext);
                            return intConstant;
                        }
%%
