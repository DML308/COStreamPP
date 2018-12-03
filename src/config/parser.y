%{
#define DEBUG
#include "defines.h"
#include "node.h"
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
%type<node> prog.start translation.unit external.definition
/* 语法分析器自己的结构   1.1.declaration */
%type<node> declaration declaring.list
%type<node> stream.declaring.list stream.type.specifier stream.declaration.list
/* 语法分析器自己的结构     1.1.3.array */
%type<node> array.declarator
/* 语法分析器自己的结构     1.1.4.initializer */
%type<node> initializer.opt initializer initializer.list
/* 语法分析器自己的结构   1.2 function.definition 函数声明 */
%type<node> function.definition function.body   parameter.list  parameter.declaration statement.list
/* 语法分析器自己的结构   1.3 composite.definition 数据流计算单元声明 */
%type<node> composite.definition  composite.head  composite.head.inout
%type<node> composite.head.inout.member.list  composite.head.inout.member
%type<node> operator.pipeline
/* 语法分析器自己的结构      1.3.2 composite.body */
%type<node> composite.body  composite.body.param.opt
%type<node> composite.body.statement.list costream.composite.statement
/* 语法分析器自己的结构 2. composite.body.operator  composite体内的init work window等组件  */
%type<node> composite.body.operator   operator.file.writer  operator.add
%type<node> splitjoinPipeline.statement.list  operator.splitjoin  split.statement
%type<node> roundrobin.statement   duplicate.statement  join.statement  argument.expression.list  operator.default.call
/* 语法分析器自己的结构 3.statement 花括号内以';'结尾的结构是statement  */
%type<node> statement labeled.statement compound.statement
%type<node> expression.statement  selection.statement   iteration.statement jump.statement
/* 语法分析器自己的结构 4.exp 计算表达式头节点  */
%type<node> assignment.operator exp constant.expression
%type<node> operator.selfdefine.body  operator.selfdefine.body.init operator.selfdefine.body.work
%type<node> operator.selfdefine.body.window.list  operator.selfdefine.window.list operator.selfdefine.window
%type<node> window.type
/* 语法分析器自己的结构 5.basic 从词法TOKEN直接归约得到的节点 */
%type<node>constant type.specifier basic.type.name
%type<num> integerConstant
%type<doubleNum> doubleConstant
%type<str> IDENTIFIER stringConstant



/* C. 优先级标记,从上至下优先级从低到高排列 */
%left '='
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
%left ')' ']'
%right '(' '['

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
prog.start: translation.unit { $$ = NULL ; };

translation.unit:
          external.definition   {
                                      line("Line:%-3d",@1.first_line);
                                      debug ("translation.unit ::= external.definition\n");
                                      $$ = NULL ;
                                }
        | translation.unit external.definition  {
                                                      line("Line:%-3d",@1.first_line);
                                                      debug ("translation.unit ::= translation.unit external.definition\n");
                                                      $$ = NULL ;
                                                }
        ;
external.definition:
          declaration           {
                                      line("Line:%-3d",@1.first_line);
                                      debug ("external.definition ::= declaration\n");
                                      $$ = NULL ;
                                }
        | function.definition   {
                                      line("Line:%-3d",@1.first_line);
                                      debug ("external.definition ::= function.definition\n");
                                      $$ = NULL ;
                                }
        | composite.definition  {
                                      line("Line:%-3d",@1.first_line);
                                      debug ("external.definition ::= composite.definition\n");
                                      $$ = NULL ;
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
                                      $$ = NULL ;
                                }
    | stream.declaring.list ';' {
                                      line("Line:%-3d",@1.first_line);
                                      debug ("declaration ::= stream.declaring.list ';' \n");
                                      $$ = NULL ;

                                }
    ;
declaring.list:
          type.specifier 			  IDENTIFIER initializer.opt  {
              line("Line:%-3d",@1.first_line);
              debug ("declaring.list ::= type.specifier IDENTIFIER initializer.opt \n");
              $$ = NULL ;
        }
        | type.specifier 			  IDENTIFIER array.declarator initializer.opt{
              line("Line:%-3d",@1.first_line);
              debug ("declaring.list ::= type.specifier IDENTIFIER array.declarator initializer.opt \n");
              $$ = NULL ;
        }
        | declaring.list 	',' 	IDENTIFIER initializer.opt{
              line("Line:%-3d",@1.first_line);
              debug ("declaring.list ::= declaring.list 	',' 	IDENTIFIER initializer.opt \n");
              $$ = NULL ;
        }
        | declaring.list 	',' 	IDENTIFIER array.declarator initializer.opt{
              line("Line:%-3d",@1.first_line);
              debug ("declaring.list ::= declaring.list 	',' 	IDENTIFIER array.declarator initializer.opt \n");
              $$ = NULL ;
        }
        ;
stream.declaring.list:
          stream.type.specifier IDENTIFIER    {
                                                  line("Line:%-3d",@1.first_line);
                                                  debug ("stream.declaring.list ::= stream.type.specifier IDENTIFIER \n");
                                                  $$ = NULL ;
                                              }
        | stream.declaring.list ',' IDENTIFIER{
                                                  line("Line:%-3d",@1.first_line);
                                                  debug ("stream.declaring.list ::= stream.declaring.list ',' IDENTIFIER \n");
                                                  $$ = NULL ;
                                              }
        ;
stream.type.specifier:
          STREAM '<' stream.declaration.list '>'{
                                                    line("Line:%-3d",@1.first_line);
                                                    debug ("stream.type.specifier ::=  STREAM '<' stream.declaration.list '>' \n");
                                                    $$ = NULL ;
                                                }
        ;
stream.declaration.list:
          type.specifier IDENTIFIER {
                                        line("Line:%-3d",@1.first_line);
                                        debug ("stream.declaration.list ::=  type.specifier IDENTIFIER \n");
                                        $$ = NULL ;
                                    }
        | type.specifier IDENTIFIER array.declarator{
                                        line("Line:%-3d",@1.first_line);
                                        debug ("stream.declaration.list ::=  type.specifier IDENTIFIER array.declarator \n");
                                        $$ = NULL ;
                                    }
        | stream.declaration.list ',' type.specifier IDENTIFIER {
                                        line("Line:%-3d",@1.first_line);
                                        debug ("stream.declaration.list ::=  stream.declaration.list ',' type.specifier IDENTIFIER \n");
                                        $$ = NULL ;
                                    }
        | stream.declaration.list ',' type.specifier IDENTIFIER array.declarator{
                                        line("Line:%-3d",@1.first_line);
                                        debug ("stream.declaration.list ::=  stream.declaration.list ',' type.specifier IDENTIFIER array.declarator \n");
                                        $$ = NULL ;
                                    }
        ;

/*************************************************************************/
/*                      1.1.3 array ( int a[] )                          */
/*************************************************************************/
array.declarator:
          '[' ']'   {
                            line("Line:%-3d",@1.first_line);
                            debug ("array.declarator ::= '[' ']' \n");
                            $$ = NULL ;
                    }
        | '[' constant.expression ']' {
                            line("Line:%-3d",@1.first_line);
                            debug ("array.declarator ::= '[' constant.expression ']' \n");
                            $$ = NULL ;
                    }
        | array.declarator '[' constant.expression ']'  {
                            line("Line:%-3d",@1.first_line);
                            debug ("array.declarator ::= array.declarator '[' constant.expression ']' \n");
                            $$ = NULL ;
                    }
        | array.declarator '[' ']'  {
                            error ("Line:%-3d array declaration with illegal empty dimension\n",@1.first_line);
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
                                    $$ = NULL ;
                                }
        | '=' initializer       {
                                    line("Line:%-3d",@1.first_line);
                                    debug ("initializer.opt ::= '=' initializer \n");
                                    $$ = NULL ;
                                }
        ;
initializer:
          '{' initializer.list '}'      {
                                            line("Line:%-3d",@1.first_line);
                                            debug ("initializer ::= '{' initializer.list '}' \n");
                                            $$ = NULL ;
                                            //$$ = NULL ;
                                        }
        | '{' initializer.list ',' '}'  {
                                            /* 本条规约规则有用吗？有用!现在js支持列表最后多个逗号 */
                                            line("Line:%-3d",@1.first_line);
                                            debug ("initializer ::= '{' initializer.list ',' '}' \n");
                                            $$ = NULL ;
                                            //$$ = NULL ;
                                        }
        | exp                           {

                                            line("Line:%-3d",@1.first_line);
                                            debug ("initializer ::= exp \n");
                                            $$ = NULL ;
                                        }
        ;

initializer.list:
          initializer   {
                            line("Line:%-3d",@1.first_line);
                            debug ("initializer.list ::= initializer \n");
                            $$ = NULL ;
                        }
        | initializer.list ',' initializer  {
                            line("Line:%-3d",@1.first_line);
                            debug ("initializer.list ::= initializer.list ',' initializer \n");
                            $$ = NULL ;
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
                $$ = NULL ;
        }
        | type.specifier IDENTIFIER '(' parameter.list ')' function.body  {
                line("Line:%-3d",@1.first_line);
                debug ("function.definition ::= type.specifier IDENTIFIER '(' parameter.list ')' function.body \n");
                $$ = NULL ;
        }
        ;

parameter.list:
          parameter.declaration   {
                line("Line:%-3d",@1.first_line);
                debug ("parameter.list ::= parameter.declaration \n");
                $$ = NULL ;
          }
        | parameter.list ',' parameter.declaration {
                line("Line:%-3d",@1.first_line);
                debug ("parameter.list ::= parameter.list ',' parameter.declaration \n");
                $$ = NULL ;
          }
        | parameter.declaration '=' initializer {
                //函数参数里不支持初始化
                error( "Line:%-3d parameter.list in function definations cannot have initializers\n",@1.first_line);
                exit(121);
          }
        | parameter.list ',' error
        ;
parameter.declaration:
          type.specifier IDENTIFIER {
                                          line("Line:%-3d",@1.first_line);
                                          debug ("parameter.declaration ::= type.specifier IDENTIFIER \n");
                                          $$ = NULL ;
                                    }
        | type.specifier IDENTIFIER array.declarator  {
                                          line("Line:%-3d",@1.first_line);
                                          debug ("parameter.declaration ::= type.specifier IDENTIFIER array.declarator \n");
                                          $$ = NULL ;
                                    }
        ;
function.body:
          '{' '}'                   {
                                          line("Line:%-3d",@1.first_line);
                                          debug ("function.body ::= '{' '}' \n");
                                          $$ = NULL ;
                                    }
        | '{' statement.list '}'    {
                                          line("Line:%-3d",@1.first_line);
                                          debug ("function.body ::= '{' statement.list '}' \n");
                                          $$ = NULL ;
                                    }
        ;

statement.list:
          statement                   { $$ = NULL ;}
        | statement.list statement    { $$ =NULL; }
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
                                          $$ = NULL ;
                                    }
    ;
