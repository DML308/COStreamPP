%{
    #define DEBUG
    #include "ast.h"
    #include "print-ast.h"
    /* 原 ANSI-C.y 中不明所以的 define,对应书中写到,yydebug控制是否产生调试报告(0值不产生),YYERROR_VERBOSE 没找到描述*/
    #ifndef YYDEBUG
    int yydebug=0;
    #endif
    #define YYERROR_VERBOSE 1

    /*
    PRIVATE void WarnAboutPrecedence(OpType op, Node *node);{
      if (node->typ == Binop && !node->parenthesized) {
        OpType subop = node->u.binop.op;

        if (op == OROR && subop == ANDAND)
          WarningCoord(4, node->coord, "suggest parentheses around && in operand of ||");
        else if ((op == '|' || op == '^') &&
             (subop == '+' || subop == '-' || subop == '&' || subop == '^') &&
             op != subop)
          WarningCoord(4, node->coord, "suggest parentheses around arithmetic in operand of %c", op);
      }
    }*/
    
   
    /* bison 进行语法分析时遇到错误默认调用的错误报告函数 */
    void yyerror(const char *msg){
        SyntaxError(msg);
    }
%}

/*在 union 里声明 %token 可能有的类型*/
%union{
    Node *n;
    List *L;
    /*
    struct {
        TypeQual   tq;
        Coord      coord;   
    } tq;*/
    Coord tok; //token coordinates
}

%token <n> IDENTIFIER
// 常量
%token <n> FLOATINGconstant INTEGERconstant OCTALconstant HEXconstant WIDECHARconstant CHARACTERconstant
// 保留字符串 /* ANSI keywords, extensions below */
%token <tok> AUTO            DOUBLE          INT             STRUCT
%token <tok> BREAK           ELSE            LONG            SWITCH
%token <tok> CASE            ENUM            REGISTER        TYPEDEF
%token <tok> CHAR            EXTERN          RETURN          UNION
%token <tok> CONST           FLOAT           SHORT           UNSIGNED
%token <tok> CONTINUE        FOR             SIGNED          VOID
%token <tok> DEFAULT         GOTO            SIZEOF          VOLATILE
%token <tok> DO              IF              STATIC          WHILE

/*-----------------Define For SPL----------------*/
%token  COMPOSITE		 INPUT			 OUTPUT			 STREAM
%token  PARAM			 ADD
%token  INIT			 WORK			 WINDOW
%token  TUMBLING		 SLIDING

%token  SPLITJOIN        SPLIT				JOIN
%token  DUPLICATE        ROUNDROBIN		PIPELINE
%token  FILEREADER       FILEWRITER

/* Multi-Character operators */
%token <tok>  ARROW            /*    ->                              */
%token <tok>  ICR DECR         /*    ++      --                      */
%token <tok>  LS RS            /*    <<      >>                      */
%token <tok>  LE GE EQ NE      /*    <=      >=      ==      !=      */
%token <tok>  ANDAND OROR      /*    &&      ||                      */
%token <tok>  ELLIPSIS         /*    ...                             */

/* modifying assignment operators */
%token <tok> '='
%token <tok> MULTassign  DIVassign    MODassign   /*   *=      /=      %=      */
%token <tok> PLUSassign  MINUSassign              /*   +=      -=              */
%token <tok> LSassign    RSassign                 /*   <<=     >>=             */
%token <tok> ANDassign   ERassign     ORassign    /*   &=      ^=      |=      */


/***************   复杂文法结构   **********************************/
// %type <L> translation.unit external.definition
// %type <n> function.definition
// /****************************Define For SPL************************************/
// /*for composite*/
// %type <n> composite.definition
// %type <n> composite.head composite.body.no.new.scope
// %type <n> composite.head.inout composite.head.inout.member
// %type <L> composite.head.inout.member.list
// /*for composite.body */
// %type <n> composite.body.param.opt
// %type <L> composite.body.statement.list
// /*for SPL compositestmt*/
// %type <n> composite.body.operator
// %type <n> operator.splitjoin operator.pipeline operator.add
// /*for streamit filter*/
// %type <n> operator.default.call
// %type <n> split.statement join.statement
// %type <n> duplicate.statement roundrobin.statement
// /*for files filter*/
// %type <n>  operator.file.writer
// /*for operator body*/
// %type <n> operator.selfdefine.body.init.opt operator.selfdefine.body.work  operator.selfdefine.body.window.list.opt
// %type <L> operator.selfdefine.window.list
// %type <n> operator.selfdefine.window  window.type
// /*for splitjoin and pipeline statement*/
// %type <L> splitjoinPipeline.statement.list
// /****************************Define For SPL************************************/

// %type <n> constant string.literal.list
 %type <n> primary.expression postfix.expression unary.expression
