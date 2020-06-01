%{
//#define DEBUG
#include "defines.h"
#include "node.h"
#include "symbol.h"
#include "nodetype.h"
#include "layertype.h"
#include "unfoldComposite.h"
//SymbolTable *top=new SymbolTable(NULL);
//SymbolTable *saved=top;
//extern SymbolTable S;
extern list<Node*> *Program;
extern UnfoldComposite *unfold;
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
%token SEQUENTIAL DENSE CONV2D MAXPOOLING2D AVERAGEPOOLING2D ACTIVATION DROPOUT
/* B.下面是语法分析器自己拥有的文法结构和类型声明 */

/* 语法分析器自己的结构 1. 文法一级入口*/
%type<list> translation.unit 
%type<node> external.definition
/* 语法分析器自己的结构   1.1.declaration */

%type<node> declaration declaring.list stream.declaring.list
%type<node> stream.type.specifier
%type<node> stream.declaration.list 
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
%type<node> exp
%type<node> operator.selfdefine.body  operator.selfdefine.body.init operator.selfdefine.body.work
%type<node> operator.selfdefine.body.window.list operator.selfdefine.window
%type<list> operator.selfdefine.window.list 
%type<node> window.type
/* 语法分析器自己的结构 5.basic 从词法TOKEN直接归约得到的节点 */
%type<node>constant type.specifier basic.type.name idNode
%type<num> integerConstant
%type<doubleNum> doubleConstant
%type<str> stringConstant IDENTIFIER
/* 语法分析器自己的结构 6. 深度学习扩展文法*/
%type<str> DENSE CONV2D MAXPOOLING2D AVERAGEPOOLING2D ACTIVATION DROPOUT
%type<node> operator.layer operator.sequential.add
%type<list> sequential.statement.list
%type<node> tuple
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
prog.start: translation.unit { Program = $1 ; };

translation.unit:
          external.definition   {
                                      line("Line:%-4d",@$.first_line);
                                      debug ("translation.unit ::= external.definition\n");
                                      $$ = new list<Node*>({$1});
                                }
        | translation.unit external.definition  {
                                                      line("Line:%-4d",@$.first_line);
                                                      debug ("translation.unit ::= translation.unit external.definition\n");
                                                      $$->push_back($2);
                                                }
        ;
external.definition:
          declaration           {
                                      line("Line:%-4d",@$.first_line);
                                      debug ("external.definition ::= declaration\n");
                                      $$ = $1;
                                }
        | function.definition   {
                                      line("Line:%-4d",@$.first_line);
                                      debug ("external.definition ::= function.definition\n");
                                      $$ = $1 ;
                                }
        | composite.definition  {
                                      line("Line:%-4d",@$.first_line);
                                      debug ("external.definition ::= composite.definition\n");
                                      $$ = $1 ;
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
                                      line("Line:%-4d",@$.first_line);
                                      debug ("declaration ::= declaring.list ';' \n");
                                      $$ = $1 ;
                                }
    | stream.declaring.list ';' {
                                      line("Line:%-4d",@$.first_line);
                                      debug ("declaration ::= stream.declaring.list ';' \n");
                                      $$ = $1 ;
                                }
    ;
declaring.list:
          type.specifier      idNode       initializer.opt  {
              //top->put(static_cast<idNode*>($2)->name,static_cast<idNode*>($2));
              (static_cast<idNode*>$2)->init = $3;
              $$ = new declareNode((primNode*)$1,(static_cast<idNode*>$2),@$) ;
              line("Line:%-4d",@$.first_line);
              debug ("declaring.list ::= type.specifier(%s) IDENTIFIER(%s) initializer.opt \n",$1->toString().c_str(),$2->toString().c_str());
          }
        | declaring.list     ',' idNode        initializer.opt{
              //top->put(static_cast<idNode*>($3)->name,static_cast<idNode*>($3));
              (static_cast<idNode*>$3)->init = $4;
              ((declareNode*)$1)->id_list.push_back((static_cast<idNode*>$3));
              $$=$1;
              line("Line:%-4d",@$.first_line);
              debug ("declaring.list ::= declaring.list ',' IDENTIFIER(%s) initializer.opt \n",$3->toString().c_str());
          }
        ;
stream.declaring.list:
          stream.type.specifier IDENTIFIER    {
                                                  line("Line:%-4d",@$.first_line);
                                                  debug ("stream.declaring.list ::= stream.type.specifier %s \n",$2->c_str());
                                                  idNode *id=new idNode(*($2),@$);
                                                  //top->put(*($2),id);
                                                  //issue : 声明的变量和stream内部变量放在了一起
                                                  ((strdclNode*)($1))->declare_stream_id.push_back(id);
                                                  $$ = $1 ;
                                              }
        | stream.declaring.list ',' IDENTIFIER{
                                                  line("Line:%-4d",@$.first_line);
                                                  debug ("stream.declaring.list ::= stream.declaring.list ',' %s \n",$3->c_str());
                                                  idNode *id=new idNode(*($3),@$);
                                                  //top->put(*($3),id);
                                                  ((strdclNode*)($1))->declare_stream_id.push_back(id);
                                                  $$ = $1 ;
                                              }
        ;
stream.type.specifier:
          STREAM '<' stream.declaration.list '>'{
                                                    line("Line:%-4d",@$.first_line);
                                                    debug ("stream.type.specifier ::=  STREAM '<' stream.declaration.list(%s) '>' \n",$3->toString().c_str());
                                                    $$ = $3 ;
                                                }
        ;
stream.declaration.list:
          type.specifier idNode     {
                                        //top->put(static_cast<idNode*>($2)->name,static_cast<idNode*>($2));
                                        (static_cast<idNode*>$2)->valType = (static_cast<primNode*>$1)->name;
                                        $$ = new strdclNode((idNode*)$2,@$) ;
                                    }
        | stream.declaration.list ',' type.specifier idNode {
                                        //top->put(static_cast<idNode*>($4)->name,static_cast<idNode*>($4));
                                        (static_cast<idNode*>$4)->valType = (static_cast<primNode*>$3)->name;
                                        (static_cast<strdclNode*>$1)->id_list.push_back((idNode*)$4);
                                        $$ = $1 ;
                                    }
        ;

/*************************************************************************/
/*                      1.1.3 array ( int a[] )                          */
/*************************************************************************/
array.declarator:
          '[' ']'   {
                            //line("Line:%-4d",@$.first_line);
                            //debug ("array.declarator ::= '[' ']' \n");
                            $$ = new arrayNode(NULL, @$) ;
                    }
        | '[' exp ']' {
                            // line("Line:%-4d",@$.first_line);
                            // debug ("array.declarator ::= '[' exp ']' \n");
                            $$ = new arrayNode((expNode*)$2, @$) ;
                    }
        | array.declarator '[' exp ']'  {
                            // line("Line:%-4d",@$.first_line);
                            // debug ("array.declarator ::= array.declarator '[' exp ']' \n");
                            (static_cast<arrayNode*>$1)->arg_list.push_back((expNode*)$3); $$ = $1;
                    }
        | array.declarator '[' ']'{
                                          yyerror ("array declaration with illegal empty dimension\n");
                                          exit(-1);
                                  }
        ;
/*************************************************************************/
/*                      1.1.4 initializer                                */
/*************************************************************************/
initializer.opt:
          /* nothing */         {
                                    line("Line:%-4d",@$.first_line);
                                    debug ("initializer.opt ::= nothing \n");
                                    $$ = NULL ;
                                }
        | '=' initializer       {
                                    line("Line:%-4d",@$.first_line);
                                    debug ("initializer.opt ::= '=' initializer(%s) \n",$2->toString().c_str());
                                    $$ = $2 ;
                                }
        ;
initializer:
          '{' initializer.list '}'      { $$ = $2 ; }
        | '{' initializer.list ',' '}'  { $$ = $2 ; }
        | exp                           { $$ = $1 ; }
        ;

initializer.list:
          initializer   {
                            $$ = new initNode($1,@$);
                        }
        | initializer.list ',' initializer  {
                            static_cast<initNode*>$1->value.push_back($3);
                            $$ = $1 ;
                        }
        ;
/*************************************************************************/
/*                      1.1.5 tuple ( (1, 2) )                          */
/*************************************************************************/
tuple:
          '(' argument.expression.list ')' {
                            $$ = new tupleNode($2, @$);
                            line("Line:%-4d",@$.first_line);
                            debug("tuple");
                        }
/*************************************************************************/
/*              1.2 function.definition 函数声明                          */
/*                      1.2.1 parameter.list                             */
/*                      1.2.1 function.body {函数体}                      */
/*************************************************************************/
function.definition:
          type.specifier IDENTIFIER '(' ')' function.body {
                $$ = new funcDclNode((primNode*)$1,$2,NULL,(funcBodyNode*)$5) ;
                line("Line:%-4d",@$.first_line);
                debug ("function.definition ::=  %s\n",$$->toString().c_str());
        }
        | type.specifier IDENTIFIER '(' parameter.list ')' function.body  {
                $$ = new funcDclNode((primNode*)$1,$2,$4,(funcBodyNode*)$6) ;
                line("Line:%-4d",@$.first_line);
                debug ("function.definition ::=  %s\n",$$->toString().c_str());
                
        }
        ;

parameter.list:
          parameter.declaration                    { $$ = new list<Node*>({$1}); }
        | parameter.list ',' parameter.declaration { $$->push_back($3); }
        | parameter.list '=' initializer {
                //函数参数里不支持初始化
                yyerror( "parameter.list in function definations cannot have initializers\n");
                exit(-1);
          }
        | parameter.list ',' error
        ;
parameter.declaration:
          type.specifier idNode     {
                                          //top->put(static_cast<idNode*>($2)->name,static_cast<idNode*>($2));
                                          (static_cast<idNode*>$2)->valType = (static_cast<primNode*>$1)->name;
                                          $$ = $2;
                                    }
        ;
function.body:
          lblock rblock                   {
                                          line("Line:%-4d",@$.first_line);
                                          debug ("function.body ::= '{' '}' \n");
                                          $$ = new funcBodyNode(NULL) ;
                                          }
        | lblock statement.list rblock    {
                                          line("Line:%-4d",@$.first_line);
                                          debug ("function.body ::= '{' statement.list '}' \n");
                                          $$ = new funcBodyNode($2) ;
                                    }
        ;

statement.list:
          statement                   {   $$ = new list<Node*> ({$1});  }
        | statement.list statement    {   $$->push_back($2);            }
        ;


/*************************************************************************/
/*              1.3 composite.definition 数据流计算单元声明                */
/*                      1.3.1 composite.head                             */
/*                      1.3.2 composite.body                             */
/*************************************************************************/
composite.definition:
      composite.head composite.body {
                                          line("Line:%-4d",@$.first_line);
                                          debug ("composite.definition ::= composite.head composite.body \n");
                                          $$ = new compositeNode((compHeadNode*)$1,(compBodyNode*)$2,@$) ;
                                          //S.InsertCompositeSymbol(((compositeNode*)$$)->compName,(compositeNode*)$$);
                                    }
    ;
composite.head:
      COMPOSITE IDENTIFIER '(' composite.head.inout ')'   {
            line("Line:%-4d",@$.first_line);
            debug ("composite.head ::= COMPOSITE %s '(' composite.head.inout ')' \n",$2->c_str());
            $$ = new compHeadNode(*($2),(ComInOutNode*)$4) ;
      }
    ;
composite.head.inout:
      /*empty*/                                                                           { $$ = NULL ;}
    | INPUT composite.head.inout.member.list                                              { $$ = new ComInOutNode($2,NULL, @$)  ; }
    | INPUT composite.head.inout.member.list ',' OUTPUT composite.head.inout.member.list  { $$ = new ComInOutNode($2,$5,   @$)  ; }
    | OUTPUT composite.head.inout.member.list                                             { $$ = new ComInOutNode(NULL,$2, @$)  ; }
    | OUTPUT composite.head.inout.member.list ',' INPUT composite.head.inout.member.list  { $$ = new ComInOutNode($5,$2,   @$)  ; }
    ;
composite.head.inout.member.list:
      composite.head.inout.member                                       {     $$ = new list<Node*>({$1});   }
    | composite.head.inout.member.list ',' composite.head.inout.member  {     $$->push_back($3);            }
    ;
composite.head.inout.member:
      stream.type.specifier IDENTIFIER    { 
                  line("Line:%-4d",@$.first_line);
                  debug ("composite.head.inout.member ::= stream.type.specifier IDENTIFIER(%s)  \n",$2->c_str());
                  idNode *id = new idNode(*($2),@$);
                 // top->put(*($2),id);
                  $$ = new inOutdeclNode($1,id,@$) ; 
            }
    ;
/*************************************************************************/
/*                      1.3.2 composite.body                             */
/*                        1.3.2.1 composite.body.param.opt               */
/*                        1.3.2.2 composite.body.declaration.list        */
/*                        1.3.2.3 composite.body.statement.list          */
/*************************************************************************/
composite.body:
          lblock composite.body.param.opt composite.body.statement.list rblock     { $$ = new compBodyNode((paramNode*)$2,$3) ; }
        ;
composite.body.param.opt:
          /*empty*/                 { $$ = NULL ; }
        | PARAM parameter.list ';'  {
                                          line("Line:%-4d",@$.first_line);
                                          debug ("composite.body.param.opt ::= PARAM parameter.list \n");
                                          $$ = new paramNode($2) ;
                                    }
        ;
composite.body.statement.list:
          costream.composite.statement                                { 
                                                                        $$=new list<Node *>();
                                                                        if($1!=NULL) $$->push_back($1);
                                                                      }
        | composite.body.statement.list costream.composite.statement  { if($2!=NULL) $$->push_back($2); }
        ;
costream.composite.statement:
          composite.body.operator   {
                                          line("Line:%-4d",@$.first_line);
                                          debug ("costream.composite.statement ::= composite.body.operator \n");
                                          $$ = $1 ;
                                    }
        | statement                 {
                                          line("Line:%-4d",@$.first_line);
                                          debug ("costream.composite.statement ::= statement \n");
                                          $$ = $1 ;
                                    }
        ;

/*****************************************************************************/
/*        2. composite.body.operator  composite体内的init work window等组件   */
/*             2.1   ADD operator.pipeline                                   */
/*             2.2   ADD operator.splitjoin                                  */
/*             2.3   ADD operator.default.call                               */
/*             2.4   ADD operator.layer                                      */
/*****************************************************************************/
composite.body.operator:
          operator.file.writer      {
                                          line("Line:%-4d",@$.first_line);
                                          debug ("composite.body.operator ::= operator.file.writer \n");
                                          $$ = $1 ;
                                    }
        | operator.add              {
                                          line("Line:%-4d",@$.first_line);
                                          debug ("composite.body.operator ::= operator.add \n");
                                          $$ = $1 ;
                                    }
        ;
operator.file.writer:
          FILEWRITER '(' IDENTIFIER ')' '(' stringConstant ')' ';' { $$ = NULL ; }
        | FILEWRITER '(' IDENTIFIER ')' '(' ')' ';' {
                                                       yyerror("FILEWRITER must have the filename of the output file.\n");
                                                    }
        ;
operator.add:
          ADD operator.pipeline     {
                                          line("Line:%-4d",@$.first_line);
                                          debug ("operator.add ::= ADD operator.pipeline \n");
                                          $$ = new addNode((pipelineNode*)$2,@$) ;
                                    }
        | ADD operator.splitjoin    {
                                          line("Line:%-4d",@$.first_line);
                                          debug ("operator.add ::= ADD operator.splitjoin \n");
                                          $$ = new addNode((splitNode*)$2,@$) ;
                                    }
        | ADD operator.default.call {
                                          line("Line:%-4d",@$.first_line);
                                          debug ("operator.add ::= ADD operator.default.call \n");
                                          $$ = new addNode((compositeCallNode*)$2,@$) ;
                                    }
        ;
operator.pipeline:
          PIPELINE lblock  splitjoinPipeline.statement.list rblock     { $$ = new pipelineNode(NULL,$3,NULL,@$) ; }
        ;
operator.sequential.add:
          ADD operator.layer              {
                                                line("Line:%-4d",@$.first_line);
                                                debug("operator.sequential.add ::= ADD operator.layer \n");
                                                $$ = new addNode((layerNode*)$2, @$);
                                          }
        ;
operator.layer:
          DENSE '(' argument.expression.list ')' ';'      {
                                                                line("Line%-4d", @$.first_line);
                                                                debug("operator.layer ::=DENSE ( argument.expression.list );\n");
                                                                $$ = new denseLayerNode("dense", $3, @$);
                                                                debug("Create dense layer!\n");
                                                          }
          | CONV2D '(' argument.expression.list ')' ';'      {
                                                                line("Line%-4d", @$.first_line);
                                                                debug("operator.layer ::=CONV2 ( argument.expression.list );\n");
                                                                layerNode* layer = new conv2DLayerNode("conv2D", $3, @$);
                                                                $$ = layer;
                                                                debug("Create conv2D layer!\n");
                                                          }
          | MAXPOOLING2D '(' argument.expression.list ')' ';' {
                                                                line("Line%-4d", @$.first_line);
                                                                debug("operator.layer ::=MAXPOOLING2D ( argument.expression.list );\n");
                                                                layerNode* layer = new maxPooling2DLayerNode("maxPooling2D", $3, @$);
                                                                $$ = layer;
                                                                debug("Create maxPooling2D layer!\n");
                                                            }
          | AVERAGEPOOLING2D '(' argument.expression.list ')' ';' {
                                                                line("Line%-4d", @$.first_line);
                                                                debug("operator.layer ::=AVERAGEPOOLING2D ( argument.expression.list );\n");
                                                                layerNode* layer = new averagePooling2DLayerNode("averagePooling2D", $3, @$);
                                                                $$ = layer;
                                                                debug("Create averagePooling2D layer!\n");
                                                            }
          | ACTIVATION '(' argument.expression.list ')' ';' {
                                                                line("Line%-4d", @$.first_line);
                                                                debug("operator.layer ::=ACTIVATION ( argument.expression.list );\n");
                                                                layerNode* layer = new activationLayerNode("activation", $3, @$);
                                                                $$ = layer;
                                                                debug("Create activation layer!\n");
                                                            }
         |  DROPOUT '(' argument.expression.list ')' ';' {
                                                                line("Line%-4d", @$.first_line);
                                                                debug("operator.layer ::=DROPOUT ( argument.expression.list );\n");
                                                                layerNode* layer = new dropoutLayerNode("dropout", $3, @$);
                                                                $$ = layer;
                                                                debug("Create dropout layer!\n");
                                                            }
        /* other layer... */
        ;
splitjoinPipeline.statement.list:
          statement                                       {
                                                                line("Line:%-4d",@$.first_line);
                                                                debug ("splitjoinPipeline.statement.list ::= statement \n");
                                                                $$ = new list<Node*>({$1});
                                                          }
        | operator.add                                    {
                                                                line("Line:%-4d",@$.first_line);
                                                                debug ("splitjoinPipeline.statement.list ::= operator.add \n");
                                                                $$ = new list<Node*>({$1});
                                                          }
        | splitjoinPipeline.statement.list statement      {
                                                                line("Line:%-4d",@$.first_line);
                                                                debug ("splitjoinPipeline.statement.list ::= splitjoinPipeline.statement.list statement \n");
                                                                $$->push_back($2);
                                                          }
        | splitjoinPipeline.statement.list operator.add   {
                                                                line("Line:%-4d",@$.first_line);
                                                                debug ("splitjoinPipeline.statement.list ::= splitjoinPipeline.statement.list operator.add \n");
                                                                $$->push_back($2);
                                                          }
        ;
operator.splitjoin:
          SPLITJOIN lblock split.statement  splitjoinPipeline.statement.list  join.statement rblock     {
                   $$=new splitjoinNode(NULL,NULL,(splitNode*)$3,NULL,$4,(joinNode*)$5,@$);
            }
        | SPLITJOIN lblock statement.list split.statement splitjoinPipeline.statement.list join.statement rblock  {
                   $$=new splitjoinNode(NULL,NULL,(splitNode*)$4,$3,$5,(joinNode*)$6,@$);
            }
        ;
split.statement:
          SPLIT duplicate.statement                        { $$ = new splitNode($2,@$) ; }
        | SPLIT roundrobin.statement                       { $$ = new splitNode($2,@$) ; }
        ;
roundrobin.statement:
          ROUNDROBIN '(' ')' ';'                            { $$ = new roundrobinNode(NULL,@$) ; }
        | ROUNDROBIN '(' argument.expression.list ')' ';'   { $$ = new roundrobinNode($3,@$) ; }
        ;
duplicate.statement:
          DUPLICATE '('  ')' ';'                            { $$ = new duplicateNode(NULL,@$) ; }
        | DUPLICATE '(' exp ')'  ';'                        { $$ = new duplicateNode((expNode*)$3,@$)  ;}
        ;
join.statement:
          JOIN roundrobin.statement                         { $$ = new joinNode((roundrobinNode*)$2,@$) ;}
        ;
argument.expression.list:
          exp                                               { $$ = new list<Node*>({$1}); line("test%-4d",@$.first_line); debug("param\n");}
        | tuple                                            { $$ = new list<Node*>({$1}); line("test%-4d",@$.first_line); debug("param\n");}
        | argument.expression.list ',' exp                  { $$ ->push_back($3);line("test%-4d",@$.first_line); debug("push param\n");}
        | argument.expression.list ',' tuple               { $$ ->push_back($3);line("test%-4d",@$.first_line); debug("push tuple param\n"); }
        ;
operator.default.call:
          IDENTIFIER  '(' ')' ';'                           { 
                  //$$ = new compositeCallNode(NULL,*($1),NULL,NULL,S.LookupCompositeSymbol(*($1)),@$);
                  $$ = new compositeCallNode(NULL,*($1),NULL,NULL,NULL,@$);
            }
        | IDENTIFIER  '(' argument.expression.list ')' ';'  {
                  //$$ = new compositeCallNode(NULL,*($1),$3,NULL,S.LookupCompositeSymbol(*($1)),@$);
                  $$ = new compositeCallNode(NULL,*($1),$3,NULL,NULL,@$);
            }
        ;
sequential.statement.list:
          statement                                          {
                                                                line("Line:%-4d", @$.first_line);
                                                                debug("sequential.statement.list ::= statement \n");
                                                                $$ = new list<Node *>({$1});
                                                             }
        | operator.sequential.add                             {
                                                                line("Line:%-4d", @$.first_line);
                                                                debug("sequential.statement.list ::= operator.sequential.add\n");
                                                                $$ = new list<Node *>({$1});
                                                             }
        | sequential.statement.list operator.sequential.add    {
                                                                line("Line:%-4d", @$.first_line);
                                                                debug("sequential.statement.list ::= sequential.statement.list operator.sequential.add\n");
                                                                $$ ->push_back($2);
                                                             }
        ;
/*************************************************************************/
/*        3. statement 花括号内以';'结尾的结构是statement                  */
/*************************************************************************/
statement:
          labeled.statement
        | compound.statement   { line("Line:%-4d",@$.first_line);debug("statement::=compound.statement\n");}
        | expression.statement
        | selection.statement
        | iteration.statement
        | jump.statement
        | declaration
        | ';'       {
                      $$ = NULL ;
                      debug("statement :: NULL");
                    }
        | error ';' {  $$ = NULL ; }
        ;

labeled.statement:
          CASE exp ':' statement                    {
                                                          line("Line:%-4d",@$.first_line);
                                                          debug ("labeled.statement ::= CASE exp ':' statement \n");
                                                          $$ = new caseNode((expNode*)$2,$4,@$) ;
                                                    }
        | DEFAULT ':' statement                     {
                                                          line("Line:%-4d",@$.first_line);
                                                          debug ("labeled.statement ::= DEFAULT ':' statement \n");
                                                          $$ = new defaultNode($3,@$) ;
                                                    }
        ;
compound.statement:
          lblock rblock                                     {  $$ = new blockNode(NULL,@$,@$); }
        | lblock composite.body.statement.list rblock       {  $$ = new blockNode($2,@$,@$) ; }
        ;

expression.statement:
          exp ';'  {  $$ = $1 ; }
        ;

selection.statement:
          IF '(' exp ')' costream.composite.statement   {  $$ = new ifNode((expNode*)$3,$5,@$) ; }
        | IF '(' exp ')' costream.composite.statement 
          ELSE costream.composite.statement             {
                                                          line("Line:%-4d",@$.first_line);
                                                          debug ("selection.statement ::= if(exp) costream.composite.statement else ...\n");
                                                          $$ = new ifElseNode((expNode*)$3,$5,$7,@$);
                                                        }
        | SWITCH '(' exp ')' statement                  {  $$ = new switchNode((expNode*)$3,$5,@$); }
        ;
iteration.statement:
          WHILE '(' exp ')' costream.composite.statement                          {  $$ = new whileNode((expNode*)$3,$5,@$) ; }
        | DO  costream.composite.statement WHILE '(' exp ')' ';'                  {  $$ = new doNode($2,(expNode*)$5,@$) ; }
        | FOR '(' exp   ';' exp ';' exp ')'  costream.composite.statement     {
                  line("Line:%-4d",@$.first_line);
                  debug ("iteration.statement ::= for(...)  costream.composite.statement \n");
                  $$ = new forNode($3,(expNode*)$5,(expNode*)$7,$9,@$) ;
            }
        | FOR '(' declaration  ';' exp ';' exp ')'  costream.composite.statement  {  
                  $$ = new forNode($3,(expNode*)$5,(expNode*)$7,$9,@$) ; 
            }
        | FOR '(' error ')' costream.composite.statement                          {  $$ = NULL ; }
        ;
jump.statement:
          CONTINUE ';'        {  $$ = new continueNode(@$) ; }
        | BREAK ';'           {  $$ = new breakNode(@$) ; }
        | RETURN exp ';'      {  $$ = new returnNode((expNode*)$2,@$) ; }
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
        | LSassign        { $$ = new string("<<=") ; }
        | RSassign        { $$ = new string(">>=") ; }
        | ANDassign       { $$ = new string("&=") ; }
        | ERassign        { $$ = new string("^=") ; }
        | ORassign        { $$ = new string("|=") ; }
        ;
exp:      idNode          { line("Line:%-4d",@$.first_line);
                            debug ("exp ::= idnode \n");
                            //$1=top->get(static_cast<idNode*>$1->name);
                            $$ = $1 ;  }
        | constant        { $$ = $1 ; }
        /* | tuple          { $$ = $1 ; } */
        | idNode '.' idNode { $$ = new binopNode((expNode*)$1,".",(expNode*)$3,@$) ; }
        | exp '+' exp     { $$ = new binopNode((expNode*)$1,"+",(expNode*)$3,@$) ; }
        | exp '-' exp     { $$ = new binopNode((expNode*)$1,"-",(expNode*)$3,@$) ; }
        | exp '*' exp     { $$ = new binopNode((expNode*)$1,"*",(expNode*)$3,@$) ; }
        | exp '/' exp     { $$ = new binopNode((expNode*)$1,"/",(expNode*)$3,@$) ; }
        | exp '%' exp     { $$ = new binopNode((expNode*)$1,"%",(expNode*)$3,@$) ;}
        | exp OROR exp    { $$ = new binopNode((expNode*)$1,"||",(expNode*)$3,@$) ;}
        | exp ANDAND exp  { $$ = new binopNode((expNode*)$1,"&&",(expNode*)$3,@$) ;}
        | exp '|' exp     { $$ = new binopNode((expNode*)$1,"|",(expNode*)$3,@$) ;}
        | exp '&' exp     { $$ = new binopNode((expNode*)$1,"&",(expNode*)$3,@$) ;}
        | exp '^' exp     { $$ = new binopNode((expNode*)$1,"^",(expNode*)$3,@$) ;}
        | exp LS exp      { $$ = new binopNode((expNode*)$1,"<<",(expNode*)$3,@$) ;}
        | exp RS exp      { $$ = new binopNode((expNode*)$1,">>",(expNode*)$3,@$) ;}
        | exp '<' exp     { $$ = new binopNode((expNode*)$1,"<",(expNode*)$3,@$) ;}
        | exp '>' exp     { $$ = new binopNode((expNode*)$1,">",(expNode*)$3,@$) ;}
        | exp LE exp      { $$ = new binopNode((expNode*)$1,"<=",(expNode*)$3,@$) ;}
        | exp GE exp      { $$ = new binopNode((expNode*)$1,">=",(expNode*)$3,@$) ;}
        | exp EQ exp      { $$ = new binopNode((expNode*)$1,"==",(expNode*)$3,@$) ;}
        | exp NE exp      { $$ = new binopNode((expNode*)$1,"!=",(expNode*)$3,@$) ;}
        | exp '?' exp ':' exp { $$ = new ternaryNode((expNode*)$1,(expNode*)$3,(expNode*)$5,@$); }
        | '+' exp         { $$ = new unaryNode("+",(expNode*)$2,@$) ; }
        | '-' exp         { $$ = new unaryNode("-",(expNode*)$2,@$) ; }
        | '~' exp         { $$ = new unaryNode("~",(expNode*)$2,@$) ; }
        | '!' exp         { $$ = new unaryNode("!",(expNode*)$2,@$) ; }
        |  ICR exp        { $$ = new unaryNode("PREINC",(expNode*)$2,@$) ; }
        |  DECR exp       { $$ = new unaryNode("PREDEC",(expNode*)$2,@$) ; }
        |  exp ICR        { $$ = new unaryNode("POSTINC",(expNode*)$1,@$) ; }
        |  exp DECR       { $$ = new unaryNode("POSTDEC",(expNode*)$1,@$) ; }
        |  '(' exp ')'    { $$ = new parenNode((expNode*)$2,@$) ; }
        | '(' basic.type.name ')' exp     { $$ = new castNode((primNode*)$2,(expNode*)$4,@$); }
        | exp assignment.operator exp     { 
                              line("Line:%-4d",@$.first_line);
                              debug ("exp ::= exp(%s) assignment.operator(%s) exp(%s)\n",$1->toString().c_str(),$2->c_str(),$3->toString().c_str()); 
                              $$ = new binopNode((expNode*)$1,*($2),(expNode*)$3,@$ ) ;
                              //当类型为splitjoin，pipeline，operator，compositecall时设置输出流
                              if($3->type==SplitJoin){

                                    list<Node*> *outputs=new list<Node*>({$1});
                                    ((splitjoinNode*)$3)->outputs=outputs;
                              }
                              else if($3->type==Pipeline){
                                    list<Node*> *outputs=new list<Node*>({$1});
                                    ((pipelineNode*)$3)->outputs=outputs;
                              }
                              else if($3->type==CompositeCall){
                                    ((compositeCallNode*)$3)->outputs=new list<Node*>({$1});
                              }
                              else if($3->type==Operator_){
                                     ((operatorNode*)$3)->outputs=new list<Node*>({$1});
                              } else if($3->type==Sequential){
                                    ((sequentialNode *)$3)->outputs=new list<Node*>({$1});
                              }
                        }
        | tuple assignment.operator exp {
                                    line("Line:%-4d",@$.first_line);
                                    debug("multiple outputs\n");
                                    $$ = new binopNode((expNode*)$1,*($2),(expNode*)$3,@$ ) ;
                                    if ($3->type  == Operator_) {
                                          ((operatorNode*)$3)->outputs= ((tupleNode *)$1)->tupleList;
                                    } else if ($3->type  == CompositeCall) {
                                          ((compositeCallNode*)$3)->outputs= ((tupleNode *)$1)->tupleList;
                                    }
                              }
        | IDENTIFIER '('  ')'                         { $$ = new callNode(*($1),NULL,@$) ; }
        | IDENTIFIER '(' argument.expression.list ')' { $$ = new callNode(*($1),$3,@$) ; }
        | FILEREADER '('  ')' '(' argument.expression.list ')' { 
                  line("Line:%-4d",@$.first_line);
                  debug ("exp ::= FILEREADER()( stringConstant )\n");
                  // list<Node *>* arg_list = new list<Node *>(($5, $7));
                  string compName = "FileReader";
                  $$ = new compositeCallNode(NULL, compName, $5,NULL,NULL,@$);
            }
        | IDENTIFIER '('  ')' operator.selfdefine.body   { 
                  line("Line:%-4d",@$.first_line);
                  debug ("exp ::= %s() operator.selfdefine.body\n",$1->c_str());
                  $$ = new operatorNode(NULL,*($1),NULL,(operBodyNode*)$4,@$) ; 
                  //error("%s",((operatorNode*)$$)->operName.c_str());
            }
        | IDENTIFIER '(' argument.expression.list ')' operator.selfdefine.body   { 
                  $$ = new operatorNode(NULL,*($1),$3,(operBodyNode*)$5,@$) ; 
            }
        | IDENTIFIER '('  ')'  '(' ')'  { 
                  line("Line:%-3d",@$.first_line);
                  debug ("exp ::= %s()()\n",$1->c_str()); 
                  //if(S.LookupCompositeSymbol(*$1)==NULL) error("Line:%s\tthe composite has not been declared!",$1->c_str());
                  //$$ = new compositeCallNode(NULL,*($1),NULL,NULL,S.LookupCompositeSymbol(*($1)),@$) ; 
                  // 建立符号表时 补上真是的composite节点
                  $$ = new compositeCallNode(NULL,*($1),NULL,NULL,NULL,@$) ; 
            }
        | IDENTIFIER '('  ')'  '(' argument.expression.list ')' { 
                  //if(S.LookupCompositeSymbol(*$1)==NULL) error("Line:%s\tthe composite has not been declared!",$1->c_str());
                  $$ = new compositeCallNode(NULL,*($1),$5,NULL,NULL,@$) ; 
                  //$$ = new compositeCallNode(NULL,*($1),$5,NULL,S.LookupCompositeSymbol(*($1)),@$) ; 
            }
        | IDENTIFIER '(' argument.expression.list ')'  '(' ')'  { 
                  //if(S.LookupCompositeSymbol(*$1)==NULL) error("Line:%s\tthe composite has not been declared!",$1->c_str());
                  //$$ = new compositeCallNode(NULL,*($1),NULL,$3,S.LookupCompositeSymbol(*($1)),@$) ; 
                  $$ = new compositeCallNode(NULL,*($1),NULL,$3,NULL,@$) ; 
            }
        | IDENTIFIER '(' argument.expression.list ')'  '(' argument.expression.list ')'    { 
                  //if(S.LookupCompositeSymbol(*$1)==NULL) error("Line:%s\tthe composite has not been declared!",$1->c_str());
                  //$$ = new compositeCallNode(NULL,*($1),$6,$3,S.LookupCompositeSymbol(*($1)),@$) ; 
                  $$ = new compositeCallNode(NULL,*($1),$6,$3,NULL,@$) ;
            }
        |  SPLITJOIN '(' argument.expression.list ')'  lblock split.statement  splitjoinPipeline.statement.list  join.statement rblock { 
            /*    1.argument.expression.list是一个identifier
                  2.查找符号表 identifier是否出现过 */
                  $$ = new splitjoinNode($3,NULL,(splitNode*)$6,NULL,$7,(joinNode*)$8,@$)  ; 

            }
        |  SPLITJOIN '(' argument.expression.list ')'  lblock statement.list split.statement splitjoinPipeline.statement.list  join.statement rblock  { 
                  /*    1.argument.expression.list是一个identifier
                  2.查找符号表 identifier是否出现过 */
                  $$ = new splitjoinNode($3,NULL,(splitNode*)$7,$6,$8,(joinNode*)$9,@$)  ; 
            }
        |   PIPELINE '(' argument.expression.list ')'  lblock splitjoinPipeline.statement.list rblock  {
                   /*    1.argument.expression.list是一个identifier
                  2.查找符号表 identifier是否出现过 */
                  $$ = new pipelineNode(NULL,$6,$3,@$) ; 
            }
        |   SEQUENTIAL '(' argument.expression.list ')'  '(' argument.expression.list ')' lblock sequential.statement.list rblock {
                  /*     1.argument.expression.list是一个identifier
                  2. argument.expression.list*/
                  line("Line%-4d", @$.first_line);
                  debug("exp ::= SEQUENTIAL\n");
                  $$ = new sequentialNode(NULL,$3,$6,$9,@$); 
        }
        ;

operator.selfdefine.body:
       lblock operator.selfdefine.body.init operator.selfdefine.body.work operator.selfdefine.body.window.list rblock
        {
            line("Line:%-4d",@$.first_line);
            debug ("operator.selfdefine.body ::=  { init work window.list }\n");
            $$ = new operBodyNode(NULL,$2,$3,(windowNode*)$4) ;
        }
     | lblock statement.list operator.selfdefine.body.init  operator.selfdefine.body.work operator.selfdefine.body.window.list rblock
        {
            line("Line:%-4d",@$.first_line);
            debug ("operator.selfdefine.body ::=  { statement.list init work window.list }\n");
            $$ = new operBodyNode($2,$3,$4,(windowNode*)$5);
        }
     ;

operator.selfdefine.body.init:
      /*empty*/{ $$ = NULL ; }
    | INIT compound.statement { $$ = $2 ; }
    ;

operator.selfdefine.body.work:
      WORK compound.statement { $$ = $2 ; }
    ;

operator.selfdefine.body.window.list:
      /*empty*/                                         { $$ = NULL ; }
      | WINDOW lblock operator.selfdefine.window.list rblock  {
                                                            line("Line:%-4d",@$.first_line);
                                                            debug ("operator.selfdefine.body.window.list ::= WINDOW { operator.selfdefine.window.list }\n");
                                                            $$ = new windowNode($3) ;
                                                        }
    ;

operator.selfdefine.window.list:
      operator.selfdefine.window                                        {     $$ = new list<Node*>({$1}); }
    | operator.selfdefine.window.list operator.selfdefine.window        {     $$->push_back($2);          }
    ;

operator.selfdefine.window:
      IDENTIFIER window.type ';'                {
                                                    line("Line:%-4d",@$.first_line);
                                                    debug ("operator.selfdefine.window ::= %s window.type (sliding? (arg_list?))\n",$1->c_str());
                                                    $$ = new winStmtNode(*($1),$2,@$) ;
                                                }
    ;

window.type:
      SLIDING '('  ')'                          {     $$ = new slidingNode(NULL,@$) ;  }
    | TUMBLING '('  ')'                         {     $$ = new tumblingNode(NULL,@$) ; }
    | SLIDING '(' argument.expression.list ')'  {     $$ = new slidingNode($3,@$) ;    }
    | TUMBLING '(' argument.expression.list ')' {     $$ = new tumblingNode($3,@$) ;   }
    ;


/*************************************************************************/
/*        5. basic 从词法TOKEN直接归约得到的节点,自底向上接入头部文法结构    */
/*************************************************************************/

lblock: '{' { 
                 // EnterScope(); /* 进入新的变量块级作用域 */ 
                 // saved=top;
                 // top=new SymbolTable(top);

            }  
rblock: '}' {     
                  // ExitScope();  /* 退出一个块级作用域    */ 
                  // top=saved;
            }

constant:
          doubleConstant    { $$ = new constantNode("double",$1,@$) ; }
        | integerConstant   { $$ = new constantNode("long long",$1,@$) ; }
        | stringConstant    { $$ = new constantNode("string",*($1),@$) ; }
        ;
type.specifier:
          basic.type.name       { $$ = $1 ;}
        | CONST basic.type.name {
                                    (static_cast<primNode*>$2)->isConst=true;
                                    $$ = $2 ; 
                                }
        ;
basic.type.name:
          INT         { $$ = new primNode("int",@$ ); }
        | LONG        { $$ = new primNode("long",@$ ); }
        | LONG LONG   { $$ = new primNode("long long",@$ ); }
        | FLOAT       { $$ = new primNode("float",@$ ); }
        | DOUBLE      { $$ = new primNode("double",@$ ); }
        | STRING      { $$ = new primNode("string",@$ ); }
        ;

idNode:
          IDENTIFIER  { 
                        $$ = new idNode($1,@$);
                      }
        | IDENTIFIER  array.declarator { 
                        $$ = new idNode($1,@$);
                        ((idNode*)$$)->isArray = 1;
                        (static_cast<idNode*>$$)->arg_list = (static_cast<arrayNode*>$2)->arg_list;
                      }  
        ; 
%%
/* ----语法树结束----*/
void yyerror (const char *msg)
{
    Error(msg,yylloc.first_line,yylloc.first_column);
}
