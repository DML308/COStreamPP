#ifndef _Node_H_
#define _Node_H_
//#include "basics.h"
typedef enum {
  /* expression Nodes */
  Const, Id, Binop, Unary, Cast, Comma, Ternary, Array, Call, Initializer,
  ImplicitCast,
  /* statement Nodes */
  Label, Switch, Case, Default, If, IfElse, While, Do, For, Goto, Continue, 
  Break, Return, Block, 
  /* type Nodes */
  Prim, Tdef, Ptr, Adcl, Fdcl, Sdcl, Udcl, Edcl, 
  /* declaration Node */
  Decl, 
  /* GCC __attribute__ extension */
  Attrib, 
  /* procedure def Node */
  Proc,
  /* random text and preprocessor command Node */
  Text,
  /*----------12----SPL Node----------*/
  STRdcl,		/*stream SPL*/
  Comdcl,		/*composite SPL*/
  Composite,	/* composite def Node SPL*/
  ComInOut,		/*composite SPL*/
  ComBody,		/*composite.block SPL*/
  Param,		/*composite.body.param SPL*/		 
  OperBody,		/*operator SPL*/	
  Operdcl,		/*operator SPL*/
  Operator_,	/* operator def Node SPL*/
  Window,		/*operator.body.window SPL*/
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
  
  /********1********新文法**********/
  Add,Uncertainty
  /*
  删除掉的节点： Var,Graph,StreamFor,StreamIf,StreamIfElse,Eviction,Trigger,logic
  Ptr,
  修改过的节点： comBodyNode,PipelineNode,SplitJoinNode,slidingNode,tumblingNode
  新增加的节点： Add
  */
} NodeType;


typedef enum {

  EMPTY_TQ = 0,
/* 
   声明限定符. 声明可以有多个限定符，但有些是互斥的（如存储类）。在解析声明时，
   decl限定符保存在其基本类型的tq字段中。 在解析了decl之后，SetDeclType然后
   将限定符移动到它们的适当位置，即Decl节点。

   添加一个声明修饰符:
   1.在下面定义一个新符号，注意它的位不与现有的decl和类型限定符冲突，因为它们可以在同一个TypeQual变量中共存。
   2.在T_DECL_QUALS（下面）中插入符号。
   3.如果在源中解析限定符（而不是计算），则将词法标记（c4.l）和新生成添加到storage.classes（ANSI-C.y）。
   4.添加代码以在TQtoText（type.c）中打印出其名称。
   5.如有必要，将合并逻辑添加到MergeTypeQuals（type.c）。
*/
  T_AUTO     = 0x00001,
  T_EXTERN   = 0x00002,
  T_REGISTER = 0x00003,
  T_STATIC   = 0x00004,
  T_TYPEDEF  = 0x00005,
  /* Insert new storage classes here */

  T_STORAGE_CLASSES = (T_AUTO | T_EXTERN | T_REGISTER | T_STATIC | T_TYPEDEF),
#define STORAGE_CLASS(tq) ((tq) & T_STORAGE_CLASSES)

  /* the following qualifiers are all mutually exclusive,
     so they can share bits */
  T_TOP_DECL    = 0x00010,   /* top-level decl */
  T_BLOCK_DECL  = 0x00020,   /* local decl in a block */
  T_FORMAL_DECL = 0x00030,   /* formal parameter decl(形参) */
  T_SU_DECL     = 0x00040,   /* struct/union field decl */
  T_ENUM_DECL   = 0x00050,   /* enumerated constant decl */
  /* Insert new decl locations here */
  T_STREAM_DECL  = 0x00060,   /* stream field decl SPL*/
  T_COMMAL_DECL = 0x00070,
  
  T_DECL_LOCATIONS = (T_TOP_DECL | T_BLOCK_DECL | T_FORMAL_DECL |
		      T_SU_DECL | T_ENUM_DECL | T_STREAM_DECL | T_COMMAL_DECL/*SPL*/),
#define DECL_LOCATION(tq)  ((tq) & T_DECL_LOCATIONS)


    /* Flag for redundant external declaration, which is defined as
     * an external declaration (NOT a definition, so it must have no
     * initializer or function body) of a name previously declared external 
     * in the same scope.  A trivial example:
     *    extern int x;
     *    extern int x;    <-- redundant
     * But:
     *    { extern int x; }
     *    { extern int x; }  <-- not redundant
     * because the two declarations have different scopes.
     */
  T_REDUNDANT_EXTERNAL_DECL = 0x00100,

  /* Insert new decl qualifiers here */

  T_DECL_QUALS = (T_STORAGE_CLASSES | T_DECL_LOCATIONS | T_REDUNDANT_EXTERNAL_DECL),
#define DECL_QUALS(tq)    ((tq) & T_DECL_QUALS)


/*
   类型修饰符.  多个类型限定符可以应用于类型。 它们可能与任何原始类型或复杂类型相
   关联。 解析后可能会移动某些类型限定符 - 例如，T_INLINE将移动到它描述的顶级Fdcl。

   添加类型修饰符的方法:
       1.在下面定义一个新符号，注意它的位与现有存储类和类型限定符不冲突，因为它们可以在同一个TypeQual变量中共存。
       2.在T_TYPE_QUALS（下面）中插入符号。
       3.将词法标记（c4.l）和新生成添加到type.qualifiers或pointer.type.qualifiers（ANSI-Cy），具体取决于类型限定符是否仅在类型的开头允许，或者可以 在'*'之后出现像const和volatile。
       4.添加代码以在TQtoText（type.c）中打印出其名称。
       5.如果具有类型限定符的对象始终与没有类型限定符的对象分配兼容，则将其符号添加到TQ_ALWAYS_COMPATIBLE（下面）。
       6.如有必要，将合并逻辑添加到MergeTypeQuals（type.c）。
    */
    T_CONST    = 0x01000,   /* leave some room for new decl qualifiers */
    T_VOLATILE = 0x02000,
    T_INLINE   = 0x04000,
    T_SUE_ELABORATED  = 0x08000,   /* on an Sdcl/Udcl/Edcl, indicates 
				      whether SUE's field List appeared
				      at that point in the source */
    T_STREAM_ELABORATED  = 0x10000,   /* on an STRdcl/, indicates 
								 whether stream's field List appeared
								 at that point in the source SPL*/
#define SUE_ELABORATED(tq)   (((tq) & T_SUE_ELABORATED) != 0)
#define STREAM_ELABORATED(tq)   (((tq) & T_STREAM_ELABORATED) != 0)/*SPL*/
    /* Insert new type qualifiers here */

    T_TYPE_QUALS = (T_CONST | T_VOLATILE | T_INLINE | T_SUE_ELABORATED | T_STREAM_ELABORATED/*SPL*/)
#define TYPE_QUALS(tq)    ((tq) & T_TYPE_QUALS)
    
/* Type qualifiers Listed in TQ_COMPATIBLE are ignored
   when checking two types for compatibility (weaker than strict equality). */
#define TQ_COMPATIBLE   (T_CONST | T_VOLATILE | T_INLINE | T_SUE_ELABORATED | T_STREAM_ELABORATED/*SPL*/)

} TypeQual;