// %type <n> cast.expression multiplicative.expression additive.expression
// %type <n> shift.expression relational.expression equality.expression
// %type <n> AND.expression exclusive.OR.expression inclusive.OR.expression
// %type <n> logical.AND.expression logical.OR.expression conditional.expression
// %type <n> assignment.expression constant.expression expression.opt
// %type <L> attributes.opt attributes attribute attribute.list
// %type <n> attrib any.word

// %type <n> initializer.opt initializer initializer.list
// %type <n> bit.field.size.opt bit.field.size enumerator.value.opt

// %type <n> costream.composite.statement statement labeled.statement expression.statement
// %type <n> selection.statement iteration.statement jump.statement
// %type <n> compound.statement compound.statement.no.new.scope

// %type <n> basic.declaration.specifier basic.type.specifier
// %type <n> type.name expression type.specifier declaration.specifier
// %type <n> typedef.declaration.specifier typedef.type.specifier
// %type <n> abstract.declarator unary.abstract.declarator
// %type <n> postfixing.abstract.declarator array.abstract.declarator
// %type <n> postfix.abstract.declarator old.function.declarator
// %type <n> struct.or.union.specifier struct.or.union elaborated.type.name
// %type <n> enum.specifier

// /**********Define For SPL ********/
// %type <n> stream.type.specifier
// %type <L> stream.declaration.list
// /**********Define For SPL ********/

// %type <n> parameter.declaration
// %type <n> identifier.declarator parameter.typedef.declarator
// %type <n> declarator paren.typedef.declarator
// %type <n> clean.typedef.declarator simple.paren.typedef.declarator
// %type <n> unary.identifier.declarator paren.identifier.declarator
// %type <n> postfix.identifier.declarator clean.postfix.typedef.declarator
// %type <n> paren.postfix.typedef.declarator postfix.old.function.declarator
// %type <n> struct.identifier.declarator struct.declarator

// %type <L> declaration declaration.list declaring.list default.declaring.list
// %type <L> argument.expression.list identifier.list statement.list
// %type <L> parameter.type.list parameter.list
// %type <L> struct.declaration.list struct.declaration struct.declaring.list
// %type <L> struct.default.declaring.list enumerator.list
// %type <L> old.function.declaration.list

// %type <n> unary.operator assignment.operator
// %type <n> identifier.or.typedef.name

// %type <tq> type.qualifier type.qualifier.list declaration.qualifier.list
// %type <tq> declaration.qualifier storage.class
// %type <tq> pointer.type.qualifier pointer.type.qualifier.list
// %type <L> operator.arguments
// %type <n> operator.selfdefine.body
%type <tok>  basic.type.name

/*优先级标记*/
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
%start prog.start
%locations

%% 
/********************************************************************************
*                            顶级    节点                                        *
********************************************************************************/
prog.start: translation.unit ;

translation.unit:              
          external.definition 
          {
              line("Line:%-3d",1);
              debug("translation.unit :== external.definition\n");
          }
        | translation.unit external.definition
          {
              line("Line:%-3d",1);
              debug("translation.unit :== translation.unit external.definition\n");
          }
        ;
external.definition:           
          declaration
          {
              line("Line:%-3d",@1.first_line);
              debug("external.definition :== declaration\n");
          }
        //| function.definition
	    //| composite.definition
	    ;
/********************************************************************************
*                            一级    节点                                        *
********************************************************************************/
declaration:
          declaring.list ';'
          {
              line("Line:%-3d",@1.first_line);
              debug("declaration :== declaring.list ';'\n");
          }
        //| default.declaring.list ';'
        ;

/********************************************************************************
*                            二级    节点                                        *
********************************************************************************/
declaring.list: 
          declaration.specifier 	declarator attributes.opt  initializer.opt
          {
              line("Line:%-3d",@1.first_line);
              debug("declaring.list :== declaration.specifier 	declarator attributes.opt  initializer.opt \n");
          }
		| type.specifier 	declarator attributes.opt initializer.opt  
          {
              line("Line:%-3d",@1.first_line);
              debug("declaring.list :== type.specifier 	declarator attributes.opt initializer.opt \n");
          }
		| declaring.list 	',' 	declarator  attributes.opt initializer.opt
          {
              line("Line:%-3d",@1.first_line);
              debug("declaring.list ',' declarator  attributes.opt initializer.opt\n");
          }
        //| declaration.specifier 	error attributes.opt initializer.opt  
		//| type.specifier 			error attributes.opt initializer.opt  
		//| declaring.list 	',' 	error
        ;
        
/********************************************************************************
*                            三级    节点                                        *
********************************************************************************/

/********************************************************************************
*                        左半边声明部分 - 需要大量删减, 预计只保留 int 和 const double *
********************************************************************************/
declaration.specifier:
          basic.declaration.specifier
          // 或 typedef 和 sue 这种停止支持的
        ; 
