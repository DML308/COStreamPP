%{
  //约定的文法, 去掉花括号后的简洁内容放在这里
%}
%%
/************************************************************************/
/*              1. 文法一级入口,由下面三种文法组成                           */
/*                 1.1. declaration 声明                                 */
/*                 1.2. function.definition 函数声明                      */
/*                 1.3. composite.definition 数据流计算单元声明             */
/*************************************************************************/
prog.start: translation.unit ;
translation.unit:
          external.definition   
        | translation.unit external.definition  
        ;
external.definition:
          declaration           
        | function.definition   
        | composite.definition  
        ;
/*************************************************************************/
/*              1.1 declaration 由下面2种文法+2个基础组件组成                */
/*                      1.1.1 declaring.list                             */
/*                      1.1.2 stream.declaring.list                      */
/*                      1.1.3 array                                      */
/*                      1.1.4 initializer                                */
/*************************************************************************/
declaration:
      declaring.list ';'        
    | stream.declaring.list ';' 
    ;
declaring.list:
          type.specifier      idNode     initializer.opt  
        | declaring.list 	',' idNode     initializer.opt
        ;
stream.declaring.list:
          stream.type.specifier IDENTIFIER    
        | stream.declaring.list ',' IDENTIFIER
        ;
stream.type.specifier:
          STREAM '<' stream.declaration.list '>'
        ;
stream.declaration.list:
          type.specifier idNode 
        | stream.declaration.list ',' type.specifier idNode 
        ;
/*************************************************************************/
/*                      1.1.3 array ( int a[] )                          */
/*************************************************************************/
array.declarator:
          '[' ']'   
        | '[' exp ']' 
        | array.declarator '[' exp ']'  
        | array.declarator '[' ']'  
        ;
/*************************************************************************/
/*                      1.1.4 initializer                                */
/*************************************************************************/
initializer.opt:
          /* nothing */         
        | '=' initializer       
        ;
initializer:
          '{' initializer.list '}'      
        | '{' initializer.list ',' '}'  
        | exp                           
        ;
initializer.list:
          initializer   
        | initializer.list ',' initializer  
        ;
/*************************************************************************/
/*              1.2 function.definition 函数声明                          */
/*                      1.2.1 parameter.list                             */
/*                      1.2.1 function.body                       */
/*************************************************************************/
function.definition:
          type.specifier IDENTIFIER '(' ')' function.body 
        | type.specifier IDENTIFIER '(' parameter.list ')' function.body  
        ;
parameter.list:
          parameter.declaration   
        | parameter.list ',' parameter.declaration 
        | parameter.list '=' initializer 
        | parameter.list ',' error
        ;
parameter.declaration:
          type.specifier idNode 
        ;
function.body:
          lblock rblock                   
        | lblock statement.list rblock    
        ;
statement.list:
          statement                   
        | statement.list statement    
        ;
/*************************************************************************/
/*              1.3 composite.definition 数据流计算单元声明                */
/*                      1.3.1 composite.head                             */
/*                      1.3.2 composite.body                             */
/*************************************************************************/
composite.definition:
      composite.head composite.body 
    ;
composite.head:
      COMPOSITE IDENTIFIER '(' composite.head.inout ')'   
    ;
composite.head.inout:
      /*empty*/                                                                           
    | INPUT composite.head.inout.member.list                                              
    | INPUT composite.head.inout.member.list ',' OUTPUT composite.head.inout.member.list  
    | OUTPUT composite.head.inout.member.list                                             
    | OUTPUT composite.head.inout.member.list ',' INPUT composite.head.inout.member.list  
    ;
composite.head.inout.member.list:
      composite.head.inout.member                                                         
    | composite.head.inout.member.list ',' composite.head.inout.member                    
    ;
composite.head.inout.member:
      stream.type.specifier IDENTIFIER                                                    
    ;
/*************************************************************************/
/*                      1.3.2 composite.body                             */
/*                        1.3.2.1 composite.body.param.opt               */
/*                        1.3.2.2 composite.body.declaration.list        */
/*                        1.3.2.3 composite.body.statement.list          */
/*************************************************************************/
composite.body:
          lblock composite.body.param.opt composite.body.statement.list rblock                              
        ;
composite.body.param.opt:
          /*empty*/                 
        | PARAM parameter.list ';'  
        ;
composite.body.statement.list:
          costream.composite.statement                                
        | composite.body.statement.list costream.composite.statement  
        ;
costream.composite.statement:
          composite.body.operator   
        | statement                 
        ;
/*****************************************************************************/
/*        2. composite.body.operator  composite体内的init work window等组件   */
/*             2.1   ADD operator.pipeline                                   */
/*             2.2   ADD operator.splitjoin                                  */
/*             2.3   ADD operator.default.call                               */
/*****************************************************************************/
composite.body.operator:
          operator.file.writer      
        | operator.add              
        ;
operator.file.writer:
          FILEWRITER '(' IDENTIFIER ')' '(' stringConstant ')' ';' 
        | FILEWRITER '(' IDENTIFIER ')' '(' ')' ';' 
        ;
operator.add:
          ADD operator.pipeline     
        | ADD operator.splitjoin    
        | ADD operator.default.call 
        ;
operator.pipeline:
          PIPELINE lblock  splitjoinPipeline.statement.list rblock     
        ;
splitjoinPipeline.statement.list:
          statement                                       
        | operator.add                                    
        | splitjoinPipeline.statement.list statement      
        | splitjoinPipeline.statement.list operator.add   
        ;
operator.splitjoin:
          SPLITJOIN lblock split.statement  splitjoinPipeline.statement.list  join.statement rblock     
        | SPLITJOIN lblock statement.list split.statement splitjoinPipeline.statement.list join.statement rblock  
        ;
