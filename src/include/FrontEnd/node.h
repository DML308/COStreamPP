#ifndef _NODE_H_
#define _NODE_H_
#include "basics.h"
#include "list.h"
#include "dataflow.h"
#include "nodetype.h"
typedef enum
{
    /* expression nodes */
    Const,
    Id
} NodeType;

typedef struct
{
    int first_line;
    int first_column;
    int last_line;
    int last_column;
} Loc;

class Node
{
  public:
    NodeType type;
    Loc *loc;
    short pass;
	/* parenthesized is set on expressions which were parenthesized
	   in the original source:  e.g., (x+y)*(w+z) would have
	   parenthesized==TRUE on both PLUS nodes, and parenthesized==FALSE
	   on both MULT nodes. */
	short parenthesized;
    Node()  { loc = new Loc; }
    virtual ~Node() { delete loc; }
    void setLoc(Loc *loc);
    virtual void print();
};

class ConstNode:public Node {
    public:
        const char* text;
        /* Node类型没定义，只能使用向前声明，只能定义为指针或者引用 */
        Node *type;
        union{
            int             i;
            unsigned int    u;
            long            l;
            unsigned long   ul;
            float           f;
            double          d;
            const char *    s;
        }value;
        ConstNode() {}
        ~ConstNode() {}
        void print();

};

class idNode:public Node{
    public:
        const char * text;
        Node *decl;
        Node *value;
        idNode(){}
        ~idNode() {}
        void print();

};

class binopNode:public Node{
    public:
        OpType op;
	    opDataType opType;/*zww:define for spl,记录该2元操作的数据类型：整型，浮点型*/
	    Node *left;
	    Node *right;
	    Node *type;
	    Node *value;
	    List *valueList;/*zww:对结构体常量传播的dot操作*/
        binopNode() {}
        ~binopNode() {}
        void print();
};

class unaryNode:public Node{
    public:
        OpType op;
        opDataType opType;/*zww:define for spl,记录该2元操作的数据类型：整型，浮点型*/
	    Node *expr;
	    Node *type;
	    Node *value;
        unaryNode() {   }
        ~unaryNode() {  }
        void print();
};

class castNode:public Node{
    public:
        Node *type;
        Node *expr;
        Node *value;
        castNode() {    }
        ~castNode() {   }
        void print();
};

class commaNode:public Node{
    public:
        List *exprs;
        commaNode() {   }
        ~commaNode() {  }
        void print();
};

/*三元操作符节点 */
class ternaryNode:public Node{
    public:
        Node *cond;
        Node *true_;
        Node *false_;
        /* Coord colon_coord; */   /* 这行被我注释了 */
        Node *type;
        Node *value;
        ternaryNode()  {    }
        ~ternaryNode() {    }
        void print();
};

class arrayNode:public Node{
    public:
        Node *type;
        Node *name;
        List *dims;
        Node *value;
        arrayNode() {   }
        ~arrayNode(){   }
        void print();
};

class callNode:public Node{
    public:
        Node *name;
        List *args;
        callNode() {    }
        ~callNode(){    }
        void print();
};

class initializerNode:public Node{
    public:
        List *exprs;
        initializerNode() { }
        ~initializerNode(){ }
        void print();
};

class implicitcastNode:public Node{
    public:
        Node *exprs;
        Node *type;
        Node *value;
        implicitcastNode() {    }
        ~implicitcastNode(){    }
        void print();
};

/*************************************************************************/
/*                                                                       */
/*                          Statement nodes                              */
/*                                                                       */
/*************************************************************************/
class labelNode:public Node{
    public:
        const char *name;
        Node *stmt;
        List *references;
        FlowValue label_values;
        labelNode() {   }
        ~labelNode(){   }
        void print();
};

class switchNode:public Node{
    public:
        Node *exprs;
        Node *stmt;
        List *cases;
        bool has_default;   /* true_ if cases includes a Default node */
        struct SwitchCheck *check;      /*这里switchcheck 还未定义 */
        FlowValue swithc_values,braek_values;
        switchNode() {  }
        ~switchNode(){  }
        void print();
};

class caseNode:public Node{
    public:
        Node *expr;
        Node *stmt;
        Node *container;
        caseNode() {    }
        ~caseNode(){    }
        void print();
};

class defaultNode:public Node{
    public:
        Node *stmt;
        Node *container;
};

class ifNode:public Node{
    public:
        Node *expr;
        Node *stmt;
        ifNode() {  }
        ~ifNode(){  }
        void print();
};

class ifelseNode:public Node{
    public:
        Node *expr;
        Node *ture_;
        Node *false_;
       /* coord else_coord */ /*coordinates of ELSE keyword*/
        ifelseNode() {}
        ~ifelseNode(){}
        void print();
};

class whileNode:public Node{
    public:
        Node *expr;
        Node *stmt;
        FlowValue loop_values,break_values; /*  此处loop_values没必要保存？*/
        whileNode() {   }
        ~whileNode(){   }
        void print();
};