/* BasicType涵盖了所有不同的基本类型。还应该将新的基本类型添加到InitTypes（）。*/
typedef enum {
  /* Unspecified=0, */
  Uchar=1, Schar, Char,
  Ushort, Sshort,
  Uint, Sint, 
  Int_ParseOnly /* used only in parsing -- FinishPrimType converts to Sint */, 
  Ulong, Slong,
  Ulonglong, Slonglong,
  Float, Double, Longdouble, 
  Void, Ellipsis,
  MaxBasicType, /* must be last */
} BasicType;

/* 

虽然在解析期间构造了BasicType，但其高位（在BasicType本身所需的位之上）是表示类型
修饰符（如signedness和length）的标志。 （扩展语言可能有自己的类型修饰符的附加标志
。）在使用FinishPrimType完成BasicType之后，清除这些高位，并将BasicType设置为上面
的枚举值之一。 （因此最终的BasicType值是一个小整数，在switch语句中比在Sshort == Short 
| Int之类的位域表示更快。）


To add a new category of type modifier:

   1. Insert it in TypeSpecifier (below), being careful not to conflict
      with bits used by other modifiers.
   2. Change to BASIC2EXPLODED and EXPLODED2BASIC to convert it (below).
   3. Add merging and finishing logic to MergePrimTypes and FinishPrimType
      (type.c).
   4. Add type names to TypeSpecifierName (type.c).

*/

typedef enum { 
  /* Length -- 0 in these bits means no sign specified */
  Short    = 0x0100, 
  Long     = 0x0200, 
  Longlong = 0x0300,
  LengthMask = Short | Long | Longlong,

  /* Signedness -- 0 in these bits means no sign specified */
  Signed   = 0x0400,
  Unsigned = 0x0800,
  SignMask = Signed | Unsigned,

  /* INSERT EXTENSIONS HERE */ 
  /* Base type -- Char, Int, Float, Double, etc.  Uses subset of
     values defined for BasicType. */
  BasicTypeMask = 0x00FF
} TypeSpecifier;

/* 扩展类型 */
typedef struct {
    TypeSpecifier base;
    TypeSpecifier sign;
    TypeSpecifier length;
  /* INSERT EXTENSIONS HERE */ 
} ExplodedType;


/* 两个宏定义 目前还不清楚代表什么 */
#define BASIC2EXPLODED(bt, et)  \
   ((et).sign = (bt) & SignMask, (et).length = (bt) & LengthMask, \
  /* INSERT EXTENSIONS HERE */     \
    (et).base = (bt) & BasicTypeMask)

#define EXPLODED2BASIC(et, bt)  \
   ((bt) = (et).sign | (et).length | \
  /* INSERT EXTENSIONS HERE */     \
    (et).base)

/*split节点的两种类型 */
typedef enum{
    S_Duplicate,
    S_RoundRobin
}SplitStyle;

/* struct/union/enum type 将不会保留*/
typedef struct {
    NodeType typ;
    Bool complete;  /* true_ if struct has been elaborated with field List */
    Coord coord;  /* location of definition (left brace) */
    Coord right_coord;  /* location of right brace */
    Bool visited;  /* this struct has been visited in size calculation */ 
    int size;
    int align;
    const char *name;
    List *fields;
} SUEtype;  

/*************************************************************************/
/*                                                                       */
/*                          Expression Nodes                             */
/*                                                                       */
/*************************************************************************/

