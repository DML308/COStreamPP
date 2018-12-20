%{
  //老文法, 方便做对比.
%}
%%
prog.start: 
          translation.unit 
		  ;
stream.type.specifier:
		  STREAM '<' stream.declaration.list '>'
		;
stream.declaration.list:
		    type.specifier paren.identifier.declarator 
		|   type.specifier paren.identifier.declarator postfixing.abstract.declarator
		| stream.declaration.list ',' type.specifier paren.identifier.declarator
		| stream.declaration.list ',' type.specifier paren.identifier.declarator postfixing.abstract.declarator
		;
composite.definition:						
		  composite.head 
		  composite.body.no.new.scope
		;
composite.head:								
		  COMPOSITE IDENTIFIER '(' composite.head.inout ')'  
		;
composite.head.inout:						
		| INPUT composite.head.inout.member.list ',' OUTPUT composite.head.inout.member.list
		| INPUT composite.head.inout.member.list
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
composite.body.no.new.scope:								
			 '{' composite.body.param.opt composite.body.statement.list '}'
		|  '{' composite.body.param.opt declaration.list composite.body.statement.list '}'
		;
composite.body.param.opt:					
    | PARAM parameter.list ';'
		;
composite.body.statement.list:
		 costream.composite.statement
		| composite.body.statement.list costream.composite.statement
		;
composite.body.operator:    
			operator.add
		| operator.file.writer
		;
operator.file.writer:
		  FILEWRITER '(' IDENTIFIER ')' operator.arguments ';'
		;
operator.add:
		  ADD operator.default.call	//add composite-call(real-param[i]);
		| ADD operator.pipeline
		| ADD operator.splitjoin
		;
operator.pipeline:
		  PIPELINE lblock splitjoinPipeline.statement.list rblock      //add 方式
		| PIPELINE lblock  declaration.list splitjoinPipeline.statement.list rblock      //add 方式
		;  
// 花括号中的完整结构为： 声明语句(可选) 初始化语句（可选） split语句 splitjoinPipeline语句 join语句
operator.splitjoin:
		  SPLITJOIN lblock split.statement  splitjoinPipeline.statement.list  join.statement rblock  //add 方式 add splitjoin
		| SPLITJOIN lblock declaration.list split.statement splitjoinPipeline.statement.list join.statement rblock  //add 方式 add splitjoin
	  | SPLITJOIN lblock declaration.list statement.list split.statement splitjoinPipeline.statement.list join.statement rblock  //add 方式 add splitjoin
		;
split.statement:
		  SPLIT duplicate.statement 
		| SPLIT roundrobin.statement
		;
splitjoinPipeline.statement.list:
		  statement  
		| operator.add
		| splitjoinPipeline.statement.list statement
		| splitjoinPipeline.statement.list operator.add
		;
join.statement:
		  JOIN roundrobin.statement
		;
roundrobin.statement:
		  ROUNDROBIN '(' ')' ';'
		| ROUNDROBIN '(' argument.expression.list ')' ';'
		;
duplicate.statement:
		  DUPLICATE '('  ')' ';'
		| DUPLICATE '(' assignment.expression ')'  ';'
		;
operator.default.call:
		  IDENTIFIER  operator.arguments ';' 
		;
operator.arguments:
		  '(' argument.expression.list ')'
		| '(' ')'
		;
operator.selfdefine.body:		  
		   lblock operator.selfdefine.body.init.opt  operator.selfdefine.body.work operator.selfdefine.body.window.list.opt rblock            
		 | lblock declaration.list operator.selfdefine.body.init.opt  operator.selfdefine.body.work operator.selfdefine.body.window.list.opt rblock          
		 ;
operator.selfdefine.body.init.opt:
		| INIT compound.statement
		;
operator.selfdefine.body.work:
		  WORK compound.statement
		;
operator.selfdefine.body.window.list.opt:					 
		  | WINDOW ''
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
		| SLIDING '(' expression ')'  
		| TUMBLING '(' expression ')'                  
		| UNCERTAINTY '(' ')'
		;