class doNode:public Node{
    public:
        Node *stmt;
	    Node *expr;
	    /* Coord while_coord;  */   /* coordinates of WHILE keyword */
	    FlowValue loop_values, continue_values, break_values;
        doNode() {  }
        ~doNode(){  }
        void print();
};

class forNode:public Node{
    public:
        Node *init;
	    Node *cond;
	    Node *next;
	    Node *stmt;
	    FlowValue loop_values, continue_values, break_values;
        forNode() { }
        ~forNode(){ }
        void print();
};

/*  删除了对gotoNode的支持 */
class continueNode :public Node{
    public:
        Node *container;
        continueNode() {    }
        ~continueNode(){    }
        void print();
};

class breakNode:public Node{
    public:
        Node *container;
        breakNode() {}
        ~breakNode(){}
        void print();
};

class returnNode:public Node{
    public:
        Node *expr;
        Node *proc;
        returnNode() {  }
        ~returnNode(){  }
        void print();
};

class blockNode:public Node{
    public:
        List *decl;
        List *stmts;
        /*coord right_coord;*/ /*coordinates of right brace  */
        Node *type;
        blockNode() {}
        ~blockNode() {}
        void print();
};

/*************************************************************************/
/*                                                                       */
/*                          Type nodes                                   */
/*                                                                       */
/*************************************************************************/
class primNode:public Node{
    public:
        TypeQual tq;
	    BasicType basic;
        primNode() {    }
        ~primNode(){    }
        void print();
};

/* 删除了tdefnode、ptrnode sdclNode,udclnode,edclnode*/
class adclNode:public Node{
    public:
        TypeQual tq;
        Node *type;
        Node *dim;
	    int   size;
        adclNode() {    }
        ~adclNode() {    }
        void print();
};

class fdclNode:public Node{
    public:
        TypeQual tq;
	    List *args;
	    Node *returns;  /* 函数返回值 */
        fdclNode() {    }
        ~fdclNode(){    }
        void print();
};

/*************************************************************************/
/*                                                                       */
/*                          Other nodes                                  */
/*                                                                       */
/*************************************************************************/
class declNode:public Node{
    public:
        const char* name;
	    TypeQual tq;       /* storage class and decl qualifiers */
	    Node *type;
	    Node *init;    /* in other versions of c-parser, init is
			       overloaded to be the offset for structs --
			       but NOT in c-to-c */
	    Node *prim_init;    /* ：为了保留数组定义是初始化列表 12.2.10 */
	    Node *bitsize;
	    int  references;    /* number of references to declared name */
	    List *attribs;      /* GCC __attribute__ declarations */
        declNode() {    }
        ~declNode(){    }
        void print();
};

/*  删除了atrributeNode */

class procNode:public Node{
    public:
        Node *decl;
	    Node *body;
	    FlowValue init_values;//zww:SPL
	    FlowValue return_values;
        procNode() {    }
        ~procNode(){    }
        void print();
};

class textNode:public Node{
    public:
        const char*text;    /* may be NULL (treated same as "") */
        bool start_new_line;/* true_ ==> nothing but whitespace should appear
                            before <text> on line */
        textNode() {    }
        ~textNode(){    }
        void print();
};


/***********************Define Nodes For SPL----begin*********************/

/*************************************************************************/
/*                                                                       */
/*                      SPL Type Nodes                                   */
/*                                                                       */
/*************************************************************************/

class strdclNode:public Node{
    public:
        TypeQual tq;
	    StreamType *type; /* this is a child link iff S_ELABORATED(tq) */
        strdclNode() {   }
        ~strdclNode(){   }
        void print();
};

class comDeclNode:public Node{
    public:
        TypeQual tq;
	    Node *inout;
        comDeclNode()   {   }
        ~comDeclNode()  {   }
        void print();
};

class compositeNode:public Node{
    public:
        Node *decl;  /*type->comInoutNode,name->id.name*/
	    Node *body;
	    bool multi;
	    FlowValue composite_values;/*for data flow among composites*/
        compositeNode() {   }
        ~compositeNode(){   }
        void print();
};

class comInOutNode:public Node{
    public:
        TypeQual tq;
	    List *outputs;
	    List *inputs;
        comInOutNode() {    }
        ~comInOutNode(){    }
        void print();
};

class comBodyNode:public Node{
    public:
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

	    /* Coord left_coord; */
	    /* Coord right_coord; */  /* coordinates of right brace */
	    Node *type;  /* the type of a {...} block is void;
				 the type of a ({..}) block is the type of its last
				 statement (initially NULL, then filled in by
				 SemCheckNode) */
        comBodyNode() { }
        ~comBodyNode(){ }
        void print();
};

class paramNode:public Node{
    public:
        List *parameters;
        paramNode() {   }
        ~paramNode(){   }
        void print();
};

