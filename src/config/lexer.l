%{
    #pragma ident "c4.l,v 1.21 1995/05/11 18:54:14 rcm Exp" //将指定的字符串放在可执行文件的 .comment 部分。https://blog.csdn.net/lowfly_xy/article/details/5414041
    
    //#include <ctype.h>
    //#define DEBUG
    #include "ast.h"
    #include "token.hh"
    #include "lexerUtils.h"
    int yycolumn = 1;
    #define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \
      yylloc.first_column = yycolumn; \
      yylloc.last_column = yycolumn + yyleng - 1; \
      yycolumn += yyleng;
    
    char *line_start;
  
    #undef YY_INPUT
    #define YY_INPUT(buf, result, max_size) \
        if (fgets(buf, max_size, yyin)==0) { result=0; } \
        else { line_start = (char *)buf; result = strlen(buf); }
        
    /*****定义 return token 的各种宏定义****/
    #define RETURN_TOKEN(x) return(x)
    /* RETURN_VAL 通用的返回 VAL 的 return 方法,先获取坐标值,再返回 token 字符串*/
    #define RETURN_VAL(x)  {GetCoord(&yylval.tok,yytext);RETURN_TOKEN(x);}
    #define PA_KEYWORD_RETURN(x) RETURN_VAL(x)
    #define PPPA_KEYWORD_RETURN(x) RETURN_VAL(x) /* PreProcessor and C keyword 目测没用*/
    /*PP_KEYWORD_RETURN 较常用 */
    #define PP_KEYWORD_RETURN(x) \
    { GetCoord(&yylval.tok,yytext);\
      yylval.n = MakeIdCoord(UniqueString(yytext), yylval.tok);\
      RETURN_TOKEN(IDENTIFIER); }
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

simple_escape ([abfnrtv'"?\\])
octal_escape  ([0-7][^'\n]*)
hex_escape ("x"[^'\n]*)
bad_escape ([\\][^abfnrtv'"?\\x0-7\n ])

escape_sequence ([\\]({simple_escape}|{octal_escape}|{hex_escape}))
c_char ([^'\\\n]|{escape_sequence})
s_char ([^"\\\n]|{escape_sequence})

/*第一个是空格 \011 tab \013回车 \014 */
any_white ([ \011\013\014\015])

Annotations      \/\*(\/\/|[^(\*\/)]|\n)*\*\/
Annotation       \/\/[^\n]*\n
%option yylineno
%%
<INITIAL,C>{Annotation}           {debug("单行注释\n");}
<INITIAL,C>{Annotations}           {debug("多行注释\n");}
<INITIAL,C>"\n"             {  Line = yylineno;}

<INITIAL,C>break               {PA_KEYWORD_RETURN(BREAK);}
<INITIAL,C>case                {PA_KEYWORD_RETURN(CASE);}
<INITIAL,C>char                {PA_KEYWORD_RETURN(CHAR);}
<INITIAL,C>const               {PA_KEYWORD_RETURN(CONST);}
<INITIAL,C>continue            {PA_KEYWORD_RETURN(CONTINUE);}
<INITIAL,C>default             {PA_KEYWORD_RETURN(DEFAULT);}
<INITIAL,C>do                  {PA_KEYWORD_RETURN(DO);}
<INITIAL,C>double              {PA_KEYWORD_RETURN(DOUBLE);}
<INITIAL,C>int                 {PA_KEYWORD_RETURN(INT);}
<INITIAL,C>else                {PPPA_KEYWORD_RETURN(ELSE);}
<INITIAL,C>for                 {PA_KEYWORD_RETURN(FOR);}
<INITIAL,C>if                  {PPPA_KEYWORD_RETURN(IF);}
<INITIAL,C>while               {PA_KEYWORD_RETURN(WHILE);}
<INITIAL,C>static               {PA_KEYWORD_RETURN(STATIC);}

<INITIAL,C>composite		       {PA_KEYWORD_RETURN(COMPOSITE);}
<INITIAL,C>input			         {PA_KEYWORD_RETURN(INPUT);}
<INITIAL,C>output			         {PA_KEYWORD_RETURN(OUTPUT);}
<INITIAL,C>stream              {PA_KEYWORD_RETURN(STREAM);}
<INITIAL,C>FileReader		    {PA_KEYWORD_RETURN(FILEREADER);}
<INITIAL,C>FileWriter		    {PA_KEYWORD_RETURN(FILEWRITER);}
<INITIAL,C>add				      {PA_KEYWORD_RETURN(ADD);}

<INITIAL,C>param			      {PA_KEYWORD_RETURN(PARAM);}

<INITIAL,C>init			        {PA_KEYWORD_RETURN(INIT);}
<INITIAL,C>work			        {PA_KEYWORD_RETURN(WORK);}
<INITIAL,C>window			      {PA_KEYWORD_RETURN(WINDOW);}

<INITIAL,C>tumbling			    {PA_KEYWORD_RETURN(TUMBLING);}
<INITIAL,C>sliding			    {PA_KEYWORD_RETURN(SLIDING);}


<INITIAL,C>splitjoin		    {PA_KEYWORD_RETURN(SPLITJOIN);}
<INITIAL,C>pipeline			    {PA_KEYWORD_RETURN(PIPELINE);}
<INITIAL,C>split			      {PA_KEYWORD_RETURN(SPLIT);}
<INITIAL,C>join				      {PA_KEYWORD_RETURN(JOIN);}
<INITIAL,C>duplicate		    {PA_KEYWORD_RETURN(DUPLICATE);}
<INITIAL,C>roundrobin		    {PA_KEYWORD_RETURN(ROUNDROBIN);}

<INITIAL,C>"("                  {return(*yytext);}
<INITIAL,C>")"                  {return(*yytext);}
<INITIAL,C>","                  {return(*yytext);}
<INITIAL,C>"#"                  {return ('#') ;} 

<INITIAL,C>"{"                  {return(*yytext);}
<INITIAL,C>"}"                  {return(*yytext);}

<INITIAL,C>"["                  {return(*yytext);}
<INITIAL,C>"]"                  {return(*yytext);}
<INITIAL,C>"."                  {return(*yytext);}
<INITIAL,C>"&"                  {return(*yytext);}
<INITIAL,C>"*"                  {return(*yytext);}
<INITIAL,C>"+"                  {return(*yytext);}
<INITIAL,C>"-"                  {return(*yytext);}
<INITIAL,C>"~"                  {return(*yytext);}
<INITIAL,C>"!"                  {return(*yytext);}
<INITIAL,C>"/"                  {return(*yytext);}
<INITIAL,C>"%"                  {return(*yytext);}
<INITIAL,C>"<"                  {return(*yytext);}
<INITIAL,C>">"                  {return(*yytext);}
<INITIAL,C>"^"                  {return(*yytext);}
<INITIAL,C>"|"                  {return(*yytext);}
<INITIAL,C>"?"                  {return(*yytext);}
<INITIAL,C>":"                  {return(*yytext);}
<INITIAL,C>";"                  {return(*yytext);}
<INITIAL,C>"="                  {return(*yytext);}

<INITIAL,C>"->"                 {RETURN_VAL(ARROW);}
<INITIAL,C>"++"                 {RETURN_VAL(ICR);}
<INITIAL,C>"--"                 {RETURN_VAL(DECR);}
<INITIAL,C>"<<"                 {RETURN_VAL(LS);}
<INITIAL,C>">>"                 {RETURN_VAL(RS);}//发生冲突了暂时去掉 SPL?WHY?lxx
<INITIAL,C>"<="                 {RETURN_VAL(LE);}
<INITIAL,C>">="                 {RETURN_VAL(GE);}
<INITIAL,C>"=="                 {RETURN_VAL(EQ);}
<INITIAL,C>"!="                 {RETURN_VAL(NE);}
<INITIAL,C>"&&"                 {RETURN_VAL(ANDAND);}
<INITIAL,C>"||"                 {RETURN_VAL(OROR);}
<INITIAL,C>"*="                 {RETURN_VAL(MULTassign);}
<INITIAL,C>"/="                 {RETURN_VAL(DIVassign);}
<INITIAL,C>"%="                 {RETURN_VAL(MODassign);}
<INITIAL,C>"+="                 {RETURN_VAL(PLUSassign);}
<INITIAL,C>"-="                 {RETURN_VAL(MINUSassign);}
<INITIAL,C>"<<="                {RETURN_VAL(LSassign);}
<INITIAL,C>">>="                {RETURN_VAL(RSassign);}
<INITIAL,C>"&="                 {RETURN_VAL(ANDassign);}
<INITIAL,C>"^="                 {RETURN_VAL(ERassign);}
<INITIAL,C>"|="                 {RETURN_VAL(ORassign);}
<INITIAL,C>"..."                {RETURN_VAL(ELLIPSIS);}

<INITIAL,C>{decimal_constant}    {  GetCoord(&yylval.tok,yytext);  
                                    yylval.n = int_constant(yytext,yylval.tok, TRUE, FALSE);
                                    RETURN_TOKEN(INTEGERconstant);}
<INITIAL,C>{octal_constant}      {  GetCoord(&yylval.tok,yytext);
                                    yylval.n = int_constant(yytext,yylval.tok, TRUE, FALSE);
                                    RETURN_TOKEN(OCTALconstant);}
<INITIAL,C>{hex_constant}        {  GetCoord(&yylval.tok,yytext);
                                    yylval.n = int_constant(yytext,yylval.tok, TRUE, FALSE);
                                    RETURN_TOKEN(HEXconstant);}
<INITIAL,C>{floating_constant}[^.]   {  yyless(yyleng-1);  /* 这一句写了等于没写,yytext 没变化*/
                                        GetCoord(&yylval.tok,yytext);
                                        yylval.n = float_constant(yytext,yylval.tok);
                                        RETURN_TOKEN(FLOATINGconstant);}
                                                                            
<INITIAL,C>{any_white}+          {}


<INITIAL,C>{identifier}         { debug("identifier\n");
                                  GetCoord(&yylval.tok,yytext);  
                                  yylval.n = MakeIdCoord(UniqueString(yytext),yylval.tok);
                         	        RETURN_TOKEN(IDENTIFIER); }
<INITIAL,C>.                    {  char tmp[20];
                                    CharToText(tmp, *yytext); //这一行暂且不用了
                                    SyntaxError("Unexpected character: %s \n", yytext); }    

%%

/*****第1步,获取文件名****/
GLOBAL char *FileNames[MAX_FILES] = {(char*)"REPL"}; //默认的命令行动态输入输出就是 REPL
GLOBAL char *Filename = FileNames[0];
GLOBAL Bool CurrentIncludedp = FALSE;
GLOBAL unsigned int CurrentFile = 0;
GLOBAL void SetFile(const char *name, int line){
  return ;
} 


//<<flex 与 bison>>第144页:  当词法分析器到达文件的末尾时,它可以选择性地调用yywrap()来了解下一步操作.可以让 yywrap 返回1或者%option noyywrap 来终止词法分析操作.
int yywrap()
{
   return 1;
}