/********************************************************************************
*										*
*                                EXPRESSIONS                                    *
*										*
********************************************************************************/
primary.expression:               
         IDENTIFIER           
        | constant
        | string.literal.list
        | '(' expression ')'    
        | '(' lblock statement.list rblock ')'
        | '(' lblock declaration.list statement.list rblock ')'
        ;
postfix.expression:             
          primary.expression
        | postfix.expression '[' expression ']'
        | postfix.expression '(' ')'
        | postfix.expression '(' argument.expression.list ')'  //函数调用形式！
//++++++++sql 新文法+++++++++++++++++++**********************************************************************************
		| postfix.expression '('  ')' operator.selfdefine.body //内置operator！
		| postfix.expression '(' argument.expression.list ')' operator.selfdefine.body //内置operator！
		| postfix.expression '('  ')' operator.arguments 
		| postfix.expression '(' argument.expression.list ')' operator.arguments 
		| SPLITJOIN '(' IDENTIFIER ')'  lblock split.statement  splitjoinPipeline.statement.list  join.statement rblock
		| SPLITJOIN '(' IDENTIFIER ')'  lblock  declaration.list split.statement  splitjoinPipeline.statement.list  join.statement rblock 
		| SPLITJOIN '(' IDENTIFIER ')'  lblock declaration.list statement.list split.statement splitjoinPipeline.statement.list  join.statement rblock 
		|  PIPELINE '(' IDENTIFIER ')' lblock  splitjoinPipeline.statement.list rblock //pipeline为单输入单输出结构
		|  PIPELINE '(' IDENTIFIER ')' lblock declaration.list splitjoinPipeline.statement.list rblock //pipeline为单输入单输出结构
		|  FILEREADER '(' ')' operator.arguments
//+++++++++++++++++++++++++++*************************************************************************
        | postfix.expression '.' IDENTIFIER
        | postfix.expression ARROW IDENTIFIER
        | postfix.expression ICR
        | postfix.expression DECR
        | postfix.expression '.'   TYPEDEFname
        | postfix.expression ARROW TYPEDEFname
        ;
argument.expression.list:       
          assignment.expression
        | argument.expression.list ',' assignment.expression
        ;
unary.expression:               
          postfix.expression
        | ICR unary.expression
        | DECR unary.expression
        | unary.operator cast.expression
        | SIZEOF unary.expression
        | SIZEOF '(' type.name ')'
        ;
unary.operator:                 
          '&'     
        | '*'     
        | '+'     
        | '-'     
        | '~'     
        | '!'     
        ;
cast.expression:                
          unary.expression
        | '(' type.name ')' cast.expression  
        ;
multiplicative.expression:      
          cast.expression
        | multiplicative.expression '*' cast.expression
        | multiplicative.expression '/' cast.expression
        | multiplicative.expression '%' cast.expression
        ;
additive.expression:            
          multiplicative.expression
        | additive.expression '+' multiplicative.expression
        | additive.expression '-' multiplicative.expression
        ;
shift.expression:               
          additive.expression
        | shift.expression LS additive.expression
        | shift.expression RS additive.expression
        ;
relational.expression:          
          shift.expression
        | relational.expression '<' shift.expression
        | relational.expression '>' shift.expression
        | relational.expression LE shift.expression
        | relational.expression GE shift.expression
        ;
equality.expression:            
          relational.expression
        | equality.expression EQ relational.expression
        | equality.expression NE relational.expression
        ;
AND.expression:                 
          equality.expression
        | AND.expression '&' equality.expression
        ;
exclusive.OR.expression:        
          AND.expression
        | exclusive.OR.expression '^' AND.expression
        ;
inclusive.OR.expression:        
          exclusive.OR.expression
        | inclusive.OR.expression '|' exclusive.OR.expression
        ;
logical.AND.expression:         
          inclusive.OR.expression
        | logical.AND.expression ANDAND inclusive.OR.expression
        ;
logical.OR.expression:          
          logical.AND.expression
        | logical.OR.expression OROR logical.AND.expression
        ;
conditional.expression:         
          logical.OR.expression
        | logical.OR.expression '?' expression ':' conditional.expression
        ;
assignment.expression:          
          conditional.expression
        | unary.expression assignment.operator assignment.expression
        ;
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
expression:                     
          assignment.expression
        | expression ',' assignment.expression
        ;
constant.expression:            
          conditional.expression 
        ;