typedef struct {
	const char* text;  /* text will be NULL if constant was not
			      derived from source, but computed by 
			      type-checker. */
	Node *type;  /* type is required to be one of:
			     Sint, Uint, Slong, Ulong,
			     Float, Double, or Adcl(Char) */
	union {
	  TARGET_INT    i;
	  TARGET_UINT   u;
	  TARGET_LONG   l;
	  TARGET_ULONG ul;
	  float         f;
	  double        d;
	  const char *  s;
	} value;
} ConstNode;


typedef struct {
	const char* text;
	Node *decl;			//保存id的声明
	/* type is decl->u.decl.type */
	Node *value;
} idNode;

typedef struct {
	OpType op;
	opDataType opType;/*zww:define for spl,记录该2元操作的数据类型：整型，浮点型*/
	Node *left;
	Node *right;
	Node *type;
	Node *value;
	List *valueList;/*zww:对结构体常量传播的dot操作*/
} binopNode;

typedef struct {
	OpType op;
	opDataType opType;/*zww:define for spl,记录该2元操作的数据类型：整型，浮点型*/
	Node *expr;
	Node *type;
	Node *value;
} unaryNode;

typedef struct {
	Node *type;
	Node *expr;
	Node *value;
} castNode;

/* 以前Comma被重载以表示逗号表达式和大括号初始化器。 但是，大括号初始化器的语义是完全
不同的，因此消除重载并引入新的AST节点Initializer似乎更加清晰。. -- rcm */
typedef struct {
	List *exprs;
} commaNode;


typedef struct {
	Node *cond;
	Node *true_;
	Node *false_;
	Coord colon_coord;  /* source line and offset of colon */
	Node *type;
	Node *value;
} ternaryNode;

typedef struct {
	Node *type;
	Node *name;
	List *dims;
	Node *value;//zww：常量传播用
} arrayNode;

typedef struct {
	Node *name;
	List *args;
} callNode;

typedef struct {
	List *exprs;
} initializerNode;


/*语义检查器使用ImplicitCasts来隐式地为表达式提供“从上面”的类型或值;
 例如，char变量隐式转换为表达式中的整数，而不是通过更改标识符节点上的
 类型，而是通过在标识符节点上方插入一个指示其新的上下文类型的implici
 tcast。*/
typedef struct {
	Node *expr;
	Node *type;
	Node *value;
} implicitcastNode;

/*************************************************************************/
/*                                                                       */
/*                          Statement Nodes                              */
/*                                                                       */
/*************************************************************************/

typedef struct {
	const char* name;
	Node *stmt;
	List *references;
	FlowValue label_values;
} labelNode;



typedef struct {
	Node *expr;
	Node *stmt;
	List *cases;
	Bool has_default;    /* true_ if cases includes a Default Node */
	struct SwitchCheck *check;  /* points to hash table of case
				       expression values for
				       duplicate-checking */
	FlowValue switch_values, break_values;
} SwitchNode;

typedef struct {
	Node *expr;
	Node *stmt;
	Node *container;
} CaseNode;

typedef struct {
	Node *stmt;
	Node *container;
} DefaultNode;

/* If statement with no Else */
typedef struct {
	Node *expr;
	Node *stmt;
} IfNode;

typedef struct {
	Node *expr;
	Node *true_;
	Node *false_;
	Coord else_coord;  /* coordinates of ELSE keyword */
} IfElseNode;

typedef struct {
	Node *expr;
	Node *stmt;
	FlowValue loop_values, break_values;
} WhileNode;

typedef struct {
	Node *stmt;
	Node *expr;
	Coord while_coord;  /* coordinates of WHILE keyword */
	FlowValue loop_values, continue_values, break_values;
} DoNode;

typedef struct {
	Node *init;
	Node *cond;
	Node *next;
	Node *stmt;
	FlowValue loop_values, continue_values, break_values;
} ForNode;

typedef struct {
	Node *label;
} GotoNode;

typedef struct {
	Node *container;
} ContinueNode;

typedef struct {
	Node *container;
} BreakNode;

typedef struct {
	Node *expr;
	Node *proc;   /* points to Proc Node containing this return stmt */
} ReturnNode;

typedef struct {
	List *decl;
	List *stmts;
	Coord right_coord;  /* coordinates of right brace */
	Node *type;  /* the type of a {...} block is void;
			the type of a ({..}) block is the type of its last
			  statement (initially NULL, then filled in by
			  SemCheckNode) */
} BlockNode;


/*************************************************************************/
/*                                                                       */
/*                          Type Nodes                                   */
/*                                                                       */
/*************************************************************************/
typedef struct {
	TypeQual tq;
	BasicType basic;
} primNode;

typedef struct {
	const char* name;
	TypeQual tq;
	Node *type;
} tdefNode;

typedef struct {
        TypeQual tq;
        Node *type;
} ptrNode;

typedef struct {
        TypeQual tq;
        Node *type;
        Node *dim;
	int   size;
} adclNode;

typedef struct {
	TypeQual tq;
	List *args;
	Node *returns;
} fdclNode;

typedef struct {
	TypeQual tq;
	SUEtype* type; /* this is a child link iff SUE_ELABORATED(tq) */
} sdclNode;

typedef struct {
	TypeQual tq;
	SUEtype* type; /* this is a child link iff SUE_ELABORATED(tq) */
} udclNode;

typedef struct {
	TypeQual tq;
	SUEtype* type; /* this is a child link iff SUE_ELABORATED(tq) */
} edclNode;


