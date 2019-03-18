#ifndef __MAIN_H__
#define __MAIN_H__ 1

/*
  Example 3-5. Advanced calculator header fb3-2.h
  Declarations for a calculator fb3-1
*/
/*
  Example 9-2. Reentrant calc header file purecalc.h
*/

#ifdef _MSC_VER
  /* strdup 경고 없애기 */
  #define strdup _strdup
#endif

extern bool debug_flag;
extern bool l_debug_flag;
extern bool y_debug_flag;

/* interface to the lexer */
void yyerror(struct pcdata *pp, const char *s, ...);

/*
  * Declarations for a calculator, pure version
*/

/* per-parse data */ 

struct pcdata {
  void *scaninfo;        /* scanner context */
  struct symbol *symtab;    /* symbols for this parse */
  struct ast *ast;          /* most recently parsed AST */
};

/* symbol table */
struct symbol {
  char *name;                         /* a variable name */
  double value;
  struct ast *func;                   /* stmt for the function */
  struct symlist *syms;               /* list of dummy args */
};

/* simple symtab of fixed size */
#define NHASH 9997

struct symbol *lookup(struct pcdata *pp, char *);

/* list of symbols, for an argument list */
struct symlist {
  struct symbol *sym;
  struct symlist *next;
};

struct symlist *newsymlist(struct pcdata *pp, struct symbol *sym, struct symlist *next);
void symlistfree(struct pcdata *pp, struct symlist *sl);

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

struct ast {
  int nodetype;
  struct ast *l;
  struct ast *r;
};

/* built-in function */
struct fncall {
  int nodetype; /* type F */
  struct ast *l;
  enum bifs functype;
};

/* user function */
struct ufncall {
  int nodetype;  /* type C */
  struct ast *l; /* list of arguments */
  struct symbol *s;
};

struct flow {
  int nodetype;     /* type I or W */
  struct ast *cond; /* condition */
  struct ast *tl;   /* then branch or do list */
  struct ast *el;   /* optional else branch */
};

struct numval {
  int nodetype; /* type K */
  double number;
};

struct symref {
  int nodetype; /* type N */
  struct symbol *s;
};

struct symasgn {
  int nodetype; /* type = */
  struct symbol *s;
  struct ast *v; /* value */
};

/* build an AST */
struct ast *newast(struct pcdata *pp, int nodetype, struct ast *l, struct ast *r);
struct ast *newcmp(struct pcdata *pp, int cmptype, struct ast *l, struct ast *r);
struct ast *newfunc(struct pcdata *pp, int functype, struct ast *l);
struct ast *newcall(struct pcdata *pp, struct symbol *s, struct ast *l);
struct ast *newref(struct pcdata *pp, struct symbol *s);
struct ast *newasgn(struct pcdata *pp, struct symbol *s, struct ast *v);
struct ast *newnum(struct pcdata *pp, double d);
struct ast *newflow(struct pcdata *pp, int nodetype, struct ast *cond, struct ast *tl, struct ast *tr);

/* define a function */ 
void dodef(struct pcdata *pp, struct symbol *name, struct symlist *syms, struct ast *stmts);

/* evaluate an AST */ 
double eval(struct pcdata *pp, struct ast *);

/* delete and free an AST */ 
void treefree(struct pcdata *pp, struct ast *);

#endif