expression.opt:                 
        | expression      
        ;
/********************************************************************************
*										*
*                               DECLARATIONS					*
*										*
*    The following is different from the ANSI C specified grammar.  The changes *
* were made to disambiguate typedef's presence in declaration.specifiers        *
* (vs. in the declarator for redefinition) to allow struct/union/enum tag       *
* declarations without declarators, and to better reflect the parsing of        *
* declarations (declarators must be combined with declaration.specifiers ASAP   *
* so that they are visible in scope).					        *
*										*
* Example of typedef use as either a declaration.specifier or a declarator:	*
*										*
*   typedef int T;								*
*   struct S ; / * redefinition of T as member name * /			*
*										*
* Example of legal and illegal statements detected by this grammar:		*
*										*
*   int;              / * syntax error: vacuous declaration      * /		*
*   struct S;         / * no error: tag is defined or elaborated * /		*
*										*
* Example of result of proper declaration binding:				*
*										*
*   /* Declare "a" with a type in the name space BEFORE parsing initializer * / *
*   int a = sizeof(a);								*
*										*
*   /* Declare "b" with a type before parsing "c" * /				*
*   int b, c[sizeof(b)];							*
*										*
********************************************************************************/
declaration: 
          declaring.list ';'
        | default.declaring.list ';'
        | sue.declaration.specifier ';'
        | sue.type.specifier ';'
        ;
declaring.list: 
          declaration.specifier declarator
          attributes.opt 
          initializer.opt
		| type.specifier declarator                    
          attributes.opt 
          initializer.opt
        | declaring.list ',' declarator                 
          attributes.opt 
          initializer.opt
        | 
          declaration.specifier error
          attributes.opt
          initializer.opt
        | 
          type.specifier error
          attributes.opt
          initializer.opt
        | declaring.list ',' error
        ;
default.declaring.list:  
          declaration.qualifier.list identifier.declarator
          attributes.opt 
	  initializer.opt
	  | stream.type.specifier identifier.declarator
        | type.qualifier.list identifier.declarator
          attributes.opt 
	  initializer.opt
        | default.declaring.list ',' identifier.declarator
          attributes.opt 
	  initializer.opt
        | 
          declaration.qualifier.list error
          attributes.opt
          initializer.opt
        | 
          type.qualifier.list error
          attributes.opt
          initializer.opt
        | default.declaring.list ',' error
        ;
declaration.specifier: 
          basic.declaration.specifier        
        | sue.declaration.specifier          
        | typedef.declaration.specifier      
        ;
basic.declaration.specifier:  
          basic.type.specifier storage.class
        | declaration.qualifier.list basic.type.name
        | basic.declaration.specifier declaration.qualifier
        | basic.declaration.specifier basic.type.name
        ;
sue.declaration.specifier:    
          sue.type.specifier storage.class
        | declaration.qualifier.list elaborated.type.name
        | sue.declaration.specifier declaration.qualifier
        ;
typedef.declaration.specifier:        
          typedef.type.specifier storage.class
        | declaration.qualifier.list TYPEDEFname
        | typedef.declaration.specifier declaration.qualifier
        ;
declaration.qualifier.list:  
          storage.class
        | type.qualifier.list storage.class
        | declaration.qualifier.list declaration.qualifier
        ;
declaration.qualifier: 
          type.qualifier
        | storage.class
        ;
type.specifier: 
          basic.type.specifier               
        | sue.type.specifier                 
        | typedef.type.specifier             
        ;
basic.type.specifier: 
          basic.type.name            
        | type.qualifier.list basic.type.name
        | basic.type.specifier type.qualifier
        | basic.type.specifier basic.type.name
        ;
sue.type.specifier: 
          elaborated.type.name              
        | type.qualifier.list elaborated.type.name
        | sue.type.specifier type.qualifier
        ;
typedef.type.specifier:               
          TYPEDEFname
        | type.qualifier.list TYPEDEFname
        | typedef.type.specifier type.qualifier
        ;
type.qualifier.list: 
          type.qualifier
        | type.qualifier.list type.qualifier
        ;
pointer.type.qualifier.list:
          pointer.type.qualifier
        | pointer.type.qualifier.list pointer.type.qualifier
        ;
