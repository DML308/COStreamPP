%{
#define DEBUG
#include "defines.h"
#include "node.h"
#include "symbol.h"
#include <list>
//using std::list;
extern int yylex ();
extern void yyerror (const char *msg);

%}

/*在 union 里声明 %token 可能有的类型*/
%union{
    long long  num;
    double doubleNum;
    std::string *str;
    Node * node;
    std::list<Node*> *list;
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
%type<list> prog.start translation.unit 
%type<node> external.definition
/* 语法分析器自己的结构   1.1.declaration */

%type<node> declaration declaring.list
%type<node> stream.type.specifier
%type<list> stream.declaration.list stream.declaring.list
/* 语法分析器自己的结构     1.1.3.array */
%type<node> array.declarator
/* 语法分析器自己的结构     1.1.4.initializer */
%type<node> initializer.opt initializer initializer.list
/* 语法分析器自己的结构   1.2 function.definition 函数声明 */
%type<node> function.definition function.body  parameter.declaration
%type<list> statement.list  parameter.list
/* 语法分析器自己的结构   1.3 composite.definition 数据流计算单元声明 */
%type<node> composite.definition  composite.head  composite.head.inout
%type<node> composite.head.inout.member
%type<list> composite.head.inout.member.list
%type<node> operator.pipeline
/* 语法分析器自己的结构      1.3.2 composite.body */
%type<node> composite.body  composite.body.param.opt
%type<node> costream.composite.statement
%type<list> composite.body.statement.list
/* 语法分析器自己的结构 2. composite.body.operator  composite体内的init work window等组件  */
%type<node> composite.body.operator   operator.file.writer  operator.add
%type<node> operator.splitjoin  split.statement
%type<list> splitjoinPipeline.statement.list
%type<node> roundrobin.statement   duplicate.statement  join.statement  operator.default.call
%type<list> argument.expression.list
/* 语法分析器自己的结构 3.statement 花括号内以';'结尾的结构是statement  */
%type<node> statement labeled.statement compound.statement
%type<node> expression.statement  selection.statement   iteration.statement jump.statement
/* 语法分析器自己的结构 4.exp 计算表达式头节点  */
%type<str>  assignment.operator
%type<node> exp exp.assignable
%type<node> operator.selfdefine.body  operator.selfdefine.body.init operator.selfdefine.body.work
%type<node> operator.selfdefine.body.window.list  operator.selfdefine.window.list operator.selfdefine.window
%type<node> window.type
/* 语法分析器自己的结构 5.basic 从词法TOKEN直接归约得到的节点 */
%type<node>constant type.specifier basic.type.name  
%type<num> integerConstant
%type<doubleNum> doubleConstant
%type<str> stringConstant IDENTIFIER



/* C. 优先级标记,从上至下优先级从低到高排列 */
%right '='
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
%left '.'
%left ')' ']'
%left '(' '['

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
                                      //$$->push_back($1);
                                      $$=$1;
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
              debug ("declaring.list ::= type.specifier %s initializer.opt \n",$2->c_str());
              //debug ("declaring.list ::= type.specifier %s initializer.opt \n",map[$2]->c_str());

              $$ = NULL ;
        }
        | type.specifier 			  IDENTIFIER array.declarator initializer.opt{
              line("Line:%-3d",@1.first_line);
              debug ("declaring.list ::= type.specifier %s array.declarator initializer.opt \n",$2->c_str());
              $$ = NULL ;
        }
        | declaring.list 	',' 	IDENTIFIER initializer.opt{
              line("Line:%-3d",@1.first_line);
              debug ("declaring.list ::= declaring.list 	',' 	%s initializer.opt \n",$3->c_str());
              $$ = NULL ;
        }
        | declaring.list 	',' 	IDENTIFIER array.declarator initializer.opt{
              line("Line:%-3d",@1.first_line);
              debug ("declaring.list ::= declaring.list 	',' 	%s array.declarator initializer.opt \n",$3->c_str());
              $$ = NULL ;
        }
        ;
