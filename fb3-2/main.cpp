/*
  Example 3-8. Advanced calculator helper functions fb3-2func.c
  helper functions for fb3-2
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include "main.h"
#include "parser.h"

bool debug_flag = true;

int debug_console(const char *format, ...) {
  va_list arg;
  int done;

  if(debug_flag) {
    va_start(arg, format);
    done = vfprintf(stdout, format, arg);
    va_end(arg);
  }

  return done;
}

Symbol symtab[NHASH];

/* symbol table */
/* hash a symbol */
static unsigned int symhash(char *sym) {
  unsigned int hash = 0;
  unsigned c;

  while ((c = *sym++)) {
    hash = hash * 9 ^ c;
  }

  return hash;
}

pSymbol lookup(char *sym) {
  pSymbol sp = &symtab[symhash(sym) % NHASH];
  int scount = NHASH; /* how many have we looked at */

  while (--scount >= 0) {
    if (sp->name && !strcmp(sp->name, sym)) { return sp; }

    if (!sp->name) { /* new entry */
      sp->name = strdup(sym);
      sp->value = 0;
      sp->func = NULL;
      sp->syms = NULL;
      return sp;
    }

    if (++sp >= symtab + NHASH) sp = symtab; /* try the next entry */
  }
  yyerror("symbol table overflow\n");
  abort(); /* tried them all, table is full */
}

pAbstractSyntaxTree newast(int nodetype, pAbstractSyntaxTree l, pAbstractSyntaxTree r) {
  pAbstractSyntaxTree a = (pAbstractSyntaxTree)malloc(sizeof(AbstractSyntaxTree));

  if (!a) {
    yyerror("out of space");
    exit(0);
  }

  a->nodetype = nodetype;
  a->l = l;
  a->r = r;

  return a;
}

pAbstractSyntaxTree newnum(double d) {
  pNumberValue a = (pNumberValue)malloc(sizeof(NumberValue));

  if (!a) {
    yyerror("out of space");
    exit(0);
  }

  a->nodetype = 'K';
  a->number = d;

  return (pAbstractSyntaxTree )a;
}

pAbstractSyntaxTree newcmp(int cmptype, pAbstractSyntaxTree l, pAbstractSyntaxTree r) {
  pAbstractSyntaxTree a = (pAbstractSyntaxTree)malloc(sizeof(AbstractSyntaxTree));

  if (!a) {
    yyerror("out of space");
    exit(0);
  }

  a->nodetype = '0' + cmptype;
  a->l = l;
  a->r = r;

  return a;
}

pAbstractSyntaxTree newfunc(int functype, pAbstractSyntaxTree l) {
  pIntrinsicFunction a = (pIntrinsicFunction)malloc(sizeof(IntrinsicFunction));

  if (!a) {
    yyerror("out of space");
    exit(0);
  }

  a->nodetype = 'F';
  a->l = l;
  a->functype = static_cast<bifs>(functype);

  return (pAbstractSyntaxTree )a;
}

pAbstractSyntaxTree newcall(pSymbol s, pAbstractSyntaxTree l) {
  pUserFunction a = (pUserFunction)malloc(sizeof(UserFunction));

  if (!a) {
    yyerror("out of space");
    exit(0);
  }

  a->nodetype = 'C';
  a->l = l;
  a->s = s;

  return (pAbstractSyntaxTree )a;
}

pAbstractSyntaxTree newref(pSymbol s) {
  pSymbolRef a = (pSymbolRef)malloc(sizeof(SymbolRef));

  if (!a) {
    yyerror("out of space");
    exit(0);
  }

  a->nodetype = 'N';
  a->s = s;

  return (pAbstractSyntaxTree )a;
}