/*************************************************************************/
/*                                                                       */
/*                          Other Nodes                                  */
/*                                                                       */
/*************************************************************************/

typedef struct {
	const char* name;
	TypeQual tq;       /* storage class and decl qualifiers */
	Node *type;
	Node *init;    /* in other versions of c-parser, init is
			       overloaded to be the offset for structs -- 
			       but NOT in c-to-c */
	Node *prim_init;//zww：为了保留数组定义是初始化列表 12.2.10
	Node *bitsize;
	int  references;    /* number of references to declared name */
	List *attribs;      /* GCC __attribute__ declarations */
} declNode;

typedef struct {
  const char *name;         /* name of attribute, like "packed" or "const" */
  Node *arg;           /* expression arguments of attribute */
} attribNode;

typedef struct {
	Node *decl;
	Node *body;
	FlowValue init_values;//zww:SPL
	FlowValue return_values;
} procNode;

typedef struct {
  const char *text;     /* may be NULL (treated same as "") */
  Bool start_new_line;  /* true_ ==> nothing but whitespace should appear
			   before <text> on line */
} textNode;

/*************************************************************************/
/*                                                                       */
/*                            Extensions                                 */
/*                                                                       */
/*************************************************************************/
/***********************Define Nodes For SPL----begin*********************/
/*************************************************************************/
/*                                                                       */
/*                      SPL Type Nodes                                   */
/*                                                                       */
/*************************************************************************/
typedef struct {
	NodeType typ;
	Bool complete;  /* true_ if struct has been elaborated with field List */
	Coord coord;  /* location of definition (left '<') */
	Coord right_coord;  /* location of right '>' */
	Bool visited;  /* this struct has been visited in size calculation */ 
	int size;
	int align;
	const char *name;
	List *fields;
} StreamType;  /*SPL stream type  */

typedef enum {
	Duplicate_,
	Roundrobin_,
	Join_,
	Common_,
} OperatorType; // SPL

/*************************************************************************/
/*                                                                       */
/*                      SPL Composite Nodes                              */
/*                                                                       */
/*************************************************************************/
/*******13******spl Node*******/
typedef struct {/*define for stream. SPL*/
	TypeQual tq;
	StreamType *type; /* this is a child link iff S_ELABORATED(tq) */
} strdclNode;

typedef struct {
	TypeQual tq;
	Node *inout;
} comDeclNode;

typedef struct {      
	Node *decl;  /*type->comInoutNode,name->id.name*/
	Node *body;
	Bool multi;
	FlowValue composite_values;/*for data flow among composites*/
} compositeNode;

typedef struct {
	TypeQual tq;
	List *outputs;
	List *inputs;
} comInOutNode;

typedef struct {
	Node *param;
	List *decl;
	List *comstmts;	/*	替代以前的var结构和graph结构 
							SPL composite中的特殊语句 和 c语句
							包括：	普通c语句
									stream类型的定义语句
									composite中的if-else语句
									operator_ 语句
									composite调用语句
									splitjoin语句
									pipeline语句
							*/  
	
	Coord left_coord;
	Coord right_coord;  /* coordinates of right brace */
	Node *type;  /* the type of a {...} block is void;
				 the type of a ({..}) block is the type of its last
				 statement (initially NULL, then filled in by
				 SemCheckNode) */
} comBodyNode;

typedef struct {
	List *parameters;
} paramNode;

typedef struct {
	List *state;  /*declaration.List  改动：无state关键字，其余结构不变*/
	Node *init;   /*compound.statement*/
	Node *work; /*compound.statement assert(work != null),kernel*/
	List *window; /*if window == null, its peek=pop=1;*/
	Coord left_coord;
	Coord right_coord;  /* coordinates of right brace */
	Node *type;  /* the type of a {...} block is void;
				 the type of a ({..}) block is the type of its last
				 statement (initially NULL, then filled in by
				 SemCheckNode) */
} operBodyNode;

typedef struct {
	TypeQual tq;
	List *outputs;/*stream.type.specifier identifier.List | identifier.List*/
	List *inputs;/*identifier.List*/
	List *arguments;/*argument.expression.List*/
} operDeclNode;

typedef struct {      
	Node *decl;  /*type->streamif.operator | splitjoin.operator | pipeline.operator | operator.invoke | operator.stream.define */
	Node *body;  /*body != null, it is a self-define operator*/
	FlowValue oper_values;
	OperatorType ot;
} operatorNode;

typedef struct {
	Node *id;
	Node *wtype;
} windowNode;

typedef struct {
	TypeQual tq;
	Node *sliding_value;
} slidingNode;

typedef struct {
	TypeQual tq;
	Node *tumbling_value;
} tumblingNode;
typedef struct {
	TypeQual tq;
	Node*uncertainty_value;
}uncertaintyNode;
/*-------7-------New For SPL----------*/
typedef struct {
	Node *call;/*call->operDeclNode*/
	Node *operdcl;
	Bool style;/*if style == true_, it is a SPL style call, else it is a StreamIt style call*/
	Node *actual_composite;// zww：在指向深拷贝实际展开后的composite
} comCallNode;

typedef struct {
	Node *output;
	Node *input;
	List *decl;
	List *stmts;	/*splitjoin 和 pipeline中的语句
						包括：	普通c语言语句
								'add'关键字开头的调用语句
								分支判断语句 spIf和spIfElse
								循环语句spfor
						*/
	Node *replace_composite; // SPL, for unfold.c
} PipelineNode;

