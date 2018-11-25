%{
    #define DEBUG
%}

/*状态定义也叫环境定义，它定义了匹配正规式时所处的状态的名字。状态定义以%s开始，后跟所定义的状态的名字，注意%s也要顶行首写https://blog.csdn.net/wp1603710463/article/details/50365495*/
%s C 

/*标识符(变量名) 以字母开头,后面跟着字母或数字或下划线*/
identifier ([a-zA-Z_][0-9a-zA-Z_]*)   

/*指数部分,例如 e3 或 E-3*/
exponent_part ([eE][-+]?[0-9]+)
/*小数部分,例如123.456 或123.或.123 注意 '123.'和'.123'这两种写法是符合标准的,但是看起来很奇怪所以不推荐后来这样写,建议替换为为'123.0'和'0.123'*/
fractional_constant ([0-9]*"."[0-9]+)|([0-9]+".")
/*浮点字面值可以用十进制或科学计数法（指数用E或e）表示，默认为double，在数值后加F或f表示单精度，加L或l表示扩展精度*/
floating_constant ((({fractional_constant}{exponent_part}?)|([0-9]+{exponent_part}))[FfLl]?)
/*参见1.lex.md, u 代表unsigned int的数字; l 代表 long的数字 */
integer_suffix_opt ([uU]?[lL]?[lL]?)|([lL][uU][lL]?)|([lL][lL][uU])
/*10进制数字,例如123456 */
decimal_constant ("0"{integer_suffix_opt}|[1-9][0-9]*{integer_suffix_opt})
/* 8进制数字 */
octal_constant ("0"[0-7]*{integer_suffix_opt}) 
/* 16进制数字 */
hex_constant ("0"[xX][0-9a-fA-F]+{integer_suffix_opt})

/* escape 字符串功能尚不明确,可删去 */
simple_escape ([abfnrtv'"?\\])
octal_escape  ([0-7][^'\n]*)
hex_escape ("x"[^'\n]*)
bad_escape ([\\][^abfnrtv'"?\\x0-7\n ])

escape_sequence ([\\]({simple_escape}|{octal_escape}|{hex_escape}))
c_char ([^'\\\n]|{escape_sequence})
s_char ([^"\\\n]|{escape_sequence})

/*空白字符的代码意义不明,应修改.*/
any_white ([ \011\013\014\015])

Annotations      \/\*(\/\/|[^(\*\/)]|\n)*\*\/
Annotation       \/\/[^\n]*\n
%option yylineno
%%
<INITIAL,C>{Annotation}             {debug("单行注释\n");}
<INITIAL,C>{Annotations}            {debug("多行注释\n");}

%%


//<<flex 与 bison>>第144页:  当词法分析器到达文件的末尾时,它可以选择性地调用yywrap()来了解下一步操作.可以让 yywrap 返回1或者%option noyywrap 来终止词法分析操作.
int yywrap()
{
   return 1;
}