#ifndef __MAIN_H__
#define __MAIN_H__ 1

/*
  Example 3-5. Advanced calculator header fb3-2.h
  Declarations for a calculator fb3-1
*/

extern bool debug_flag;

/* interface to the lexer */
extern int yylineno; /* from lexer */
void yyerror(const char *s, ...);

typedef struct _ast AbstractSyntaxTree, *pAbstractSyntaxTree;
typedef struct _symbol Symbol, *pSymbol;
typedef struct _symlist SymbolList, *pSymbolList;

typedef struct _fncall IntrinsicFunction, *pIntrinsicFunction;
typedef struct _ufncall UserFunction, *pUserFunction;
typedef struct _flow FlowControl, *pFlowControl;
typedef struct _numval NumberValue, *pNumberValue;
typedef struct _symref SymbolRef, *pSymbolRef;
typedef struct _symasgn SymbolAssign, *pSymbolAssign;

/* symbol table */
struct _symbol {
  char *name;                         /* a variable name */
  double value;
  pAbstractSyntaxTree func;                   /* stmt for the function */
  pSymbolList syms;               /* list of dummy args */
};

/* simple symtab of fixed size */
#define NHASH 9997

pSymbol lookup(char *);

/* list of symbols, for an argument list */
struct _symlist {
  pSymbol sym;
  pSymbolList next;
};

pSymbolList newsymlist(pSymbol sym, pSymbolList next);
void symlistfree(pSymbolList sl);

/* node types
 * + - * / |
 * 0-7  comparison ops, bit coded 04 equal, 02 less, 01 greater
 * M    unary minus
 * L    expression or statement list
 * I    IF statement
 * W    WHILE statement
 * N    symbol ref
 * =    assignment
 * S    list of symbols
 * F    built in function call
 * C    user function call
 */

/* built-in functions */
enum bifs { B_sqrt = 1, B_exp, B_log, B_print };

/* nodes in the abstract syntax tree */
/* all have common initial nodetype */

struct _ast {
  int nodetype;
  pAbstractSyntaxTree l;
  pAbstractSyntaxTree r;
};

/* built-in function */
struct _fncall {
  int nodetype; /* type F */
  pAbstractSyntaxTree l;
  enum bifs functype;
};

/* user function */
struct _ufncall {
  int nodetype;  /* type C */
  pAbstractSyntaxTree l; /* list of arguments */
  pSymbol s;
};

struct _flow {
  int nodetype;     /* type I or W */
  pAbstractSyntaxTree cond; /* condition */
  pAbstractSyntaxTree tl;   /* then branch or do list */
  pAbstractSyntaxTree el;   /* optional else branch */
};

struct _numval {
  int nodetype; /* type K */
  double number;
};

struct _symref {
  int nodetype; /* type N */
  pSymbol s;
};

struct _symasgn {
  int nodetype; /* type = */
  pSymbol s;
  pAbstractSyntaxTree v; /* value */
};

/* build an AST */
pAbstractSyntaxTree newast(int nodetype, pAbstractSyntaxTree l, pAbstractSyntaxTree r);
pAbstractSyntaxTree newcmp(int cmptype, pAbstractSyntaxTree l, pAbstractSyntaxTree r);
pAbstractSyntaxTree newfunc(int functype, pAbstractSyntaxTree l);
pAbstractSyntaxTree newcall(pSymbol s, pAbstractSyntaxTree l);
pAbstractSyntaxTree newref(pSymbol s);
pAbstractSyntaxTree newasgn(pSymbol s, pAbstractSyntaxTree v);
pAbstractSyntaxTree newnum(double d);
pAbstractSyntaxTree newflow(int nodetype, pAbstractSyntaxTree cond, pAbstractSyntaxTree tl, pAbstractSyntaxTree tr);

/* define a function */ 
void dodef(pSymbol name, pSymbolList syms, pAbstractSyntaxTree stmts);

/* evaluate an AST */ 
double eval(pAbstractSyntaxTree );

/* delete and free an AST */ 
void treefree(pAbstractSyntaxTree );

#endif