elaborated.type.name: 
          struct.or.union.specifier
        | enum.specifier
        ;
declarator: 
          paren.typedef.declarator       
        | parameter.typedef.declarator   
        | identifier.declarator
        | old.function.declarator
        ;
paren.typedef.declarator: 
          paren.postfix.typedef.declarator
        | '*' paren.typedef.declarator
        | '*' '(' simple.paren.typedef.declarator ')' 
        | '*' pointer.type.qualifier.list '(' simple.paren.typedef.declarator ')' 
        | '*' pointer.type.qualifier.list paren.typedef.declarator
        ;
paren.postfix.typedef.declarator:  
          '(' paren.typedef.declarator ')'
        | '(' simple.paren.typedef.declarator postfixing.abstract.declarator ')'
        | '(' paren.typedef.declarator ')' postfixing.abstract.declarator
        ;
simple.paren.typedef.declarator: 
          TYPEDEFname
        | '(' simple.paren.typedef.declarator ')'
        ;
parameter.typedef.declarator: 
          TYPEDEFname 
        | TYPEDEFname postfixing.abstract.declarator
        | clean.typedef.declarator
        ;
/*
   The  following have at least one '*'. There is no (redundant) 
   '(' between the '*' and the TYPEDEFname. 
*/
clean.typedef.declarator: 
          clean.postfix.typedef.declarator
        | '*' parameter.typedef.declarator
        | '*' pointer.type.qualifier.list parameter.typedef.declarator  
        ;
clean.postfix.typedef.declarator: 
          '(' clean.typedef.declarator ')'
        | '(' clean.typedef.declarator ')' postfixing.abstract.declarator
        ;
abstract.declarator: 
          unary.abstract.declarator
        | postfix.abstract.declarator
        | postfixing.abstract.declarator
        ;
unary.abstract.declarator: 
          '*' 
        | '*' pointer.type.qualifier.list 
        | '*' abstract.declarator
        | '*' pointer.type.qualifier.list abstract.declarator
        ;
postfix.abstract.declarator: 
          '(' unary.abstract.declarator ')'
        | '(' postfix.abstract.declarator ')'
        | '(' postfixing.abstract.declarator ')'
        | '(' unary.abstract.declarator ')' postfixing.abstract.declarator
        ;
postfixing.abstract.declarator: 
          array.abstract.declarator     
        | '(' ')'                       
        | '(' parameter.type.list ')'   
        ;
identifier.declarator: 
          unary.identifier.declarator
        | paren.identifier.declarator
        ;
unary.identifier.declarator: 
          postfix.identifier.declarator
        | '*' identifier.declarator
        | '*' pointer.type.qualifier.list identifier.declarator
        ;
postfix.identifier.declarator: 
          paren.identifier.declarator postfixing.abstract.declarator
        | '(' unary.identifier.declarator ')'
        | '(' unary.identifier.declarator ')' postfixing.abstract.declarator
        ;
paren.identifier.declarator: 
          IDENTIFIER
        | '(' paren.identifier.declarator ')'
        ;
old.function.declarator: 
          postfix.old.function.declarator
        | '*' old.function.declarator
        | '*' pointer.type.qualifier.list old.function.declarator
        ;
postfix.old.function.declarator: 
          paren.identifier.declarator '(' identifier.list ')'  
        | '(' old.function.declarator ')'
        | '(' old.function.declarator ')' postfixing.abstract.declarator
        ;
/* 
    ANSI C section 3.7.1 states  
      "An identifier declared as a typedef name shall not be redeclared 
       as a parameter".  
    Hence the following is based only on IDENTIFIERs 
*/
identifier.list: 
          IDENTIFIER
        | identifier.list ',' IDENTIFIER
        ;
identifier.or.typedef.name: 
          IDENTIFIER
        | TYPEDEFname
        ;
type.name: 
          type.specifier
        | type.specifier abstract.declarator
        | type.qualifier.list 
        | type.qualifier.list  abstract.declarator
        ;
attributes.opt:
	| attributes
	;
attributes:
      attribute
	| attributes attribute
	;
attribute:
      ATTRIBUTE '(' '(' attribute.list ')' ')'
	;
