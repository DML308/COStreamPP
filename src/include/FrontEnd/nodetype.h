#ifndef _NODETYPE_H_
#define _NODETYPE_H_
/* definition of Node data structure */
typedef enum {
  /* expression nodes */
  constant=1, Id, Binop, Unary, Cast, Ternary,Initializer,
  /* statement nodes */
  Label, Switch, Case, Default, If, IfElse, While, Do, For, Continue,
  Break, Return, Block,Paren,
  /* type nodes */
  primary,
  /* array */
  Array,
  /* declaration node */
  Decl,
  /* function */
  FuncDcl, Call,
  /*----------12----SPL node----------*/
  StrDcl,		/*stream SPL*/
  Compdcl,		/*composite SPL*/
  Composite,	/* composite def node SPL*/
  ComInOut,		/*composite SPL*/
  InOutdcl,
  CompHead,
  CompBody,		/*composite.block SPL*/
  Param,		/*composite.body.param SPL*/
  ParamDcl,
  OperBody,		/*operator SPL*/
  OperHead,		/*operator SPL*/
  Operator_,	/* operator def node SPL*/
  Window,		/*operator.body.window SPL*/
  WindowStmt,
  Sliding,		/*sliding window SPL*/
  Tumbling,		/*tumbling window SPL*/

  /*-------7-------New For SPL----------*/
  CompositeCall,/*composite.head.inout.stream.id SPL*/
  Pipeline,
  SplitJoin,
  Split,
  Join,
  RoundRobin,
  Duplicate,
  Squential,
  Layer,
  
  /********1********新文法**********/
  Add, Tuple
  /*
  删除掉的节点： Var,Graph,StreamFor,StreamIf,StreamIfElse,Eviction,Trigger,logic
  修改过的节点： comBodyNode,PipelineNode,SplitJoinNode,slidingNode,tumblingNode
  新增加的节点： Add
  */
} NodeType;

#endif