stream.declaring.list:
          stream.type.specifier IDENTIFIER    {
                                                  line("Line:%-3d",@1.first_line);
                                                  debug ("stream.declaring.list ::= stream.type.specifier %s \n",$2->c_str());
                                                  $$ = NULL ;
                                              }
        | stream.declaring.list ',' IDENTIFIER{
                                                  line("Line:%-3d",@1.first_line);
                                                  debug ("stream.declaring.list ::= stream.declaring.list ',' %s \n",$3->c_str());
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
                                        debug ("stream.declaration.list ::=  type.specifier %s \n",$2->c_str());
                                        $$ = NULL ;
                                    }
        | type.specifier IDENTIFIER array.declarator{
                                        line("Line:%-3d",@1.first_line);
                                        debug ("stream.declaration.list ::=  type.specifier %s array.declarator \n",$2->c_str());
                                        $$ = NULL ;
                                    }
        | stream.declaration.list ',' type.specifier IDENTIFIER {
                                        line("Line:%-3d",@1.first_line);
                                        debug ("stream.declaration.list ::=  stream.declaration.list ',' type.specifier %s \n",$4->c_str());
                                        $$ = NULL ;
                                    }
        | stream.declaration.list ',' type.specifier IDENTIFIER array.declarator{
                                        line("Line:%-3d",@1.first_line);
                                        debug ("stream.declaration.list ::=  stream.declaration.list ',' type.specifier %s array.declarator \n",$4->c_str());
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
        | '[' exp ']' {
                            line("Line:%-3d",@1.first_line);
                            debug ("array.declarator ::= '[' exp ']' \n");
                            $$ = NULL ;
                    }
        | array.declarator '[' exp ']'  {
                            line("Line:%-3d",@1.first_line);
                            debug ("array.declarator ::= array.declarator '[' exp ']' \n");
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
                                    line("Line:%-3d",@$.first_line);
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
                            $$ = new initializerNode((Loc*)&(@1));
                        }
        | initializer.list ',' initializer  {
                            line("Line:%-3d",@1.first_line);
                            debug ("initializer.list ::= initializer.list ',' initializer \n");
                            static_cast<initializerNode*>$1->value.push_back($3);
                            $$ = $1 ;
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
                debug ("function.definition ::= type.specifier %s '(' ')' function.body \n",$2->c_str());
                $$ = NULL ;
        }
        | type.specifier IDENTIFIER '(' parameter.list ')' function.body  {
                line("Line:%-3d",@1.first_line);
                debug ("function.definition ::= type.specifier %s '(' parameter.list ')' function.body \n",$2->c_str());
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
                                          debug ("parameter.declaration ::= type.specifier %s \n",$2->c_str());
                                          $$ = NULL ;
                                    }
        | type.specifier IDENTIFIER array.declarator  {
                                          line("Line:%-3d",@1.first_line);
                                          debug ("parameter.declaration ::= type.specifier %s array.declarator \n",$2->c_str());
                                          $$ = NULL ;
                                    }
        ;
function.body:
          lblock rblock                   {
                                          line("Line:%-3d",@1.first_line);
                                          debug ("function.body ::= '{' '}' \n");
                                          $$ = NULL ;
                                          }
        | lblock statement.list rblock    {
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
                                                                debug ("composite.head ::= COMPOSITE %s '(' composite.head.inout ')' \n",$2->c_str());
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
          lblock composite.body.param.opt composite.body.statement.list rblock                              { $$ = NULL ; }
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
          PIPELINE lblock  splitjoinPipeline.statement.list rblock                  { $$ = NULL ; }
        ;
splitjoinPipeline.statement.list:
          statement                                       {
                                                                line("Line:%-3d",@1.first_line);
                                                                debug ("splitjoinPipeline.statement.list ::= statement \n");
                                                                $$ = NULL ;
                                                          }
        | operator.add                                    {
                                                                line("Line:%-3d",@1.first_line);
                                                                debug ("splitjoinPipeline.statement.list ::= operator.add \n");
                                                                $$ = NULL ;
                                                          }
        | splitjoinPipeline.statement.list statement      {
                                                                line("Line:%-3d",@1.first_line);
                                                                debug ("splitjoinPipeline.statement.list ::= splitjoinPipeline.statement.list statement \n");
                                                                $$ = NULL ;
                                                          }
        | splitjoinPipeline.statement.list operator.add   {
                                                                line("Line:%-3d",@1.first_line);
                                                                debug ("splitjoinPipeline.statement.list ::= splitjoinPipeline.statement.list operator.add \n");
                                                                $$ = NULL ;
                                                          }
        ;
operator.splitjoin:
          SPLITJOIN lblock split.statement  splitjoinPipeline.statement.list  join.statement rblock     {
                                                                                                    $$ = NULL ;
                                                                                                  }
        | SPLITJOIN lblock statement.list split.statement splitjoinPipeline.statement.list join.statement rblock  {
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
        | ';'       {
                      line("Line:%-3d",@1.first_line);
                      debug ("statement ::= ';'\n");
                      $$ = NULL ;
                    }
        | error ';' {  $$ = NULL ; }
        ;

labeled.statement:
          CASE exp ':' statement                    {
                                                          line("Line:%-3d",@1.first_line);
                                                          debug ("labeled.statement ::= CASE exp ':' statement \n");
                                                          $$ = NULL ;
                                                    }
        | DEFAULT ':' statement                     {
                                                          line("Line:%-3d",@1.first_line);
                                                          debug ("labeled.statement ::= DEFAULT ':' statement \n");
                                                          $$ = NULL ;
                                                    }
        ;
compound.statement:
          lblock rblock                                               {  $$ = NULL ; }
        | lblock composite.body.statement.list rblock                 {  $$ = NULL ; }
        ;

expression.statement:
          exp ';'  {  $$ = NULL ; }
        ;

selection.statement:
          IF '(' exp ')' costream.composite.statement   {  $$ = NULL ; }
        | IF '(' exp ')' costream.composite.statement 
          ELSE costream.composite.statement             {
                                                          line("Line:%-3d",@1.first_line);
                                                          debug ("selection.statement ::= if(exp) costream.composite.statement else ...\n");
                                                          $$ = NULL ;
                                                        }
        | SWITCH '(' exp ')' statement                  {  $$ = NULL ; }
        ;
iteration.statement:
          WHILE '(' exp ')' costream.composite.statement                          {  $$ = NULL ; }
        | DO  costream.composite.statement WHILE '(' exp ')' ';'                  {  $$ = NULL ; }
        | FOR '(' exp   ';' exp ';' exp ')'  costream.composite.statement         {
                                                                                        line("Line:%-3d",@1.first_line);
                                                                                        debug ("iteration.statement ::= for(...)  costream.composite.statement \n");
                                                                                        $$ = NULL ;
                                                                                  }
        | FOR '(' declaration  ';' exp ';' exp ')'  costream.composite.statement  {  $$ = NULL ; }
        | FOR '(' error ')' costream.composite.statement                          {  $$ = NULL ; }
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
          '='             { $$ = new string("=")  ; }
        | MULTassign      { $$ = new string("*=") ; }
        | DIVassign       { $$ = new string("/=") ; }
        | MODassign       { $$ = new string("%=") ; }
        | PLUSassign      { $$ = new string("+=") ; }
        | MINUSassign     { $$ = new string("-=") ; }
        | LSassign        { $$ = new string("<=") ; }
        | RSassign        { $$ = new string(">=") ; }
        | ANDassign       { $$ = new string("&=") ; }
        | ERassign        { $$ = new string("^=") ; }
        | ORassign        { $$ = new string("|=") ; }
        ;
exp.assignable:
          IDENTIFIER                        
            { line("Line:%-3d",@1.first_line);debug ("exp.assignable ::= %s\n",$1->c_str()); $$ = NULL ; }
        | IDENTIFIER  array.declarator      
            { line("Line:%-3d",@1.first_line);debug ("exp.assignable ::= %s array.declarator\n",$1->c_str()); $$ = NULL ; }  
        ; 
exp:      exp.assignable                    
            { line("Line:%-3d",@1.first_line);debug ("exp ::= exp.assignable\n"); $$ = NULL ; }
        | exp.assignable '.' IDENTIFIER     
            { line("Line:%-3d",@1.first_line);debug ("exp ::= exp.assignable '.' %s\n",$3->c_str()); $$ = NULL ; }
        | exp.assignable '.' IDENTIFIER array.declarator 
            { line("Line:%-3d",@1.first_line);debug ("exp ::= exp.assignable '.' %s array.declarator\n",$3->c_str()); $$ = NULL ; }
        | constant        { line("Line:%-3d",@1.first_line);debug ("exp ::= constant\n"); $$ = $1 ; }
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
        | exp assignment.operator exp                         { line("Line:%-3d",@1.first_line);debug ("exp ::= exp.assignable assignment.operator exp\n"); $$ = NULL ; }
        | IDENTIFIER '(' argument.expression.list ')'         { line("Line:%-3d",@1.first_line);debug ("exp ::= function ( exp.list )\n"); $$ = NULL ; }
        | FILEREADER '(' ')' '(' stringConstant ')'           { line("Line:%-3d",@1.first_line);debug ("exp ::= FILEREADER()( stringConstant )\n"); $$ = NULL ; }
        | IDENTIFIER '('  ')' operator.selfdefine.body        { line("Line:%-3d",@1.first_line);debug ("exp ::= %s() operator.selfdefine.body\n",$1->c_str()); $$ = NULL ; }
        | IDENTIFIER '(' argument.expression.list ')' operator.selfdefine.body   { $$ = NULL ; }
        | IDENTIFIER '('  ')'  '(' ')'                        { line("Line:%-3d",@1.first_line);debug ("exp ::= %s()()\n",$1->c_str()); $$ = NULL ; }
        | IDENTIFIER '('  ')'  '(' argument.expression.list ')'                            { $$ = NULL ; }
        | IDENTIFIER '(' argument.expression.list ')'  '(' ')'                             { $$ = NULL ; }
        | IDENTIFIER '(' argument.expression.list ')'  '(' argument.expression.list ')'    { $$ = NULL ; }
        |  SPLITJOIN '(' argument.expression.list ')'  lblock split.statement  splitjoinPipeline.statement.list  join.statement rblock                { $$ = NULL ; }
        |  SPLITJOIN '(' argument.expression.list ')'  lblock statement.list split.statement splitjoinPipeline.statement.list  join.statement rblock  { $$ = NULL ; }
        |   PIPELINE '(' argument.expression.list ')'  lblock splitjoinPipeline.statement.list rblock                                                 { $$ = NULL ; }
        ;

operator.selfdefine.body:
       lblock operator.selfdefine.body.init operator.selfdefine.body.work operator.selfdefine.body.window.list rblock
        {
            line("Line:%-3d",@1.first_line);
            debug ("operator.selfdefine.body ::=  { init work window.list }\n");
            $$ = NULL ;
        }
     | lblock statement.list operator.selfdefine.body.init  operator.selfdefine.body.work operator.selfdefine.body.window.list rblock
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
      | WINDOW lblock operator.selfdefine.window.list rblock  {
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
                                                    debug ("operator.selfdefine.window ::= %s window.type (sliding? (arg_list?))\n",$1->c_str());
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
/* 设置变量作用域相关 */
lblock: '{'  { EnterScope();  }  
rblock: '}'  { ExitScope();   }

constant:
          doubleConstant    {
                                line("Line:%-3d",@1.first_line);
                                debug ("constant ::= doubleConstant | value:=%lf\n",$1);
                                $$ = new constantNode("double",$1,(Loc*)&(@1)) ;
                            }
        | integerConstant   {
                                line("Line:%-3d",@1.first_line);
                                debug ("constant ::= integerConstant | value:=%d\n",$1);
                                $$ = new constantNode("interger",$1,(Loc*)&(@1)) ;
                            }
        | stringConstant    {
                                line("Line:%-3d",@1.first_line);
                                debug ("constant ::= stringConstant | value:=%s\n",$1->c_str());
                                $$ = new constantNode("string",*($1),(Loc*)&(@1)) ;
                            }
        ;
type.specifier:
          basic.type.name       {
                                    line("Line:%-3d",@1.first_line);
                                    debug ("type.specifier ::=  basic.type.name \n");
                                    $$ = $1 ;
                                }
        | CONST basic.type.name {
                                    line("Line:%-3d",@2.first_line);
                                    debug ("type.specifier ::=  CONST basic.type.name \n");
                                    (static_cast<primaryNode*>$2)->isConst=true;
                                    $$ = $2 ; /* const 暂时还未处理*/
                                }
        ;
basic.type.name:
          INT   {
                      line("Line:%-3d",@1.first_line);
                      debug ("basic.type.name ::=  INT \n");
                      $$ = new primaryNode("int",(Loc*)&(@1) );
                }
        | LONG  {
                      line("Line:%-3d",@1.first_line);
                      debug ("basic.type.name ::=  LONG \n");
                      $$ = new primaryNode("LONG",(Loc*)&(@1) ) ;
                }
        | LONG LONG {
                      line("Line:%-3d",@1.first_line);
                      debug ("basic.type.name ::=  LONG LONG  \n");
                      $$ = new primaryNode("LONG LONG",(Loc*)&(@1) ) ;
                    }
        | FLOAT {
                      line("Line:%-3d",@1.first_line);
                      debug ("basic.type.name ::=  FLOAT \n");
                      $$ = new primaryNode("FLOAT",(Loc*)&(@1) ) ;
                }
        | DOUBLE{
                      line("Line:%-3d",@1.first_line);
                      debug ("basic.type.name ::=  DOUBLE \n");
                      $$ = new primaryNode("DOUBLE",(Loc*)&(@1) ) ;
                }
        | STRING{
                      line("Line:%-3d",@1.first_line);
                      debug ("basic.type.name ::=  STRING \n");
                      $$ = new primaryNode("STRING",(Loc*)&(@1) ) ;
                }
        ;
%%
/* ----语法树结束----*/
void yyerror (const char *msg)
{
    error ("%s", msg);
}