composite.head:
      COMPOSITE IDENTIFIER '(' composite.head.inout ')'   {
                                                                line("Line:%-3d",@1.first_line);
                                                                debug ("composite.head ::= COMPOSITE IDENTIFIER '(' composite.head.inout ')' \n");
                                                                $$ = NULL ;
                                                          }
    ;
composite.head.inout:
      /*empty*/                                                                           { $$ = NULL ; }
    | INPUT composite.head.inout.member.list                                              { $$ = NULL ; }
    | INPUT composite.head.inout.member.list ',' OUTPUT composite.head.inout.member.list  { $$ = NULL ; }
    | OUTPUT composite.head.inout.member.list                                             { $$ = NULL ; }
    | OUTPUT composite.head.inout.member.list ',' INPUT composite.head.inout.member.list  { $$ = NULL ; }
    ;
composite.head.inout.member.list:
      composite.head.inout.member                                                         { $$ = NULL ; }
    | composite.head.inout.member.list ',' composite.head.inout.member                    { $$ = NULL ; }
    ;
composite.head.inout.member:
      stream.type.specifier IDENTIFIER                                                    { $$ = NULL ; }
    ;
/*************************************************************************/
/*                      1.3.2 composite.body                             */
/*                        1.3.2.1 composite.body.param.opt               */
/*                        1.3.2.2 composite.body.declaration.list        */
/*                        1.3.2.3 composite.body.statement.list          */
/*************************************************************************/
composite.body:
          '{' composite.body.param.opt composite.body.statement.list '}'                              { $$ = NULL ; }
        ;