pAbstractSyntaxTree newasgn(pSymbol s, pAbstractSyntaxTree v) {
  pSymbolAssign a = (pSymbolAssign)malloc(sizeof(SymbolAssign));

  if (!a) {
    yyerror("out of space");
    exit(0);
  }

  a->nodetype = '=';
  a->s = s;
  a->v = v;

  return (pAbstractSyntaxTree )a;
}

pAbstractSyntaxTree newflow(int nodetype, pAbstractSyntaxTree cond, pAbstractSyntaxTree tl,
                    pAbstractSyntaxTree el) {
  pFlowControl a = (pFlowControl)malloc(sizeof(FlowControl));

  if (!a) {
    yyerror("out of space");
    exit(0);
  }

  a->nodetype = nodetype;
  a->cond = cond;
  a->tl = tl;
  a->el = el;

  return (pAbstractSyntaxTree )a;
}

/* free a tree of ASTs */
void treefree(pAbstractSyntaxTree a) {
  switch (a->nodetype) {
    /* two subtrees */
    case '+':
    case '-':
    case '*':
    case '/':
    case '1': case '2': case '3': case '4': case '5': case '6':
    case 'L':
      treefree(a->r);

    /* one subtree */
    case '|':
    case 'M': case 'C': case 'F':
      treefree(a->l);
      
    /* no subtree */
    case 'K': case 'N':
      break;

    case '=':
      free(((pSymbolAssign)a)->v);
      break;

    /* up to three subtrees */
    case 'I': case 'W':
      free(((pFlowControl)a)->cond);
      if (((pFlowControl)a)->tl) treefree(((pFlowControl)a)->tl);
      if (((pFlowControl)a)->el) treefree(((pFlowControl)a)->el);
      break;

    default:
      printf("internal error: free bad node %c\n", a->nodetype);
  }

  free(a); /* always free the node itself */
}

pSymbolList newsymlist(pSymbol sym, pSymbolList next) {
  pSymbolList sl = (pSymbolList)malloc(sizeof(SymbolList));

  if (!sl) {
    yyerror("out of space");
    exit(0);
  }

  sl->sym = sym;
  sl->next = next;

  return sl;
}

/* free a list of symbols */
void symlistfree(pSymbolList sl) {
  pSymbolList nsl;

  while (sl) {
    nsl = sl->next;
    free(sl);
    sl = nsl;
  }
}

static double callbuiltin(pIntrinsicFunction); 
static double calluser(pUserFunction);

double eval(pAbstractSyntaxTree a) {
  double v; /* calculated value of this subtree */

  if (!a) {
    yyerror("internal error, null eval");
    return 0.0;
  }

  switch (a->nodetype) {
    /* constant */
    case 'K': v = ((pNumberValue)a)->number; break;

      /* name reference */
    case 'N': v = ((pSymbolRef)a)->s->value; break;

    /* assignment */
    case '=': v = ((pSymbolAssign)a)->s->value = eval(((pSymbolAssign)a)->v); break;

      /* expressions */
    case '+': v = eval(a->l) + eval(a->r); break;
    case '-': v = eval(a->l) - eval(a->r); break;
    case '*': v = eval(a->l) * eval(a->r); break;
    case '/': v = eval(a->l) / eval(a->r); break;
    case '|': v = fabs(eval(a->l)); break;
    case 'M': v = -eval(a->l); break;

    /* comparisons */
    case '1': v = (eval(a->l) > eval(a->r))? 1 : 0; break;
    case '2': v = (eval(a->l) < eval(a->r))? 1 : 0; break;
    case '3': v = (eval(a->l) != eval(a->r))? 1 : 0; break;
    case '4': v = (eval(a->l) == eval(a->r))? 1 : 0; break;
    case '5': v = (eval(a->l) >= eval(a->r))? 1 : 0; break;
    case '6': v = (eval(a->l) <= eval(a->r))? 1 : 0; break;

    /* control flow */
    /* null expressions allowed in the grammar, so check for them */

    /* if/then/else */
    case 'I':
      if (eval(((pFlowControl)a)->cond) != 0) { /* check the condition */
        if (((pFlowControl)a)->tl) { /* the true branch */
          v = eval(((pFlowControl)a)->tl);
        }
        else {
          v = 0.0; /* a default value */
        }
      } else {
        if (((pFlowControl)a)->el) { /* the false branch */
          v = eval(((pFlowControl)a)->el);
        } else {
          v = 0.0; /* a default value */
        }
      }
      break;

    /* while/do */
    case 'W':
      v = 0.0; /* a default value */

      if (((pFlowControl)a)->tl) {
        while (eval(((pFlowControl)a)->cond) != 0) { /* evaluate the condition */
          v = eval(((pFlowControl)a)->tl); /* evaluate the target statements */
        }
      }
      break; /* value of last statement is value of while/do */
    
    /* list of statements */
    case 'L': eval(a->l); v = eval(a->r); break;
    case 'F': v = callbuiltin((pIntrinsicFunction)a); break;
    case 'C': v = calluser((pUserFunction)a); break;

    default: printf("internal error: bad node %c\n", a->nodetype);
  }
  return v;
}

