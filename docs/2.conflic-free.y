
%{
  //无冲突文法
%}
%%
prog_start: translation_unit EOF          ;
/************************************************************************/
/*              1. 文法一级入口,由下面三种文法组成                           */
/*                 1.1. declaration 声明                                 */
/*                 1.2. function_definition 函数声明                      */
/*                 1.3. composite_definition 数据流计算单元声明             */
/*************************************************************************/
translation_unit:
      external_declaration                  
    | translation_unit external_declaration 
    ;

external_declaration:
      function_definition
    | declaration
    | composite_definition
    | IMPORT MATRIX   ';'                   
    ;
/*************************************************************************/
/*              1.1 declaration 由下面2种文法+2个基础组件组成                */
/*                      1.1.1 declaring_list                             */
/*                      1.1.2 stream_declaring_list                      */
/*                      1.1.3 initializer                                */
/*************************************************************************/ 
declaration:
      declaring_list ';'                          
    | stream_declaring_list ';'                   
    ;
declaring_list:
      type_specifier   init_declarator_list       
    ;
init_declarator_list:
      init_declarator                             
    | init_declarator_list ',' init_declarator    
    ;

init_declarator:
      declarator                                  
    | declarator '=' initializer                  
    ;

declarator:
      IDENTIFIER                                  
    | '(' declarator ')'                          
    | declarator '[' constant_expression ']'      
    | declarator '[' ']'                          
    ;  
/*************************************************************************/
/*                      1.1.2 stream_declaring_list                      */
/*************************************************************************/    
stream_declaring_list:
      stream_type_specifier IDENTIFIER            
    | stream_declaring_list ',' IDENTIFIER        
    ;
/*************************************************************************/
/*                      1.1.3 initializer                                */
/*************************************************************************/
initializer:
      assignment_expression
    | '{' initializer_list '}'                    
    | '{' initializer_list ',' '}'                
    ;

initializer_list:
      initializer                                 
    | initializer_list ',' initializer            
    ;
/*************************************************************************/
/*              1.2 function_definition 函数声明                          */
/*                      1.2.1 parameter_type_list                        */
/*                      1.2.1 function_body                              */
/*************************************************************************/
function_definition:
      type_specifier declarator '(' parameter_type_list ')' compound_statement 
    | type_specifier declarator '(' ')' compound_statement 
    ;

parameter_type_list:
      parameter_declaration                         
    | parameter_type_list ',' parameter_declaration 
    ;

parameter_declaration:
      type_specifier declarator         
    ;
/*************************************************************************/
/*              1.3 composite.definition 数据流计算单元声明                */
/*                      1.3.1 composite.head                             */
/*                      1.3.2 composite.body                             */
/*************************************************************************/
composite_definition:
      composite_head composite_body                         
    ;
composite_head:
      COMPOSITE IDENTIFIER '(' composite_head_inout ')'     
    ;
composite_head_inout:
      /*empty*/                                                                           
    | INPUT composite_head_inout_member_list                                              
    | INPUT composite_head_inout_member_list ',' OUTPUT composite_head_inout_member_list  
    | OUTPUT composite_head_inout_member_list                                             
    | OUTPUT composite_head_inout_member_list ',' INPUT composite_head_inout_member_list  
    ;
composite_head_inout_member_list:
      composite_head_inout_member                                                                           
    | composite_head_inout_member_list ',' composite_head_inout_member                      
    ;
composite_head_inout_member:
      stream_type_specifier IDENTIFIER                                                        
    ;
stream_type_specifier:
      STREAM '<' stream_declaration_list '>'                            
    ;    
stream_declaration_list:
      type_specifier IDENTIFIER                                         
    | stream_declaration_list ',' type_specifier IDENTIFIER             ) }
    ;
/*************************************************************************/
/*                      1.3.2 composite_body                             */
/*************************************************************************/
composite_body:
      '{' composite_body_param_opt statement_list '}'                         
    ;
composite_body_param_opt:
      /*empty*/                                                         
    | PARAM parameter_type_list ';'                                     
    ;