composite.body.param.opt:
          /*empty*/                 { $$ = NULL ; }
        | PARAM parameter.list ';'  {
                                          line("Line:%-3d",@1.first_line);
                                          debug ("composite.body.param.opt ::= PARAM parameter.list \n");
                                          $$ = NULL ;
                                    }
        ;
composite.body.statement.list:
          costream.composite.statement                                { $$ = NULL ; }
        | composite.body.statement.list costream.composite.statement  { $$ = NULL ; }
        ;
costream.composite.statement:
          composite.body.operator   {
                                          line("Line:%-3d",@1.first_line);
                                          debug ("costream.composite.statement ::= composite.body.operator \n");
                                          $$ = NULL ;
                                    }
        | statement                 {
                                          line("Line:%-3d",@1.first_line);
                                          debug ("costream.composite.statement ::= statement \n");
                                          $$ = NULL ;
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
                                          $$ = NULL ;
                                    }
        | operator.add              {
                                          line("Line:%-3d",@1.first_line);
                                          debug ("composite.body.operator ::= operator.add \n");
                                          $$ = NULL ;
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
                                          $$ = NULL ;
                                    }
        | ADD operator.splitjoin    {
                                          line("Line:%-3d",@1.first_line);
                                          debug ("operator.add ::= ADD operator.splitjoin \n");
                                          $$ = NULL ;
                                    }
        | ADD operator.default.call {
                                          line("Line:%-3d",@1.first_line);
                                          debug ("operator.add ::= ADD operator.default.call \n");
                                          $$ = NULL ;
                                    }
        ;