class operBodyNode:public Node{
    public:
        List *state;  /*declaration.list  改动：无state关键字，其余结构不变*/
	    Node *init;   /*compound.statement*/
	    Node *work; /*compound.statement assert(work != null),kernel*/
	    List *window; /*if window == null, its peek=pop=1;*/
	    /*Coord left_coord;*/
	    /*Coord right_coord;*/ /* coordinates of right brace */
	    Node *type;  /* the type of a {...} block is void;
				 the type of a ({..}) block is the type of its last
				 statement (initially NULL, then filled in by
				 SemCheckNode) */
        operBodyNode() {    }
        ~operBodyNode(){    }
        void print();

};

class operDeclNode:public Node{
    public:
        TypeQual tq;
	    List *outputs;/*stream.type.specifier identifier.list | identifier.list*/
	    List *inputs;/*identifier.list*/
	    List *arguments;/*argument.expression.list*/
        operDeclNode() {    }
        ~operDeclNode(){    }
        void print();
};

class operatorNode:public Node{
    public:
        Node *decl;  /*type->streamif.operator | splitjoin.operator | pipeline.operator | operator.invoke | operator.stream.define */
	    Node *body;  /*body != null, it is a self-define operator*/
	    FlowValue oper_values;
	    OperatorType ot;
        operatorNode()  {   }
        ~operatorNode() {   }
        void print();
};

class windowNode:public Node{
    public:
        Node *id;
	    Node *wtype;
        windowNode() {  }
        ~windowNode(){  }
        void print();
};

class slidingNode:public Node{
    public:
        TypeQual tq;
	    Node *sliding_value;
        slidingNode() { }
        ~slidingNode(){ }
        void print();
};

class tumblingNode:public Node{
    public:
        TypeQual tq;
	    Node *tumbling_value;
        tumblingNode() {    }
        ~tumblingNode(){    }
        void print();
};

class uncertaintyNode:public Node{
    public:
        TypeQual tq;
	    Node*uncertainty_value;
        uncertaintyNode() { }
        ~uncertaintyNode(){ }
        void print();
};

class comCallNode:public Node{
    public:
        Node *call;/*call->operDeclNode*/
	    Node *operdcl;
	    bool style;/*if style == true_, it is a SPL style call, else it is a StreamIt style call*/
	    Node *actual_composite;// zww：在指向深拷贝实际展开后的composite
        comCallNode()  {    }
        ~comCallNode() {    }
        void print();
};

class PipelineNode:public Node{
    public:
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
        PipelineNode() {    }
        ~PipelineNode(){    }
        void print();
};

class SplitJoinNode:public Node{
    public:
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
        SplitJoinNode() {   }
        ~SplitJoinNode(){   }
        void print();
};

class splitNode:public Node{
    public:
        Node *type;
        splitNode() {   }
        ~splitNode() {  }
        void print();
};

class joinNode:public Node{
    public:
        Node *type;
        joinNode() {    }
        ~joinNode(){    }
        void print();
};

class roundrobinNode:public Node{
    public:
        List *arguments;
        roundrobinNode()    {   }
        ~roundrobinNode()   {   }
        void print();
};

class duplicateNode:public Node{
    public:
        Node *expr;
        duplicateNode()    {   }
        ~duplicateNode()   {   }
        void print();
};

class addNode:public Node{
    public:
        Node *content;
        addNode()   {   }
        ~addNode()  {   }
        void print();
};


/*************************************************************************/
/*                                                                       */
/*                            Node definition                            */
/*                                                                       */
/*************************************************************************/


struct nodeStruct {
	NodeType typ;
    /* 暂时注释掉coord */
	/* Coord coord; */ /* location of first terminal in production */

	/* pass is incremented for each top-level call to PrintNode, so
	   that PrintNode can print out decls anywhere they are used
	   without infinite recursion on recursive data structures. */

    /* pass 表示Node嵌套节点深度 ; */
	short pass;
	/* parenthesized is set on expressions which were parenthesized
	   in the original source:  e.g., (x+y)*(w+z) would have
	   parenthesized==TRUE on both PLUS nodes, and parenthesized==FALSE
	   on both MULT nodes. */
	short parenthesized;
    /*  暂时注释掉 */
	/* data-flow analysis information */
	//Analysis analysis;
	union {
		adclNode adcl;
		arrayNode array;
		binopNode binop;	//二元操作符
		blockNode Block;
		breakNode Break;
		callNode call;		/* 函数调用 */
		caseNode Case;
		castNode cast;
		commaNode comma;
		ConstNode Const;
		continueNode Continue;
		declNode decl;
		defaultNode Default;
		doNode Do;
		fdclNode fdcl;
		forNode For;
		idNode id;
		ifNode If;
		ifelseNode IfElse;
		implicitcastNode implicitcast;
		initializerNode initializer;
		labelNode label;
		primNode prim;
		procNode proc;
		returnNode Return;
		switchNode Switch;
		ternaryNode ternary;
		textNode text;
		unaryNode unary;		//一元操作符
		whileNode While;
		//********13**************--------------Define For SPL----------****************************
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
		//-------7-------New For SPL----------
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


#endif