/*****************************************************************************/
/*        2. operator_add  composite体内的init work window等组件   */
/*             2_1   ADD operator_pipeline                                   */
/*             2_2   ADD operator_splitjoin                                  */
/*             2_3   ADD operator_default_call                               */
/*****************************************************************************/
operator_add:
          ADD operator_pipeline                             
        | ADD operator_splitjoin                            
        | ADD operator_layer                                
        | ADD operator_default_call                         
        ;  

operator_pipeline:
          PIPELINE '{'  statement_list '}'
        ;
operator_splitjoin:
          SPLITJOIN '{' split_statement  statement_list  join_statement '}'     
        | SPLITJOIN '{' statement_list split_statement statement_list join_statement '}'  
        ;
split_statement:
          SPLIT duplicate_statement                                   
        | SPLIT roundrobin_statement                        
        ;
roundrobin_statement:
          ROUNDROBIN '(' ')' ';'                            
        | ROUNDROBIN '(' argument_expression_list ')' ';'   
        ;
duplicate_statement:
          DUPLICATE '('  ')' ';'                            
        | DUPLICATE '(' exp ')'  ';'                        
        ;
join_statement:
          JOIN roundrobin_statement                         
        ;
operator_default_call:
          IDENTIFIER  '(' ')' ';'                           
        | IDENTIFIER  '(' argument_expression_list ')' ';'  
        ;  
operator_layer:      
          DENSE  '(' argument_expression_list ')' ';'       
        | CONV2D '(' argument_expression_list ')' ';'       
        | MAXPOOLING2D '(' argument_expression_list ')' ';'     
        | AVERAGEPOOLING2D '(' argument_expression_list ')' ';' 
        | ACTIVATION '(' argument_expression_list ')' ';'       
        ; 
/*************************************************************************/
/*        3. statement 花括号内以';'结尾的结构是statement                    */
/*************************************************************************/    
statement:
      labeled_statement
    | compound_statement
    | expression_statement
    | selection_statement
    | iteration_statement
    | jump_statement
    | declaration
    | operator_add
    ;
labeled_statement:
      CASE constant_expression ':' statement    
    | DEFAULT ':' statement                     
    ;
compound_statement: 
      '{' '}'                                    
    | '{' statement_list '}'                    
    ;
statement_list:
      statement                
    | statement_list statement 
    ;
expression_statement:
      ';'                       
    | multi_expression ';'      
    ;
selection_statement:
      IF '(' expression ')' statement %prec IF_WITHOUT_ELSE 
    | IF '(' expression ')' statement ELSE statement
    | SWITCH '(' expression ')' statement
    ;
iteration_statement:
      WHILE '(' expression ')' statement 
    | DO statement WHILE '(' expression ')' ';' 
    | FOR '(' expression_statement expression_statement ')' statement
    | FOR '(' expression_statement expression_statement expression ')' statement
    ;
jump_statement:
      CONTINUE ';'          
    | BREAK ';'             
    | RETURN ';'            
    | RETURN expression ';' 
    ;    

/*************************************************************************/
/*        4. expression 计算表达式头节点                                    */
/*            4.1 矩阵的常量节点                                            */
/*************************************************************************/
matrix_slice_pair:
        ':'                     
    |   expression                  
    |   exp ':'                
    |   ':' exp                
    |   exp ':' exp            
    ;
matrix_slice_pair_list:
        matrix_slice_pair                               
    |   matrix_slice_pair_list ',' matrix_slice_pair    
    ;
matrix_slice:
      '[' matrix_slice_pair_list ']'                    
    ;
/*************************************************************************/
/*            4.2 expression 其他节点                                     */
/*************************************************************************/
vector_expression:
      '[' multi_expression ']'      
    ;
multi_expression:
      expression                    
    | multi_expression ',' expression    
    ;
primary_expression:
      IDENTIFIER            
    | 'NUMBER'                   
    | STRING_LITERAL             
    | '(' multi_expression ')'   
    | vector_expression 
    ;