operator.pipeline:
          PIPELINE '{'  splitjoinPipeline.statement.list '}'                  { $$ = NULL ; }
        ;
splitjoinPipeline.statement.list:
          statement                                       { $$ = NULL ; }
        | operator.add                                    { $$ = NULL ; }
        | splitjoinPipeline.statement.list statement      { $$ = NULL ; }
        | splitjoinPipeline.statement.list operator.add   { $$ = NULL ; }
        ;
operator.splitjoin:
          SPLITJOIN '{' split.statement  splitjoinPipeline.statement.list  join.statement '}'     {
                                                                                                    $$ = NULL ;
                                                                                                  }
        | SPLITJOIN '{' statement.list split.statement splitjoinPipeline.statement.list join.statement '}'  {
                                                                                                    $$ = NULL ;
                                                                                                  }
        ;
split.statement:
          SPLIT duplicate.statement                        { $$ = NULL ; }
        | SPLIT roundrobin.statement                       { $$ = NULL ; }
        ;
roundrobin.statement:
          ROUNDROBIN '(' ')' ';'                            { $$ = NULL ; }
        | ROUNDROBIN '(' argument.expression.list ')' ';'   { $$ = NULL ; }
        ;
duplicate.statement:
          DUPLICATE '('  ')' ';'                            { $$ = NULL ; }
        | DUPLICATE '(' exp ')'  ';'                        { $$ = NULL ; }
        ;
join.statement:
          JOIN roundrobin.statement                         { $$ = NULL ; }
        ;
argument.expression.list:
          exp                                               { $$ = NULL ; }
        | argument.expression.list ',' exp                  { $$ = NULL ; }
        ;