attribute.list:
      attrib
	| attribute.list ',' attrib
	;
attrib:
	| any.word
	| any.word '(' expression ')'
	;
any.word:
	  IDENTIFIER
	| TYPEDEFname
	| CONST 
	;
initializer.opt: 
        | '=' initializer                
        ;
initializer: 
          ''       
        | ''   
        | assignment.expression          		
        ;
initializer.list: 
          initializer
        | initializer.list ',' initializer
        ;
parameter.type.list: 
          parameter.list
        | parameter.list ',' ELLIPSIS   
        | ELLIPSIS
        ;
parameter.list: 
          parameter.declaration
        | parameter.list ',' parameter.declaration
        | parameter.declaration '=' initializer
        | parameter.list ',' error
        ;
parameter.declaration: 
          declaration.specifier
        | declaration.specifier abstract.declarator
        | declaration.specifier identifier.declarator
        | declaration.specifier parameter.typedef.declarator
        | declaration.qualifier.list  
        | declaration.qualifier.list  abstract.declarator
        | declaration.qualifier.list  identifier.declarator
        | type.specifier
        | type.specifier abstract.declarator
        | type.specifier identifier.declarator
        | type.specifier parameter.typedef.declarator
        | type.qualifier.list  
        | type.qualifier.list  abstract.declarator
        | type.qualifier.list  identifier.declarator
        ;
array.abstract.declarator: 
          '[' ']'
        | '[' constant.expression ']'
        | array.abstract.declarator '[' constant.expression ']'
        |
          array.abstract.declarator '[' ']'
        ;
/********************************************************************************
*										*
*                      STRUCTURES, UNION, and ENUMERATORS			*
*										*
********************************************************************************/
struct.or.union.specifier: 
          struct.or.union ''
        | struct.or.union identifier.or.typedef.name
          ''
        | struct.or.union identifier.or.typedef.name
        | struct.or.union ''
        | struct.or.union identifier.or.typedef.name ''
        ;
struct.or.union:  
          STRUCT   
        | UNION    
        ;
struct.declaration.list: 
          struct.declaration
        | struct.declaration.list struct.declaration
        ;
struct.declaration: 
          struct.declaring.list ';'
        | struct.default.declaring.list ';'
        ;
struct.default.declaring.list:         
          type.qualifier.list struct.identifier.declarator
        | struct.default.declaring.list ',' struct.identifier.declarator
        ;
struct.declaring.list:         
          type.specifier struct.declarator
        | struct.declaring.list ',' struct.declarator
        ;
struct.declarator: 
          declarator bit.field.size.opt attributes.opt
        | bit.field.size attributes.opt
        ;
struct.identifier.declarator: 
          identifier.declarator bit.field.size.opt attributes.opt
        | bit.field.size attributes.opt
        ;
bit.field.size.opt: 
        | bit.field.size
        ;
bit.field.size: 
          ':' constant.expression 
        ;
enum.specifier: 
          ENUM ''
        | ENUM identifier.or.typedef.name ''
        | ENUM identifier.or.typedef.name
        ;
enumerator.list: 
          identifier.or.typedef.name enumerator.value.opt
        | enumerator.list ',' identifier.or.typedef.name enumerator.value.opt
        ;
enumerator.value.opt: 
        | '=' constant.expression     
        ;
comma.opt: 
        | ','              
        ;
/********************************************************************************
*										*
*                                  STATEMENTS					*
*										*
********************************************************************************/
costream.composite.statement: 
          composite.body.operator
	    | statement
		;
statement:                      
          labeled.statement
        | compound.statement
        | expression.statement
        | selection.statement
        | iteration.statement
        | jump.statement
        | error ';'
        ;
labeled.statement:              
          IDENTIFIER ':'             
          statement
        | CASE constant.expression ':' statement
        | DEFAULT ':' statement
        | TYPEDEFname ':' statement
        ;
compound.statement:             
          lblock rblock
        | lblock declaration.list rblock
        | lblock composite.body.statement.list rblock
        | lblock declaration.list composite.body.statement.list rblock
        ;
lblock: '{'  { EnterScope(); }
rblock: '}'  { ExitScope(); }
compound.statement.no.new.scope:             
          '{' '}'
        | '{' declaration.list '}'
        | '{' statement.list '}'
        | '{' declaration.list statement.list '}'
        ;