basic.declaration.specifier:
          basic.type.specifier storage.class
          {
              line("Line:%-3d",@1.first_line);
              debug("basic.declaration.specifier :== basic.type.specifier storage.class \n");
          }
        | basic.declaration.specifier basic.type.name
          {
              line("Line:%-3d",@1.first_line);
              debug("basic.declaration.specifier :== basic.declaration.specifier basic.type.name \n");
          }
        ;
type.specifier:
          basic.type.specifier
        ;
basic.type.specifier:
          basic.type.name
        ;
basic.type.name:
          INT
        | DOUBLE
        ;
declarator:
          identifier.declarator 
        ;
identifier.declarator:
          paren.identifier.declarator 
        ;
paren.identifier.declarator:
          IDENTIFIER 
        ;
/********************************************************************************
*                            右半边表达式部分 : 条件表达式 | 计算表达式                 *
********************************************************************************/
attributes.opt:
          /* empty */
        ;
initializer.opt:
          /* empty */
        | '=' initializer
        ;
initializer:
          assignment.expression
        ;
assignment.expression:
          exp
          {
            line("Line:%-3d",@1.first_line);
            debug ("assignment.expression ::= exp\n");
          }
        
        ;
exp:    IDENTIFIER      { line("Line:%-3d",@1.first_line);debug ("exp ::= IDENTIFIER\n");}
        | constant      { line("Line:%-3d",@1.first_line);debug ("exp ::= constant\n");}
        | exp '+' exp   { line("Line:%-3d",@1.first_line);debug ("exp ::= exp + exp\n");}
        | exp '-' exp   { line("Line:%-3d",@1.first_line);debug ("exp ::= exp - exp\n");}
        | exp '*' exp   { line("Line:%-3d",@1.first_line);debug ("exp ::= exp * exp\n");}
        | exp '/' exp   { line("Line:%-3d",@1.first_line);debug ("exp ::= exp / exp\n");}
        | exp '%' exp   { line("Line:%-3d",@1.first_line);debug ("exp ::= exp %% exp\n");}
        | exp OROR exp   { line("Line:%-3d",@1.first_line);debug ("exp ::= exp || exp\n");}
        | exp ANDAND exp   { line("Line:%-3d",@1.first_line);debug ("exp ::= exp && exp\n");}
        | exp '|' exp   { line("Line:%-3d",@1.first_line);debug ("exp ::= exp | exp\n");}
        | exp '&' exp   { line("Line:%-3d",@1.first_line);debug ("exp ::= exp & exp\n");}
        | exp '^' exp   { line("Line:%-3d",@1.first_line);debug ("exp ::= exp ^ exp\n");}
        | exp LS exp   { line("Line:%-3d",@1.first_line);debug ("exp ::= exp << exp\n");}
        | exp RS exp   { line("Line:%-3d",@1.first_line);debug ("exp ::= exp >> exp\n");}
        | exp '<' exp   { line("Line:%-3d",@1.first_line);debug ("exp ::= exp < exp\n");}
        | exp '>' exp   { line("Line:%-3d",@1.first_line);debug ("exp ::= exp > exp\n");}
        | exp LE exp   { line("Line:%-3d",@1.first_line);debug ("exp ::= exp <= exp\n");}
        | exp GE exp   { line("Line:%-3d",@1.first_line);debug ("exp ::= exp >= exp\n");}
        | exp EQ exp   { line("Line:%-3d",@1.first_line);debug ("exp ::= exp == exp\n");}
        | exp NE exp   { line("Line:%-3d",@1.first_line);debug ("exp ::= exp != exp\n");}
        ;
/********************************************************************************
*                            计算表达式 - unary表达式                                *
********************************************************************************/
unary.expression:               
          postfix.expression
        ;
postfix.expression: 
          primary.expression
        ;
primary.expression:            
         IDENTIFIER          
        | constant
        ;
assignment.operator: //全都是类似于 = 或 += 的赋值操作符
          '='             
          {
            line("Line:%-3d",@1.first_line);
            debug ("assignment.operator ::= =\n");
          }
        | MULTassign     
          {
            line("Line:%-3d",@1.first_line);
            debug ("assignment.operator ::= +=\n");
          }
        ;
/********************************************************************************
*                            基础    节点                                        *
********************************************************************************/
constant: FLOATINGconstant      { PrintNode(stdout,$1,3); }
        | INTEGERconstant       { PrintNode(stdout,$1,3); }
        | OCTALconstant         { PrintNode(stdout,$1,3); }
        | HEXconstant           { PrintNode(stdout,$1,3); }
        ;
storage.class:
          STATIC {}
        ;



%%
/* ----语法树结束----*/

