%{
  //约定的文法, 去掉花括号后的简洁内容放在这里
%}
%%
/*************************************************************************/
/*               1. 文法一级入口,由下面三种文法组成                           */
/*                  1.1. decalration 声明                                 */
/*                  1.2. function.definition 函数声明                      */
/*                  1.3. composite.definition 数据流计算单元声明             */
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
/*               1.1 decalration 由下面三种文法组成                         */
/*                  1.1.1 declaring.list                                 */
/*                  1.1.2 default.declaring.list                         */
/*************************************************************************/
declaration:
		  declaring.list ';'
		| default.declaring.list ';'
		;
declaring.list: 
          declaration.specifier 	declarator attributes.opt  initializer.opt
		    | type.specifier 			declarator attributes.opt initializer.opt  
		    | declaring.list 	',' 	declarator  attributes.opt initializer.opt
        | declaration.specifier 	error attributes.opt initializer.opt  
		    | type.specifier 			error attributes.opt initializer.opt  
		    | declaring.list 	',' 	error
        ;
default.declaring.list:  
          declaration.qualifier.list identifier.declarator attributes.opt initializer.opt 
	      | stream.type.specifier identifier.declarator 
        | type.qualifier.list identifier.declarator ttributes.opt initializer.opt
        | default.declaring.list ',' identifier.declarator attributes.opt initializer.opt
        | declaration.qualifier.list error attributes.opt initializer.opt
        | type.qualifier.list error attributes.opt initializer.opt
        | default.declaring.list ',' error
        ;
/*************************************************************************/
/*                  1.2 function 讨论后再完成                              */
/*************************************************************************/
function.definition: /* 暂时省略 */;


/*************************************************************************/
/*                  1.3 composite 暂时省略                                   */
/*************************************************************************/
composite.definition:						
		composite.head composite.body.no.new.scope  
		;


/*************************************************************************/
/*               2. expression 计算表达式节点,自底向上接入头部文法结构          */
/*************************************************************************/
%%
/* ----语法树结束----*/