operator_arguments:
      '(' ')'               
    | '(' argument_expression_list ')' 
    ;
postfix_expression:
      primary_expression     
    | postfix_expression matrix_slice                           
    | postfix_expression operator_arguments                 
    | MATRIX '.' IDENTIFIER                                 
    | postfix_expression '.' IDENTIFIER                     
    | postfix_expression '++'                               
    | postfix_expression '--'                               
    | FILEREADER '(' ')' '(' STRING_LITERAL ',' NUMBER ')'  
    | postfix_expression operator_arguments operator_selfdefine_body  
    |  SPLITJOIN '(' argument_expression_list ')'  '{' split_statement statement_list  join_statement '}' 
    |  SPLITJOIN '(' argument_expression_list ')'  '{' statement_list split_statement statement_list  join_statement '}'
    |   PIPELINE '(' argument_expression_list ')'  '{' statement_list '}'
    |   SEQUENTIAL '(' argument_expression_list ')' '(' argument_expression_list ')' '{' statement_list '}' 
    ;

argument_expression_list:
      assignment_expression                                 
    | argument_expression_list ',' assignment_expression    
    ;

unary_expression:
      postfix_expression                
    | '++' unary_expression             
    | '--' unary_expression             
    | unary_operator unary_expression   
    | '(' basic_type_name ')' unary_expression    
    ;

unary_operator:
      '+'
    | '-'
    | '~'
    | '!'
    ;

exp:
      unary_expression
    | exp "*" exp   
    | exp "/" exp   
    | exp "+" exp   
    | exp "-" exp   
    | exp "%" exp   
    | exp "^" exp   
    | exp "|" exp   
    | exp "&" exp   
    | exp "<" exp   
    | exp ">" exp   
    | exp "<=" exp  
    | exp ">=" exp  
    | exp "==" exp  
    | exp "!=" exp  
    | exp "<<" exp  
    | exp ">>" exp  
    | exp "||" exp  
    | exp "&&" exp  
    ;

conditional_expression:
      exp
    | exp '?' expression ':' expression 
    ;

assignment_expression:
      conditional_expression
    | unary_expression assignment_operator assignment_expression
    ;
assignment_operator:
      '='
    | ASSIGNMENT_OPERATOR
    ;
expression:
      assignment_expression 
    ;

constant_expression:
      conditional_expression
    ;
/*************************************************************************/
/*        4.1 postfix_operator COStream 的 operator 表达式                */
/*************************************************************************/
operator_selfdefine_body:
       '{' operator_selfdefine_body_init operator_selfdefine_body_work operator_selfdefine_body_window_list '}'
     | '{' statement_list operator_selfdefine_body_init  operator_selfdefine_body_work operator_selfdefine_body_window_list '}'
     ;    
operator_selfdefine_body_init:
      /*empty*/
    | INIT compound_statement 
    ;
operator_selfdefine_body_work:
      WORK compound_statement 
    ;
operator_selfdefine_body_window_list:
      /*empty*/                                         
    | WINDOW '{' operator_selfdefine_window_list '}'  
    ;
operator_selfdefine_window_list:
      operator_selfdefine_window                                    
    | operator_selfdefine_window_list operator_selfdefine_window    
    ;
operator_selfdefine_window:
      IDENTIFIER window_type ';'                       
    ;
window_type:
      SLIDING '('  ')'                                  
    | TUMBLING '('  ')'                                    
    | SLIDING '(' argument_expression_list ')'        
    | TUMBLING '(' argument_expression_list ')'        
    ;     
/*************************************************************************/
/*        5. basic 从词法TOKEN直接归约得到的节点,自底向上接入头部文法结构        */
/*************************************************************************/
type_specifier:
          basic_type_name       
        | CONST basic_type_name  
        ;
basic_type_name:
          INT   
        | LONG  
        | LONG LONG  
        | FLOAT 
        | DOUBLE
        | STRING
        | MATRIX
        ;