static double callbuiltin(pIntrinsicFunction f) {
  enum bifs functype = f->functype;
  double v = eval(f->l);

  switch (functype) {
    case B_sqrt: return sqrt(v);
    case B_exp: return exp(v);
    case B_log: return log(v);
    case B_print: printf("= %4.4g\n", v); return v;
    default: yyerror("Unknown built-in function %d", functype); return 0.0;
  }
}

/* define a function
  > let max(x,y) = if x >= y then x; else y;;
  > max(4+5,6+7)
*/
void dodef(pSymbol name, pSymbolList syms, pAbstractSyntaxTree func) {
  if (name->syms) symlistfree(name->syms);
  if (name->func) treefree(name->func);

  name->syms = syms;
  name->func = func;
}

static double calluser(pUserFunction f) {
  pSymbol fn = f->s;   /* function name */
  pSymbolList sl;         /* dummy arguments */
  pAbstractSyntaxTree args = f->l;    /* actual arguments */
  double *oldval, *newval;    /* saved arg values */
  double v;
  int nargs;
  int i;

  if (!fn->func) {
    yyerror("call to undefined function", fn->name);
    return 0;
  }

  /* count the arguments */
  sl = fn->syms;
  for (nargs = 0; sl; sl = sl->next) nargs++;

  /* prepare to save them */
  oldval = (double *)malloc(nargs * sizeof(double));
  newval = (double *)malloc(nargs * sizeof(double));

  if (!oldval || !newval) {
    yyerror("Out of space in %s", fn->name);
    return 0.0;
  }

  /* evaluate the arguments */
  for (i = 0; i < nargs; i++) {
    if (!args) {
      yyerror("too few args in call to %s", fn->name);
      free(oldval);
      free(newval);
      return 0.0;
    }

    if (args->nodetype == 'L') { /* if this is a list node */
      newval[i] = eval(args->l);
      args = args->r;
    } else { /* if it's the end of the list */
      newval[i] = eval(args);
      args = NULL;
    }
  }

  /* save old values of dummies, assign new ones */
  sl = fn->syms;
  for (i = 0; i < nargs; i++) {
    pSymbol s = sl->sym;

    oldval[i] = s->value;
    s->value = newval[i];
    sl = sl->next;
  }

  free(newval);

  /* evaluate the function */
  v = eval(fn->func);

  /* put the real values of the dummies back */
  sl = fn->syms;
  for (i = 0; i < nargs; i++) {
    pSymbol s = sl->sym;

    s->value = oldval[i];
    sl = sl->next;
  }

  free(oldval);
  
  return v;
}

void yyerror(const char *s, ...) {
  va_list ap;
  va_start(ap, s);
  fprintf(stderr, "%d: error: ", yylineno);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}

int main() {
  printf("> ");
  return yyparse();
}