typedef struct { 
	Node *output;
	Node *input;

	List *decl;
	List *initstmts;/* 为c语言初始化语句*/
	Node *split;
	List *stmts;	/*splitjoin 和 pipeline中的语句
						包括：	普通c语言语句
								'add'关键字开头的调用语句
								分支判断语句 spIf和spIfElse
								循环语句spfor
						*/
	Node *join;
	Node *replace_composite; // SPL, for unfold.c
	Node *splitOperator;
	Node *joinOperator;
} SplitJoinNode;

typedef struct {
	Node *type;
} splitNode;

typedef struct {
	Node *type;
} joinNode;

typedef struct {
	List *arguments;
} roundrobinNode;

typedef struct {
	Node *expr;
} duplicateNode;

/**********1*****新文法*******************/
typedef struct {
	Node *content;/* */
} addNode;

/***********************Define Nodes For SPL----over**********************/
/*************************************************************************/
/*                                                                       */
/*                            Node definition                            */
/*                                                                       */
/*************************************************************************/
struct NodeStruct {
	NodeType typ;
	Coord coord; /* location of first terminal in production */

	/* pass is incremented for each top-level call to PrintNode, so 
	   that PrintNode can print out decls anywhere they are used
	   without infinite recursion on recursive data structures. */
	short pass;

	/* parenthesized is set on expressions which were parenthesized
	   in the original source:  e.g., (x+y)*(w+z) would have 
	   parenthesized==TRUE on both PLUS Nodes, and parenthesized==FALSE
	   on both MULT Nodes. */
	short parenthesized;

	/* data-flow analysis information */
	Analysis analysis;

	union {
		adclNode adcl;
		arrayNode array;
		attribNode attrib;
		binopNode binop;	//二元操作符
		BlockNode Block;
		BreakNode Break;
		callNode call;		/* 函数调用 */
		CaseNode Case;
		castNode cast;
		commaNode comma;
		ConstNode Const;
		ContinueNode Continue;
		declNode decl;
		DefaultNode Default;
		DoNode Do;
		edclNode edcl;		//enum declare
		fdclNode fdcl;		
		ForNode For;		
		GotoNode Goto;
		idNode id;
		IfNode If;
		IfElseNode IfElse;
		implicitcastNode implicitcast;
		initializerNode initializer;
		labelNode label;
		primNode prim;
		procNode proc;
		ptrNode ptr;
		ReturnNode Return;
		sdclNode sdcl;
		SwitchNode Switch;
		tdefNode tdef;
		ternaryNode ternary;
		textNode text;
		udclNode udcl;
		unaryNode unary;		//一元操作符
		WhileNode While;
		/*********13**************--------------Define For SPL----------****************************/
		strdclNode		strdcl;
		comDeclNode		comdcl;
		compositeNode	composite;
		comInOutNode	comInOut;
		comBodyNode		comBody;
		paramNode		param;
		operBodyNode	operBody;
		operDeclNode	operdcl;	
		operatorNode	operator_;
		windowNode		window;
		slidingNode		sliding;
		tumblingNode	tumbling;
		uncertaintyNode uncertainty;
		/*-------7-------New For SPL----------*/
		comCallNode		comCall;
		PipelineNode	pipeline;
		SplitJoinNode	splitJoin;
		splitNode		split;
		joinNode		join;
		roundrobinNode	roundrobin;				
		duplicateNode	duplicate;
		/**********1*****新文法*******************/
		addNode			add;		//add 节点
	} u;
};


