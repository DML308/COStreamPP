%{
    #define DEBUG
    #include "defines.h"
    #include "token.hh"
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
floating_constant ((({fractional_constant}{exponent_part}?)|([0-9]+{exponent_part})))

/*10进制数字,例如123456 */
decimal_constant ([1-9][0-9]*)
/* 8进制数字 */
octal_constant ("0"[0-7]*) 
/* 16进制数字 */
hex_constant ("0"[xX][0-9a-fA-F]+)

/* 字符串: 考虑到可能会有这样的字符串 "123 \n \" " ,不合规的字符串有很多,放到后续语义分析再处理 */
string_constant ('[^\n]*'|\"[^\n]*\")

/*空白字符的代码只保留 空格 tab 和 \n.*/
any_white ([ \t \n])

Annotations      \/\*(\/\/|[^(\*\/)]|\n)*\*\/
Annotation       \/\/[^\n]*\n

%option noyywrap
%option yylineno
%%
{any_white}             
{Annotation}            {debug("单行注释\n");}
{Annotations}           {debug("多行注释\n");}
{decimal_constant}      {
                          yylval.num = atoi (yytext);
                          return intConstant;
                        }
{string_constant}       {
                          string str = string(yytext);
                          yylval.str = new string(str.substr(1,str.size()-2));
                          return stringConstant;
                        }

string      { return STRING; }
int         { return INT; }
double      { return DOUBLE; } 
long        { return LONG; }
const       { return CONST; }
define      { return DEFINE; }

while       { return WHILE; }                                  
for         { return FOR; }
break       { return BREAK; }
continue    { return CONTINUE; }
switch      { return SWITCH; }              
case        { return CASE; }
default     { return DEFAULT; }               
if          { return IF; }
else        { return ELSE; }  

composite   { return COMPOSITE; }
input       { return INPUT; }
output      { return OUTPUT; }
stream      { return STREAM; }
FileReader  { return FILEREADER; }
FileWriter  { return FILEWRITER; }
add         { return ADD; }

param        { return PARAM; }
init         { return INIT; }
work         { return WORK; }
window       { return WINDOW; }
tumbling     { return TUMBLING; }
sliding      { return SLIDING; }

splitjoin    { return SPLITJOIN; }
pipeline     { return PIPELINE; }
split        { return SPLIT; }
join         { return JOIN; }
duplicate    { return DUPLICATE; }
roundrobin   { return ROUNDROBIN; }

"("          { return *yytext; }
")"          { return *yytext; }
"["          { return *yytext; }
"]"          { return *yytext; }
"{"          { return *yytext; }
"}"          { return *yytext; }
","          { return *yytext; }
"#"          { return *yytext; }

"."          { return '.'; }
"&"          { return '&'; }
"|"          { return '|'; }
"^"          { return '^'; }
"+"          { return '+'; }
"-"          { return '-'; }
"*"          { return '*'; }
"/"          { return '/'; }
"%"          { return '%'; }
"<"          { return '<'; }
">"          { return '>'; }
"~"          { return '~'; }
"!"          { return '!'; }
"?"          { return '?'; }
":"          { return ':'; }
";"          { return ';'; }
"="          { return '='; }

"##"         { return POUNDPOUND; }
"++"         { return ICR; }
"--"         { return DECR; }
"<<"         { return LS; }
">>"         { return RS; }
"<="         { return LE; }
">="         { return GE; }
"=="         { return EQ; }
"!="         { return NE; }
"&&"         { return ANDAND; }
"||"         { return OROR; }
"*="         { return MULTassign; }
"/="         { return DIVassign; }
"+="         { return PLUSassign; }
"-="         { return MINUSassign; }
"<<="        { return LSassign; }
">>="        { return RSassign; }
"&="         { return ANDassign; }
"^="         { return ERassign; }
"|="         { return ORassign; }

.* {
  yylval.str = new string(yytext);
  return err_tok;
}
                        
%%