operator.default.call:
          IDENTIFIER  '(' ')' ';'                           { $$ = NULL ; }
        | IDENTIFIER  '(' argument.expression.list ')' ';'  {
                                                              /*composite call(StreamIt style)*///operator.param.list 不能为空以区分函数调用/*composite call*/
                                                              ///*DEBUG*/printf("have found operator.default.call\n");
                                                              $$ = NULL ;
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
        | error ';'{  $$ = NULL ; }
        ;

labeled.statement:
          CASE constant.expression ':' statement    {
                                                          line("Line:%-3d",@1.first_line);
                                                          debug ("labeled.statement ::= CASE constant.expression ':' statement \n");
                                                          $$ = NULL ;
                                                    }
        | DEFAULT ':' statement                     {
                                                          line("Line:%-3d",@1.first_line);
                                                          debug ("labeled.statement ::= DEFAULT ':' statement \n");
                                                          $$ = NULL ;
                                                    }
        ;
compound.statement:
          '{' '}'                                               {  $$ = NULL ; }
        | '{' composite.body.statement.list '}'                 {  $$ = NULL ; }
        ;

expression.statement:
          exp ';'  {  $$ = NULL ; }
        ;

selection.statement:
          IF '(' exp ')' costream.composite.statement    {  $$ = NULL ; }
        | IF '(' exp ')' costream.composite.statement ELSE costream.composite.statement {  /* 可以为普通表达式也可以为流声明 */ $$ = NULL ; }
        | SWITCH '(' exp ')' statement                   {  $$ = NULL ; }
        ;
iteration.statement:
          WHILE '(' exp ')' costream.composite.statement                                           {  $$ = NULL ; }
        | DO  costream.composite.statement WHILE '(' exp ')' ';'                                   {  $$ = NULL ; }
        | FOR '(' exp   ';' exp ';' exp ')'  costream.composite.statement                           {  $$ = NULL ; }
        | FOR '(' declaration  ';' exp ';' exp ')'  costream.composite.statement                    {  $$ = NULL ; }
        | FOR '(' error ')' costream.composite.statement                                                  {  $$ = NULL ; }
        ;
jump.statement:
          CONTINUE ';'               {  $$ = NULL ; }
        | BREAK ';'                 {  $$ = NULL ; }
        | RETURN exp ';' {  $$ = NULL ; }
        ;

/*************************************************************************/
/*        4. exp 计算表达式头节点                        */
/*************************************************************************/
assignment.operator:
          '='             { $$ = NULL ; }
        | MULTassign      { $$ = NULL ; }
        | DIVassign       { $$ = NULL ; }
        | MODassign       { $$ = NULL ; }
        | PLUSassign      { $$ = NULL ; }
        | MINUSassign     { $$ = NULL ; }
        | LSassign        { $$ = NULL ; }
        | RSassign        { $$ = NULL ; }
        | ANDassign       { $$ = NULL ; }
        | ERassign        { $$ = NULL ; }
        | ORassign        { $$ = NULL ; }
        ;
exp:      IDENTIFIER                        { line("Line:%-3d",@1.first_line);debug ("exp ::= IDENTIFIER\n"); $$ = NULL ; }
        | IDENTIFIER  array.declarator      { line("Line:%-3d",@1.first_line);debug ("exp ::= IDENTIFIER\n"); $$ = NULL ; }
        | constant        { line("Line:%-3d",@1.first_line);debug ("exp ::= constant\n"); $$ = NULL ; }
        | exp '+' exp     { line("Line:%-3d",@1.first_line);debug ("exp ::= exp + exp\n"); $$ = NULL ; }
        | exp '-' exp     { line("Line:%-3d",@1.first_line);debug ("exp ::= exp - exp\n"); $$ = NULL ; }
        | exp '*' exp     { line("Line:%-3d",@1.first_line);debug ("exp ::= exp * exp\n"); $$ = NULL ; }
        | exp '/' exp     { line("Line:%-3d",@1.first_line);debug ("exp ::= exp / exp\n"); $$ = NULL ; }
        | exp '%' exp     { line("Line:%-3d",@1.first_line);debug ("exp ::= exp %% exp\n"); $$ = NULL ; }
        | exp OROR exp    { line("Line:%-3d",@1.first_line);debug ("exp ::= exp || exp\n"); $$ = NULL ; }
        | exp ANDAND exp  { line("Line:%-3d",@1.first_line);debug ("exp ::= exp && exp\n"); $$ = NULL ; }
        | exp '|' exp     { line("Line:%-3d",@1.first_line);debug ("exp ::= exp | exp\n"); $$ = NULL ; }
        | exp '&' exp     { line("Line:%-3d",@1.first_line);debug ("exp ::= exp & exp\n"); $$ = NULL ; }
        | exp '^' exp     { line("Line:%-3d",@1.first_line);debug ("exp ::= exp ^ exp\n"); $$ = NULL ; }
        | exp LS exp      { line("Line:%-3d",@1.first_line);debug ("exp ::= exp << exp\n"); $$ = NULL ; }
        | exp RS exp      { line("Line:%-3d",@1.first_line);debug ("exp ::= exp >> exp\n"); $$ = NULL ; }
        | exp '<' exp     { line("Line:%-3d",@1.first_line);debug ("exp ::= exp < exp\n"); $$ = NULL ; }
        | exp '>' exp     { line("Line:%-3d",@1.first_line);debug ("exp ::= exp > exp\n"); $$ = NULL ; }
        | exp LE exp      { line("Line:%-3d",@1.first_line);debug ("exp ::= exp <= exp\n"); $$ = NULL ; }
        | exp GE exp      { line("Line:%-3d",@1.first_line);debug ("exp ::= exp >= exp\n"); $$ = NULL ; }
        | exp EQ exp      { line("Line:%-3d",@1.first_line);debug ("exp ::= exp == exp\n"); $$ = NULL ; }
        | exp NE exp      { line("Line:%-3d",@1.first_line);debug ("exp ::= exp != exp\n"); $$ = NULL ; }
        | exp '?' exp ':' exp { line("Line:%-3d",@1.first_line);debug ("exp ::= exp ? exp : exp\n"); $$ = NULL ; }
        | '+' exp         { line("Line:%-3d",@1.first_line);debug ("exp ::= + exp\n"); $$ = NULL ; }
        | '-' exp         { line("Line:%-3d",@1.first_line);debug ("exp ::= - exp\n"); $$ = NULL ; }
        | '~' exp         { line("Line:%-3d",@1.first_line);debug ("exp ::= ~ exp\n"); $$ = NULL ; }
        | '!' exp         { line("Line:%-3d",@1.first_line);debug ("exp ::= ! exp\n"); $$ = NULL ; }
        |  ICR exp        { line("Line:%-3d",@1.first_line);debug ("exp ::= ++ exp\n"); $$ = NULL ; }
        |  DECR exp       { line("Line:%-3d",@1.first_line);debug ("exp ::= -- exp\n"); $$ = NULL ; }
        |  exp ICR        { line("Line:%-3d",@1.first_line);debug ("exp ::= exp ++ \n"); $$ = NULL ; }
        |  exp DECR       { line("Line:%-3d",@1.first_line);debug ("exp ::= exp --\n"); $$ = NULL ; }
        |  '(' exp ')'    { line("Line:%-3d",@1.first_line);debug ("exp ::= ( exp )\n"); $$ = NULL ; }
        | '(' basic.type.name ')' exp                         { line("Line:%-3d",@1.first_line);debug ("exp ::= ( type ) exp\n"); $$ = NULL ; }
        | IDENTIFIER assignment.operator exp                  { line("Line:%-3d",@1.first_line);debug ("exp ::= IDENTIFIER assignment.operator exp\n"); $$ = NULL ; }
        | IDENTIFIER array.declarator assignment.operator exp { line("Line:%-3d",@1.first_line);debug ("exp ::= IDENTIFIER array.declarator assignment.operator exp\n"); $$ = NULL ; }
        | IDENTIFIER '(' argument.expression.list ')'         { line("Line:%-3d",@1.first_line);debug ("exp ::= IDENTIFIER ( exp.list )\n"); $$ = NULL ; }
        | FILEREADER '(' ')' '(' stringConstant ')'           { line("Line:%-3d",@1.first_line);debug ("exp ::= FILEREADER()( stringConstant )\n"); $$ = NULL ; }
        | IDENTIFIER '('  ')' operator.selfdefine.body        { line("Line:%-3d",@1.first_line);debug ("exp ::= IDENTIFIER() operator.selfdefine.body\n"); $$ = NULL ; }
        | IDENTIFIER '(' IDENTIFIER ')' operator.selfdefine.body   { $$ = NULL ; }
        | IDENTIFIER '('  ')'  '(' ')'{
                                                                        /* 调用composite的四种情况*/
                                      }
        | IDENTIFIER '('  ')'  '(' argument.expression.list ')'              { $$ = NULL ; }
        | IDENTIFIER '(' IDENTIFIER ')'  '(' ')'                             { $$ = NULL ; }
        | IDENTIFIER '(' IDENTIFIER ')'  '(' argument.expression.list ')'    { $$ = NULL ; }
        | SPLITJOIN '(' IDENTIFIER ')'  '{'   split.statement  splitjoinPipeline.statement.list  join.statement '}'   { $$ = NULL ; }
        | SPLITJOIN '(' IDENTIFIER ')'  '{'   statement.list split.statement splitjoinPipeline.statement.list  join.statement '}'  { $$ = NULL ; }
        |  PIPELINE '(' IDENTIFIER ')'  '{'   splitjoinPipeline.statement.list '}'                                     { $$ = NULL ; }
        ;
constant.expression:
          exp             {
                            //常量表达式放到语义检查中做
                          }
        ;


operator.selfdefine.body:
       '{' operator.selfdefine.body.init operator.selfdefine.body.work operator.selfdefine.body.window.list '}'
        {
            line("Line:%-3d",@1.first_line);
            debug ("operator.selfdefine.body ::=  { init work window.list }\n");
            $$ = NULL ;
        }
     | '{' statement.list operator.selfdefine.body.init  operator.selfdefine.body.work operator.selfdefine.body.window.list '}'
        {
            line("Line:%-3d",@1.first_line);
            debug ("operator.selfdefine.body ::=  { statement.list init work window.list }\n");
            $$ = NULL ;
        }
     ;

operator.selfdefine.body.init:
      /*empty*/{ $$ = NULL; }
    | INIT compound.statement { $$ = NULL ; }
    ;

operator.selfdefine.body.work:
      WORK compound.statement { $$ = NULL ; }
    ;

operator.selfdefine.body.window.list:
      /*empty*/                                         { $$ = NULL; }
      | WINDOW '{' operator.selfdefine.window.list '}'  {
                                                            line("Line:%-3d",@1.first_line);
                                                            debug ("operator.selfdefine.body.window.list ::= WINDOW { operator.selfdefine.window.list }\n");
                                                            $$ = NULL ;
                                                        }
    ;

operator.selfdefine.window.list:
      operator.selfdefine.window
    | operator.selfdefine.window.list operator.selfdefine.window
    ;

operator.selfdefine.window:
      IDENTIFIER window.type ';'                {
                                                    line("Line:%-3d",@1.first_line);
                                                    debug ("operator.selfdefine.window ::= IDENTIFIER window.type (sliding? (arg_list?))\n");
                                                    $$ = NULL ;
                                                }
    ;

window.type:
      SLIDING '('  ')'                          {
                                                    $$ = NULL ;
                                                }
    | TUMBLING '('  ')'                         {
                                                    $$ = NULL ;
                                                }
    | SLIDING '(' argument.expression.list ')'  {
                                                    $$ = NULL ;
                                                }
    | TUMBLING '(' argument.expression.list ')' {
                                                    $$ = NULL ;
                                                }
    ;


/*************************************************************************/
/*        5. basic 从词法TOKEN直接归约得到的节点,自底向上接入头部文法结构    */
/*************************************************************************/
constant:
          doubleConstant    {
                                line("Line:%-3d",@1.first_line);
                                debug ("constant ::= doubleConstant | value:=%lf\n",$1);
                                $$ = NULL ;
                            }
        | integerConstant   {
                                line("Line:%-3d",@1.first_line);
                                debug ("constant ::= integerConstant | value:=%d\n",$1);
                                $$ = NULL ;
                            }
        | stringConstant    {
                                line("Line:%-3d",@1.first_line);
                                debug ("constant ::= stringConstant | value:=%s\n",$1->c_str());
                                $$ = NULL ;
                            }
        ;
type.specifier:
          basic.type.name       {
                                    line("Line:%-3d",@1.first_line);
                                    debug ("type.specifier ::=  basic.type.name \n");
                                    $$ = NULL ;
                                }
        | CONST basic.type.name {
                                    line("Line:%-3d",@1.first_line);
                                    debug ("type.specifier ::=  CONST basic.type.name \n");
                                    $$ = NULL ;
                                }
        ;
basic.type.name:
          INT   {
                      line("Line:%-3d",@1.first_line);
                      debug ("basic.type.name ::=  INT \n");
                      $$ = NULL ;
                }
        | LONG  {
                      line("Line:%-3d",@1.first_line);
                      debug ("basic.type.name ::=  LONG \n");
                      $$ = NULL ;
                }
        | LONG LONG {
                      line("Line:%-3d",@1.first_line);
                      debug ("basic.type.name ::=  LONG LONG  \n");
                      $$ = NULL ;
                    }
        | FLOAT {
                      line("Line:%-3d",@1.first_line);
                      debug ("basic.type.name ::=  FLOAT \n");
                      $$ = NULL ;
                }
        | DOUBLE{
                      line("Line:%-3d",@1.first_line);
                      debug ("basic.type.name ::=  DOUBLE \n");
                      $$ = NULL ;
                }
        | STRING{
                      line("Line:%-3d",@1.first_line);
                      debug ("basic.type.name ::=  STRING \n");
                      $$ = NULL ;
                }
        ;
%%
/* ----语法树结束----*/
void yyerror (const char *msg)
{
    error ("%s", msg);
}