/* 以下三个宏还不明确 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

          ASTSWITCH, ASTWALK, ListWALK


  These macros abstracts over a switch statement applied to 
  Node->typ, to simplify operations on the syntax tree.
  这些宏通过应用于Node-> typ的switch语句进行抽象，以简化语法树上的操作。

ASTSWITCH
  Parameters:
      Node    abstract syntax tree Node

      CODE    #defined macro taking three parameters:

                   name       enumerated constant corresponding to Node->typ
		   Node       same as Node passed to ASTSWITCH
		   union      pointer to field in Node->u appropriate for
		                 this type of Node


  ASTSWITCH expands into a big switch statement.  The code after each
  case of the switch statement is CODE(name, Node, union), followed by
  a break.  (Thus CODE does not need to include a break statement.)
  The default case of the switch statement, which is executed when the
  Node type is invalid, is an assertion failure.
  ASTSWITCH扩展为一个重要的转换声明。 switch语句的每个case之后的代码是CODE
  （name，Node，union），然后是break。 （因此CODE不需要包含break语句。）switch
  语句的默认情况（在节点类型无效时执行）是断言失败。

  Typically, CODE will concatenate its name parameter with a prefix to create
  the name of a method function, then call that function with some parameters.
  Examples may be found in SemCheckNode (sem-check.c) and PrintNode 
  (print-ast.c).
  通常，CODE会将其name参数与前缀连接起来以创建方法函数的名称，然后使用某些参数调用
  该函数。 示例可以在SemCheckNode（sem-check.c）和PrintNode（print-ast.c）中找到。


ASTWALK
  Parameters:
      Node    abstract syntax tree Node

      CODE    #defined macro taking one parameter, which is a child
              pointer of <Node>.  The parameter may be used in an 
	      assignment to change the child pointer.


  ASTWALK expands CODE once for each non-nil child of <Node>.  
  (Child pointers which are NULL will not be seen by CODE.)
  It also walks over child Lists of <Node>, calling CODE on each
  member.

  Typically CODE will make a recursive call on each child Node.
  For examples, see NodeCopy and DiscardCoords (ast.c).



ListWALK
  Parameters:
      l       List pointer

      CODE    #defined macro taking one parameter, which is a
              member of List l.  The parameter may be used in an
              assignment to change the member of the List.


  ListWALK executes CODE  for each non-nil member of <l>.
  For examples, see the definition of ASTWALK below.



* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define ASTSWITCH(n, CODE)  \
switch (n->typ) { \
 case Const:         CODE(Const, n, &n->u.Const); break; \
 case Id:            CODE(Id, n, &n->u.id); break; \
 case Binop:         CODE(Binop, n, &n->u.binop); break; \
 case Unary:         CODE(Unary, n, &n->u.unary); break; \
 case Cast:          CODE(Cast, n, &n->u.cast); break; \
 case Comma:         CODE(Comma, n, &n->u.comma); break; \
 case Ternary:       CODE(Ternary, n, &n->u.ternary); break; \
 case Array:         CODE(Array, n, &n->u.array); break; \
 case Call:          CODE(Call, n, &n->u.call); break; \
 case Initializer:   CODE(Initializer, n, &n->u.initializer); break; \
 case ImplicitCast:  CODE(ImplicitCast, n, &n->u.implicitcast); break; \
 case Label:         CODE(Label, n, &n->u.label); break; \
 case Switch:        CODE(Switch, n, &n->u.Switch); break; \
 case Case:          CODE(Case, n, &n->u.Case); break; \
 case Default:       CODE(Default, n, &n->u.Default); break; \
 case If:            CODE(If, n, &n->u.If); break; \
 case IfElse:        CODE(IfElse, n, &n->u.IfElse); break; \
 case While:         CODE(While, n, &n->u.While); break; \
 case Do:            CODE(Do, n, &n->u.Do); break; \
 case For:           CODE(For, n, &n->u.For); break; \
 case Goto:          CODE(Goto, n, &n->u.Goto); break; \
 case Continue:      CODE(Continue, n, &n->u.Continue); break; \
 case Break:         CODE(Break, n, &n->u.Break); break; \
 case Return:        CODE(Return, n, &n->u.Return); break; \
 case Block:         CODE(Block, n, &n->u.Block); break; \
 case Prim:          CODE(Prim, n, &n->u.prim); break; \
 case Tdef:          CODE(Tdef, n, &n->u.tdef); break; \
 case Ptr:           CODE(Ptr, n, &n->u.ptr); break; \
 case Adcl:          CODE(Adcl, n, &n->u.adcl); break; \
 case Fdcl:          CODE(Fdcl, n, &n->u.fdcl); break; \
 case Sdcl:          CODE(Sdcl, n, &n->u.sdcl); break; \
 case Udcl:          CODE(Udcl, n, &n->u.udcl); break; \
 case Edcl:          CODE(Edcl, n, &n->u.edcl); break; \
 case Decl:          CODE(Decl, n, &n->u.decl); break; \
 case Attrib:		 CODE(Attrib, n, &n->u.attrib); break; \
 case Proc:          CODE(Proc, n, &n->u.proc); break; \
 case Text:          CODE(Text, n, &n->u.text); break; \
 /****************13******--------------Define For SPL----------****************************/ \
 case STRdcl:        CODE(STRdcl, n, &n->u.strdcl); break; \
 case Comdcl:        CODE(Comdcl, n, &n->u.comdcl); break; \
 case Composite:     CODE(Composite, n, &n->u.composite); break; \
 case ComInOut:      CODE(ComInOut, n, &n->u.comInOut); break; \
 case ComBody:       CODE(ComBody, n, &n->u.comBody); break; \
 case Param:         CODE(Param, n, &n->u.param); break; \
 case OperBody:      CODE(OperBody, n, &n->u.operBody); break; \
 case Operdcl:       CODE(Operdcl, n, &n->u.operdcl); break; \
 case Operator_:     CODE(Operator_, n, &n->u.operator_); break; \
 case Window:        CODE(Window, n, &n->u.window); break; \
 case Sliding:       CODE(Sliding, n, &n->u.sliding); break; \
 case Tumbling:      CODE(Tumbling, n, &n->u.tumbling); break; \
 /*---------7-----New For SPL----------*/\
 case CompositeCall: CODE(CompositeCall, n, &n->u.comCall); break; \
 case Pipeline:		 CODE(Pipeline, n, &n->u.pipeline); break; \
 case SplitJoin:	 CODE(SplitJoin, n, &n->u.splitJoin); break; \
 case Split:		 CODE(Split, n, &n->u.split); break; \
 case Join:			 CODE(Join, n, &n->u.join); break; \
 case RoundRobin:	 CODE(RoundRobin, n, &n->u.roundrobin); break; \
 case Duplicate:	 CODE(Duplicate, n, &n->u.duplicate); break; \
 case Uncertainty:	 CODE(Uncertainty, n, &n->u.uncertainty); break;\
 /**************新文法******2*************/\
 case Add:	 		CODE(Add, n, &n->u.add); break; \
 /***********************--------------Define For SPL----------****************************/ \
 default:            FAIL("unexpected Node type"); break; \
 }