declaration.list:               
          declaration                   
        | declaration.list declaration  
        ;
statement.list:                 
          statement                   
        | statement.list statement    
        ;
expression.statement:           
          expression.opt ';'
        ;
selection.statement:            
          IF '(' expression ')' costream.composite.statement
        | IF '(' expression ')' costream.composite.statement ELSE costream.composite.statement
        | SWITCH  '(' expression ')' statement
        ;
iteration.statement:            
          WHILE 
          '(' expression ')' statement
        | DO 
          statement WHILE '(' expression ')' ';'
        | FOR '(' expression.opt ';' expression.opt ';' expression.opt ')'  
          costream.composite.statement
        | FOR '(' error ';' expression.opt ';' expression.opt ')'  
          costream.composite.statement
        | FOR '(' expression.opt ';' expression.opt ';' error ')'  
          costream.composite.statement
        | FOR '(' expression.opt ';' error ';' expression.opt ')'  
         costream.composite.statement
        | FOR '(' error ')'  costream.composite.statement
        ;
jump.statement:                 
          GOTO IDENTIFIER ';'
        | CONTINUE ';'
        | BREAK ';'
        | RETURN expression.opt ';'
        | GOTO TYPEDEFname ';'
        ;
/********************************************************************************
*										*
*                            EXTERNAL DEFINITIONS                               *
*										*
********************************************************************************/
translation.unit:               
          external.definition
        | translation.unit external.definition   
        ;
external.definition:            
          declaration
              $$ = $1;
            }
        | function.definition  
              $$ = MakeNewList($1); 
            }
		| composite.definition
              $$ = MakeNewList($1); 
		}
		;
function.definition:            
          identifier.declarator
          compound.statement.no.new.scope
        | identifier.declarator BOGUS
        | declaration.specifier      identifier.declarator
          compound.statement.no.new.scope
        | type.specifier             identifier.declarator
          compound.statement.no.new.scope
        | declaration.qualifier.list identifier.declarator
          compound.statement.no.new.scope
        | type.qualifier.list        identifier.declarator
          compound.statement.no.new.scope
        | old.function.declarator
          compound.statement.no.new.scope
        | declaration.specifier old.function.declarator 
          compound.statement.no.new.scope
        | type.specifier old.function.declarator
          compound.statement.no.new.scope
        | declaration.qualifier.list old.function.declarator
          compound.statement.no.new.scope
        | type.qualifier.list        old.function.declarator
          compound.statement.no.new.scope
        | old.function.declarator old.function.declaration.list
          compound.statement.no.new.scope
        | declaration.specifier old.function.declarator old.function.declaration.list
          compound.statement.no.new.scope
        | type.specifier old.function.declarator old.function.declaration.list
          compound.statement.no.new.scope
        | declaration.qualifier.list old.function.declarator old.function.declaration.list
          compound.statement.no.new.scope
        | type.qualifier.list old.function.declarator old.function.declaration.list
          compound.statement.no.new.scope
        ;
old.function.declaration.list:
             declaration.list
        ;
/********************************************************************************
*										*
*                          CONSTANTS and LITERALS                               *
*										*
********************************************************************************/
/* 
  CONSTANTS.  Note ENUMERATIONconstant is treated like a variable with a type
  of "enumeration constant" (elsewhere)
*/
constant: 
          FLOATINGconstant      
        | INTEGERconstant       
        | OCTALconstant         
        | HEXconstant           
        | CHARACTERconstant     
        ;
string.literal.list: 
          STRINGliteral         
        | string.literal.list STRINGliteral
        ;
type.qualifier: 
          CONST      
        | VOLATILE  
        | INLINE    
        ;
pointer.type.qualifier: 
          CONST      
        | VOLATILE  
        ;
storage.class: 
          TYPEDEF    
        | EXTERN     
        | STATIC     
        | AUTO       
        | REGISTER   
        ;
basic.type.name: 
          VOID       
        | CHAR       
        | INT        
        | FLOAT      
        | DOUBLE     
        | SIGNED     
        | UNSIGNED   
        | SHORT      
        | LONG      
        ;
%%
