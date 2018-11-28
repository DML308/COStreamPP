#ifndef _NODETYPE_H_
#define _NODETYPE_H_
/* definition of Node data structure */

typedef enum {
  /* expression nodes */
  Const, Id, Binop, Unary, Cast, Comma, Ternary, Array, Call, Initializer,
  ImplicitCast,

  /* statement nodes */
  Label, Switch, Case, Default, If, IfElse, While, Do, For, Goto, Continue, 
  Break, Return, Block, 
  
  /* type nodes */
  Prim, Tdef, Ptr, Adcl, Fdcl, Sdcl, Udcl, Edcl, 
 
  /* declaration node */
  Decl, 

  /* GCC __attribute__ extension */
  Attrib, 

  /* procedure def node */
  Proc,


  /* random text and preprocessor command node */
  Text,

  /*----------12----SPL node----------*/
  STRdcl,		/*stream SPL*/
  Comdcl,		/*composite SPL*/
  Composite,	/* composite def node SPL*/
  ComInOut,		/*composite SPL*/
  ComBody,		/*composite.block SPL*/
  Param,		/*composite.body.param SPL*/		 
  OperBody,		/*operator SPL*/	
  Operdcl,		/*operator SPL*/
  Operator_,	/* operator def node SPL*/
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
  修改过的节点： comBodyNode,PipelineNode,SplitJoinNode,slidingNode,tumblingNode
  新增加的节点： Add
  */
} NodeType;


/*************************************************************************/
/*                                                                       */
/*                      Type representations                             */
/*                                                                       */
/*************************************************************************/

/* TypeQual: storage classes, type qualifiers, and additional attributes */

typedef enum {

  EMPTY_TQ = 0,
/* 
   Declaration qualifiers.  A declaration can have multiple qualifiers,
   though some are mutually exclusive (like storage classes).
   During the parsing of a declaration, decl qualifiers are kept in 
   the tq field of its primitive type.  After the decl has been parsed,
   SetDeclType then moves the qualifiers to their proper place, the 
   Decl node.

   To add a new decl qualifier:
      1. Define a new symbol below, being careful that its bits do
         not conflict with existing decl and type qualifiers,
         since they can coexist in the same TypeQual variable.
      2. Insert the symbol in T_DECL_QUALS (below).
      3. If the qualifier is parsed in the source (rather than calculated),
         add a lexical token (c4.l) and a new production to storage.classes
         (ANSI-C.y).
      4. Add code to print out its name in TQtoText (type.c).
      5. Add merging logic to MergeTypeQuals (type.c), if necessary.
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
   Type qualifiers.  Multiple type qualifiers may apply to a type.  
   They may be associated with any primitive or complex type.  
   Some type qualifiers may be moved after parsing -- for instance,
   T_INLINE is moved to the top-level Fdcl it is describing.

   To add a new type qualifier:
      1. Define a new symbol below, being careful that its bits do
         not conflict with existing storage classes and type qualifiers,
         since they can coexist in the same TypeQual variable.
      2. Insert the symbol in T_TYPE_QUALS (below).
      3. Add a lexical token (c4.l) and a new production to either
         type.qualifiers or pointer.type.qualifiers (ANSI-C.y), depending 
         on whether the type qualifier is allowed only at the beginning
         of a type, or can appear after '*' like const and volatile.
      4. Add code to print out its name in TQtoText (type.c).
      5. Add its symbol to TQ_ALWAYS_COMPATIBLE (below) if an object with
         the type qualifier is always assignment-compatible with an object
	 without the type qualifier.
      6. Add merging logic to MergeTypeQuals (type.c), if necessary.
 */

    T_CONST    = 0x01000,   /* leave some room for new decl qualifiers */
    T_VOLATILE = 0x02000,
    T_INLINE   = 0x04000,
    T_SUE_ELABORATED  = 0x08000,   /* on an Sdcl/Udcl/Edcl, indicates 
				      whether SUE's field list appeared
				      at that point in the source */
    T_STREAM_ELABORATED  = 0x10000,   /* on an STRdcl/, indicates 
								 whether stream's field list appeared
								 at that point in the source SPL*/
#define SUE_ELABORATED(tq)   (((tq) & T_SUE_ELABORATED) != 0)
#define STREAM_ELABORATED(tq)   (((tq) & T_STREAM_ELABORATED) != 0)/*SPL*/
    /* Insert new type qualifiers here */
    
    T_TYPE_QUALS = (T_CONST | T_VOLATILE | T_INLINE | T_SUE_ELABORATED | T_STREAM_ELABORATED/*SPL*/)
#define TYPE_QUALS(tq)    ((tq) & T_TYPE_QUALS)
    

/* Type qualifiers listed in TQ_COMPATIBLE are ignored
   when checking two types for compatibility (weaker than strict equality). */
#define TQ_COMPATIBLE   (T_CONST | T_VOLATILE | T_INLINE | T_SUE_ELABORATED | T_STREAM_ELABORATED/*SPL*/)

} TypeQual;



/* BasicType covers all of the different fundamental types.
   New basic types should also be added to InitTypes(). */
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

While a BasicType is constructed during parsing, its high-order bits
(above the bits needed for the BasicType itself) are flags
representing type modifiers like signedness and length.  (Extension
languages may have additional flags for their own type modifiers.)
After the BasicType is finalized with FinishPrimType, these 
high-order bits are cleared and the BasicType is set to one of the
enumerated values above.  (Thus the final BasicType value is a small
integer, which will be faster in a switch statement than a bitfield
representation like Sshort == Short | Int.)


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


typedef struct {
  TypeSpecifier base;
  TypeSpecifier sign;
  TypeSpecifier length;
  /* INSERT EXTENSIONS HERE */ 
} ExplodedType;


#define BASIC2EXPLODED(bt, et)  \
   ((et).sign = (bt) & SignMask, (et).length = (bt) & LengthMask, \
  /* INSERT EXTENSIONS HERE */     \
    (et).base = (bt) & BasicTypeMask)

#define EXPLODED2BASIC(et, bt)  \
   ((bt) = (et).sign | (et).length | \
  /* INSERT EXTENSIONS HERE */     \
    (et).base)

typedef enum{
    S_Duplicate,
    S_RoundRobin
}SplitStyle;


/*************************************************************************/
/*                                                                       */
/*                      SPL Type Nodes                                   */
/*                                                                       */
/*************************************************************************/
typedef struct {
	NodeType typ;
	bool complete;  /* true_ if struct has been elaborated with field list */
	/* Coord coord;*/  /* location of definition (left '<') */
	/*Coord right_coord;*/  /* location of right '>' */

	bool visited;  /* this struct has been visited in size calculation */ 
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
typedef struct nodeStruct Node;
#endif