#define ASTWALK(n, CODE)  \
switch ((n)->typ) { \
 case Const:         if ((n)->u.Const.type) {CODE((n)->u.Const.type);} break; \
 case Id:            break; \
 case Binop:         if ((n)->u.binop.left) {CODE((n)->u.binop.left);} if ((n)->u.binop.right) {CODE((n)->u.binop.right);} break; \
 case Unary:         if ((n)->u.unary.expr) {CODE((n)->u.unary.expr);} break; \
 case Cast:          if ((n)->u.cast.type) {CODE((n)->u.cast.type);} if ((n)->u.cast.expr) {CODE((n)->u.cast.expr);} break; \
 case Comma:         if ((n)->u.comma.exprs) {ListWALK((n)->u.comma.exprs, CODE);} break; \
 case Ternary:       if ((n)->u.ternary.cond) {CODE((n)->u.ternary.cond);} if ((n)->u.ternary.true_) {CODE((n)->u.ternary.true_);} if ((n)->u.ternary.false_) {CODE((n)->u.ternary.false_);} break; \
 case Array:         if ((n)->u.array.name) {CODE((n)->u.array.name);} if ((n)->u.array.dims) {ListWALK((n)->u.array.dims, CODE);} break; \
 case Call:          if ((n)->u.call.name) {CODE((n)->u.call.name);} if ((n)->u.call.args) {ListWALK((n)->u.call.args, CODE);} break; \
 case Initializer:   if ((n)->u.initializer.exprs) {ListWALK((n)->u.initializer.exprs, CODE);} break; \
 case ImplicitCast:  if ((n)->u.implicitcast.expr) {CODE((n)->u.implicitcast.expr);} break; \
 case Label:         if ((n)->u.label.stmt) {CODE((n)->u.label.stmt);} break; \
 case Switch:        if ((n)->u.Switch.expr) {CODE((n)->u.Switch.expr);} if ((n)->u.Switch.stmt) {CODE((n)->u.Switch.stmt);} break; \
 case Case:          if ((n)->u.Case.expr) {CODE((n)->u.Case.expr);} if ((n)->u.Case.stmt) {CODE((n)->u.Case.stmt);} break; \
 case Default:       if ((n)->u.Default.stmt) {CODE((n)->u.Default.stmt);} break; \
 case If:            if ((n)->u.If.expr) {CODE((n)->u.If.expr);} if ((n)->u.If.stmt) {CODE((n)->u.If.stmt);} break; \
 case IfElse:        if ((n)->u.IfElse.expr) {CODE((n)->u.IfElse.expr);} if ((n)->u.IfElse.true_) {CODE((n)->u.IfElse.true_);} if ((n)->u.IfElse.false_) {CODE((n)->u.IfElse.false_);} break; \
 case While:         if ((n)->u.While.expr) {CODE((n)->u.While.expr);} if ((n)->u.While.stmt) {CODE((n)->u.While.stmt);} break; \
 case Do:            if ((n)->u.Do.stmt) {CODE((n)->u.Do.stmt);} if ((n)->u.Do.expr) {CODE((n)->u.Do.expr);} break; \
 case For:           if ((n)->u.For.init) {CODE((n)->u.For.init);} if ((n)->u.For.cond) {CODE((n)->u.For.cond);} if ((n)->u.For.next) {CODE((n)->u.For.next);} if ((n)->u.For.stmt) {CODE((n)->u.For.stmt);} break; \
 case Goto:          break; \
 case Continue:      break; \
 case Break:         break; \
 case Return:        if ((n)->u.Return.expr) {CODE((n)->u.Return.expr);} break; \
 case Block:         if ((n)->u.Block.decl) {ListWALK((n)->u.Block.decl, CODE);} if ((n)->u.Block.stmts) {ListWALK((n)->u.Block.stmts, CODE);} break; \
 case Prim:          break; \
 case Tdef:          break; \
 case Ptr:           if ((n)->u.ptr.type) {CODE((n)->u.ptr.type);} break; \
 case Adcl:          if ((n)->u.adcl.type) {CODE((n)->u.adcl.type);} if ((n)->u.adcl.dim) {CODE((n)->u.adcl.dim);} break; \
 case Fdcl:          if ((n)->u.fdcl.args) {ListWALK((n)->u.fdcl.args, CODE);} if ((n)->u.fdcl.returns) {CODE((n)->u.fdcl.returns);} break; \
 case Sdcl:          if (SUE_ELABORATED((n)->u.sdcl.tq) && (n)->u.sdcl.type->fields) {ListWALK((n)->u.sdcl.type->fields, CODE);} break; \
 case Udcl:          if (SUE_ELABORATED((n)->u.udcl.tq) && (n)->u.udcl.type->fields) {ListWALK((n)->u.udcl.type->fields, CODE);} break; \
 case Edcl:          if (SUE_ELABORATED((n)->u.edcl.tq) && (n)->u.edcl.type->fields) {ListWALK((n)->u.edcl.type->fields, CODE);} break; \
 case Decl:          if ((n)->u.decl.type) {CODE((n)->u.decl.type);} if ((n)->u.decl.init) {CODE((n)->u.decl.init);} if ((n)->u.decl.bitsize) {CODE((n)->u.decl.bitsize);} break; \
 case Attrib:     if (n->u.attrib.arg) {CODE(n->u.attrib.arg);} break; \
 case Proc:          if ((n)->u.proc.decl) {CODE((n)->u.proc.decl);} if ((n)->u.proc.body) {CODE((n)->u.proc.body);} break; \
 case Text:          break; \
 /************13***********--------------zww:Define For SPL----------****************************/ \
 case STRdcl:        if (STREAM_ELABORATED((n)->u.strdcl.tq) && (n)->u.strdcl.type->fields) {ListWALK((n)->u.strdcl.type->fields, CODE);} break;\
 case Comdcl:        if ((n)->u.comdcl.inout){CODE((n)->u.comdcl.inout);}break; \
 case Composite:     if ((n)->u.composite.decl){CODE((n)->u.composite.decl);}if ((n)->u.composite.body){CODE((n)->u.composite.body);} break;\
 case ComInOut:      if ((n)->u.comInOut.inputs){ListWALK((n)->u.comInOut.inputs,CODE);}if ((n)->u.comInOut.outputs){ListWALK((n)->u.comInOut.outputs,CODE);} break; \
 case ComBody:       if ((n)->u.comBody.param){CODE((n)->u.comBody.param);}  if ((n)->u.comBody.decl){ListWALK((n)->u.comBody.decl,CODE);} if((n)->u.comBody.comstmts){ListWALK((n)->u.comBody.comstmts ,CODE);} break;\
 case Param:         if ((n)->u.param.parameters){ListWALK((n)->u.param.parameters ,CODE);} break;\
 case OperBody:      if ((n)->u.operBody.state){ListWALK((n)->u.operBody.state ,CODE);}if ((n)->u.operBody.init) {CODE((n)->u.operBody.init);} if ((n)->u.operBody.work) {CODE((n)->u.operBody.work);} if ((n)->u.operBody.window){ListWALK((n)->u.operBody.window ,CODE);} break;\
 case Operdcl:       if ((n)->u.operdcl.inputs){ListWALK((n)->u.operdcl.inputs ,CODE);}if ((n)->u.operdcl.outputs){ListWALK((n)->u.operdcl.outputs,CODE);}if ((n)->u.operdcl.arguments){ListWALK((n)->u.operdcl.arguments ,CODE);}break; \
 case Operator_:     if ((n)->u.operator_.decl) {CODE((n)->u.operator_.decl);} if ((n)->u.operator_.body) {CODE((n)->u.operator_.body);}break; \
 case Window:        if ((n)->u.window.id) {CODE((n)->u.window.id);} if ((n)->u.window.wtype){CODE((n)->u.window.wtype);} break; \
 case Sliding:       if ((n)->u.sliding.sliding_value) {CODE((n)->u.sliding.sliding_value);} break; \
 case Tumbling:      if ((n)->u.tumbling.tumbling_value) {CODE((n)->u.tumbling.tumbling_value);} break; \
 /*-------7-------New For SPL----------*/\
 case CompositeCall: if ((n)->u.comCall.call) {CODE((n)->u.comCall.call);} if ((n)->u.comCall.operdcl) {CODE((n)->u.comCall.operdcl);}break;\
 case Pipeline:		 if ((n)->u.pipeline.output) {CODE((n)->u.pipeline.output);} if ((n)->u.pipeline.input) {CODE((n)->u.pipeline.input);} if ((n)->u.pipeline.decl){ListWALK((n)->u.pipeline.decl ,CODE);} if ((n)->u.pipeline.stmts){ListWALK((n)->u.pipeline.stmts ,CODE);}break;\
 case SplitJoin:	 if ((n)->u.splitJoin.output) {CODE((n)->u.splitJoin.output);}if ((n)->u.splitJoin.input) {CODE((n)->u.splitJoin.input);}if ((n)->u.splitJoin.decl){ListWALK((n)->u.splitJoin.decl ,CODE);}if ((n)->u.splitJoin.initstmts){ListWALK((n)->u.splitJoin.initstmts ,CODE);}if ((n)->u.splitJoin.split) {CODE((n)->u.splitJoin.split);}if ((n)->u.splitJoin.stmts){ListWALK((n)->u.splitJoin.stmts ,CODE);}if ((n)->u.splitJoin.join) {CODE((n)->u.splitJoin.join);} break; \
 case Split:		 if ((n)->u.split.type) {CODE((n)->u.split.type);} break; \
 case Join:			 if ((n)->u.join.type) {CODE((n)->u.join.type);} break; \
 case RoundRobin:	 if ((n)->u.roundrobin.arguments){ListWALK((n)->u.roundrobin.arguments ,CODE);}break; \
 case Duplicate:	 if ((n)->u.duplicate.expr) {CODE((n)->u.duplicate.expr);} break; \
 case Uncertainty:   break;\
 /***********1***新文法***************/\
 case Add:	 		if ((n)->u.add.content){CODE((n)->u.add.content);} break;\
 /***********************--------------zww:Define For SPL----------****************************/ \
 default:            FAIL("Unrecognized Node type"); break; \
 }


#define ListWALK(l, CODE)  \
{ ListMarker _Listwalk_marker; Node *_Listwalk_ref; \
  IterateList(&_Listwalk_marker, l); \
  while (NextOnList(&_Listwalk_marker, (GenericREF)&_Listwalk_ref)) { \
     if (_Listwalk_ref) {CODE(_Listwalk_ref);}                     \
     SetCurrentOnList(&_Listwalk_marker, (Generic *)_Listwalk_ref); \
  }\
}

#endif