split.statement:
          SPLIT duplicate.statement                        
        | SPLIT roundrobin.statement                       
        ;
roundrobin.statement:
          ROUNDROBIN '(' ')' ';'                            
        | ROUNDROBIN '(' argument.expression.list ')' ';'   
        ;
duplicate.statement:
          DUPLICATE '('  ')' ';'                            
        | DUPLICATE '(' exp ')'  ';'                        
        ;
join.statement:
          JOIN roundrobin.statement                         
        ;
argument.expression.list:
          exp                                               
        | argument.expression.list ',' exp                  
        ;
operator.default.call:
          IDENTIFIER  '(' ')' ';'                           
        | IDENTIFIER  '(' argument.expression.list ')' ';'  
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
        | ';'       
        | error ';' 
        ;
labeled.statement:
          CASE exp ':' statement                    
        | DEFAULT ':' statement                     
        ;
compound.statement:
          lblock rblock                                     
        | lblock composite.body.statement.list rblock       
        ;
expression.statement:
          exp ';'  
        ;
selection.statement:
          IF '(' exp ')' costream.composite.statement   
        | IF '(' exp ')' costream.composite.statement 
          ELSE costream.composite.statement             
        | SWITCH '(' exp ')' statement                  
        ;
iteration.statement:
          WHILE '(' exp ')' costream.composite.statement                          
        | DO  costream.composite.statement WHILE '(' exp ')' ';'                  
        | FOR '(' exp   ';' exp ';' exp ')'  costream.composite.statement     
        | FOR '(' declaration  ';' exp ';' exp ')'  costream.composite.statement  
        | FOR '(' error ')' costream.composite.statement                          
        ;
jump.statement:
          CONTINUE ';'        
        | BREAK ';'           
        | RETURN exp ';'      
        ;
/*************************************************************************/
/*        4. exp 计算表达式头节点                        */
/*************************************************************************/
assignment.operator:
          '='             
        | MULTassign      
        | DIVassign       
        | MODassign       
        | PLUSassign      
        | MINUSassign     
        | LSassign        
        | RSassign        
        | ANDassign       
        | ERassign        
        | ORassign        
        ;

exp:      idNode        
        | constant       
        | idNode '.' idNode 
        | constant        
        | exp '+' exp   
        | exp '-' exp   
        | exp '*' exp   
        | exp '/' exp   
        | exp '%' exp   
        | exp OROR exp  
        | exp ANDAND exp
        | exp '|' exp   
        | exp '&' exp   
        | exp '^' exp   
        | exp LS exp    
        | exp RS exp    
        | exp '<' exp   
        | exp '>' exp   
        | exp LE exp    
        | exp GE exp    
        | exp EQ exp    
        | exp NE exp    
        | exp '?' exp ':' exp 
        | '+' exp       
        | '-' exp       
        | '~' exp       
        | '!' exp       
        |  ICR exp      
        |  DECR exp     
        |  exp ICR      
        |  exp DECR     
        |  '(' exp ')'  
        | '(' basic.type.name ')' exp                         
        | exp assignment.operator exp                         
        | IDENTIFIER '(' argument.expression.list ')'         
        | FILEREADER '(' ')' '(' stringConstant ')'           
        | IDENTIFIER '('  ')' operator.selfdefine.body        
        | IDENTIFIER '(' argument.expression.list ')' operator.selfdefine.body   
        | IDENTIFIER '('  ')'  '(' ')'                        
        | IDENTIFIER '('  ')'  '(' argument.expression.list ')'                            
        | IDENTIFIER '(' argument.expression.list ')'  '(' ')'                             
        | IDENTIFIER '(' argument.expression.list ')'  '(' argument.expression.list ')'    
        |  SPLITJOIN '(' argument.expression.list ')'  lblock statement.list split.statement splitjoinPipeline.statement.list  join.statement rblock  
        ;
operator.selfdefine.body:
       lblock operator.selfdefine.body.init operator.selfdefine.body.work operator.selfdefine.body.window.list rblock
     | lblock statement.list operator.selfdefine.body.init  operator.selfdefine.body.work operator.selfdefine.body.window.list rblock
     ;
operator.selfdefine.body.init:
      /*empty*/
    | INIT compound.statement 
    ;
operator.selfdefine.body.work:
      WORK compound.statement 
    ;
operator.selfdefine.body.window.list:
      /*empty*/                                         
    | WINDOW lblock operator.selfdefine.window.list rblock  
    ;
operator.selfdefine.window.list:
      operator.selfdefine.window                
    | operator.selfdefine.window.list operator.selfdefine.window
    ;
operator.selfdefine.window:
      IDENTIFIER window.type ';'                
    ;
window.type:
      SLIDING '('  ')'                          
    | TUMBLING '('  ')'                         
    | SLIDING '(' argument.expression.list ')'  
    | TUMBLING '(' argument.expression.list ')' 
    ;
/*************************************************************************/
/*        5. basic 从词法TOKEN直接归约得到的节点,自底向上接入头部文法结构    */
/*************************************************************************/
/* 设置变量作用域相关 */
lblock: '{'  { EnterScope();  }  
rblock: '}'  { ExitScope();   }
constant:
          doubleConstant    
        | integerConstant   
        | stringConstant    
        ;
type.specifier:
          basic.type.name       
        | CONST basic.type.name 
        ;
basic.type.name:
          INT   
        | LONG  
        | LONG LONG 
        | FLOAT 
        | DOUBLE
        | STRING
        ;
idNode:
          IDENTIFIER
        | IDENTIFIER  array.declarator 
        ; 
%%
/* ----语法树结束----*/
