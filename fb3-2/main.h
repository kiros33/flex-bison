#ifndef __MAIN_H__
#define __MAIN_H__ 1

/*
  Example 3-5. Advanced calculator header fb3-2.h
  Declarations for a calculator fb3-1
*/

#ifdef _MSC_VER
  /* strdup 경고 없애기 */
  #define strdup _strdup
#endif

extern bool debug_flag;
extern bool l_debug_flag;
extern bool y_debug_flag;

/* interface to the lexer */
extern int yylineno; /* from lexer */
void yyerror(const char *s, ...);

/* symbol table */
struct symbol {
  char *name;                         /* a variable name */
  double value;
  struct ast *func;                   /* stmt for the function */
  struct symlist *syms;               /* list of dummy args */
};

/* simple symtab of fixed size */
#define NHASH 9997

struct symbol *lookup(char *);

/* list of symbols, for an argument list */
struct symlist {
  struct symbol *sym;
  struct symlist *next;
};

struct symlist *newsymlist(struct symbol *sym, struct symlist *next);
void symlistfree(struct symlist *sl);

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
struct ast *newast(int nodetype, struct ast *l, struct ast *r);
struct ast *newcmp(int cmptype, struct ast *l, struct ast *r);
struct ast *newfunc(int functype, struct ast *l);
struct ast *newcall(struct symbol *s, struct ast *l);
struct ast *newref(struct symbol *s);
struct ast *newasgn(struct symbol *s, struct ast *v);
struct ast *newnum(double d);
struct ast *newflow(int nodetype, struct ast *cond, struct ast *tl, struct ast *tr);

/* define a function */ 
void dodef(struct symbol *name, struct symlist *syms, struct ast *stmts);

/* evaluate an AST */ 
double eval(struct ast *);

/* delete and free an AST */ 
void treefree(struct ast *);

#endif