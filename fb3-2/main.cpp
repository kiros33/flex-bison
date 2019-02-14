/*
  Example 3-8. Advanced calculator helper functions fb3-2func.c
  helper functions for fb3-2
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <ctype.h>
#include "main.h"
#include "parser.h"

bool debug_flag = false;
bool l_debug_flag = false;
bool y_debug_flag = false;

int debug_console(const char *format, ...) {
  va_list arg;
  int done = 0;

  if(debug_flag) {
    va_start(arg, format);
    done = vfprintf(stdout, format, arg);
    va_end(arg);
  }

  return done;
}

int l_debug_console(const char *format, ...) {
  va_list arg;
  int done = 0;

  if(l_debug_flag) {
    va_start(arg, format);
    done = vfprintf(stdout, format, arg);
    printf("\n");
    va_end(arg);
  }

  return done;
}

int y_debug_console(const char *format, ...) {
  va_list arg;
  int done = 0;

  if(y_debug_flag) {
    va_start(arg, format);
    done = vfprintf(stdout, format, arg);
    printf("\n");
    va_end(arg);
  }

  return done;
}

struct symbol symtab[NHASH];

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

struct symbol *lookup(char *sym) {
  struct symbol *sp = &symtab[symhash(sym) % NHASH];
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

struct ast *newast(int nodetype, struct ast *l, struct ast *r) {
  struct ast *a = (struct ast *)malloc(sizeof(struct ast));

  if (!a) {
    yyerror("out of space");
    exit(0);
  }

  a->nodetype = nodetype;
  a->l = l;
  a->r = r;

  return a;
}

struct ast *newnum(double d) {
  struct numval *a = (struct numval *)malloc(sizeof(struct numval));

  if (!a) {
    yyerror("out of space");
    exit(0);
  }

  a->nodetype = 'K';
  a->number = d;

  return (struct ast *)a;
}

struct ast *newcmp(int cmptype, struct ast *l, struct ast *r) {
  struct ast *a = (struct ast *)malloc(sizeof(struct ast));

  if (!a) {
    yyerror("out of space");
    exit(0);
  }

  a->nodetype = '0' + cmptype;
  a->l = l;
  a->r = r;

  return a;
}

struct ast *newfunc(int functype, struct ast *l) {
  struct fncall *a = (struct fncall *)malloc(sizeof(struct fncall));

  if (!a) {
    yyerror("out of space");
    exit(0);
  }

  a->nodetype = 'F';
  a->l = l;
  a->functype = static_cast<bifs>(functype);

  return (struct ast *)a;
}

struct ast *newcall(struct symbol *s, struct ast *l) {
  struct ufncall *a = (struct ufncall *)malloc(sizeof(struct ufncall));

  if (!a) {
    yyerror("out of space");
    exit(0);
  }

  a->nodetype = 'C';
  a->l = l;
  a->s = s;

  return (struct ast *)a;
}

struct ast *newref(struct symbol *s) {
  struct symref *a = (struct symref *)malloc(sizeof(struct symref));

  if (!a) {
    yyerror("out of space");
    exit(0);
  }

  a->nodetype = 'N';
  a->s = s;

  return (struct ast *)a;
}

struct ast *newasgn(struct symbol *s, struct ast *v) {
  struct symasgn *a = (struct symasgn *)malloc(sizeof(struct symasgn));

  if (!a) {
    yyerror("out of space");
    exit(0);
  }

  a->nodetype = '=';
  a->s = s;
  a->v = v;

  return (struct ast *)a;
}

struct ast *newflow(int nodetype, struct ast *cond, struct ast *tl,
                    struct ast *el) {
  struct flow *a = (struct flow *)malloc(sizeof(struct flow));

  if (!a) {
    yyerror("out of space");
    exit(0);
  }

  a->nodetype = nodetype;
  a->cond = cond;
  a->tl = tl;
  a->el = el;

  return (struct ast *)a;
}

/* free a tree of ASTs */
void treefree(struct ast *a) {
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
      free(((struct symasgn *)a)->v);
      break;

    /* up to three subtrees */
    case 'I': case 'W':
      free(((struct flow *)a)->cond);
      if (((struct flow *)a)->tl) treefree(((struct flow *)a)->tl);
      if (((struct flow *)a)->el) treefree(((struct flow *)a)->el);
      break;

    default:
      printf("internal error: free bad node %c\n", a->nodetype);
  }

  free(a); /* always free the node itself */
}

struct symlist *newsymlist(struct symbol *sym, struct symlist *next) {
  struct symlist *sl = (struct symlist *)malloc(sizeof(struct symlist));

  if (!sl) {
    yyerror("out of space");
    exit(0);
  }

  sl->sym = sym;
  sl->next = next;

  return sl;
}

/* free a list of symbols */
void symlistfree(struct symlist *sl) {
  struct symlist *nsl;

  while (sl) {
    nsl = sl->next;
    free(sl);
    sl = nsl;
  }
}

static double callbuiltin(struct fncall *); 
static double calluser(struct ufncall *);

double eval(struct ast *a) {
  double v; /* calculated value of this subtree */

  if (!a) {
    yyerror("internal error, null eval");
    return 0.0;
  }

  switch (a->nodetype) {
    /* constant */
    case 'K': v = ((struct numval *)a)->number; break;

      /* name reference */
    case 'N': v = ((struct symref *)a)->s->value; break;

    /* assignment */
    case '=': v = ((struct symasgn *)a)->s->value = eval(((struct symasgn *)a)->v); break;

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
      if (eval(((struct flow *)a)->cond) != 0) { /* check the condition */
        if (((struct flow *)a)->tl) { /* the true branch */
          v = eval(((struct flow *)a)->tl);
        }
        else {
          v = 0.0; /* a default value */
        }
      } else {
        if (((struct flow *)a)->el) { /* the false branch */
          v = eval(((struct flow *)a)->el);
        } else {
          v = 0.0; /* a default value */
        }
      }
      break;

    /* while/do */
    case 'W':
      v = 0.0; /* a default value */

      if (((struct flow *)a)->tl) {
        while (eval(((struct flow *)a)->cond) != 0) { /* evaluate the condition */
          v = eval(((struct flow *)a)->tl); /* evaluate the target statements */
        }
      }
      break; /* value of last statement is value of while/do */
    
    /* list of statements */
    case 'L': eval(a->l); v = eval(a->r); break;
    case 'F': v = callbuiltin((struct fncall *)a); break;
    case 'C': v = calluser((struct ufncall *)a); break;

    default: printf("internal error: bad node %c\n", a->nodetype);
  }
  return v;
}

static double callbuiltin(struct fncall *f) {
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
void dodef(struct symbol *name, struct symlist *syms, struct ast *func) {
  if (name->syms) symlistfree(name->syms);
  if (name->func) treefree(name->func);

  name->syms = syms;
  name->func = func;
}

static double calluser(struct ufncall *f) {
  struct symbol *fn = f->s;   /* function name */
  struct symlist *sl;         /* dummy arguments */
  struct ast *args = f->l;    /* actual arguments */
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
    struct symbol *s = sl->sym;

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
    struct symbol *s = sl->sym;

    s->value = oldval[i];
    sl = sl->next;
  }

  free(oldval);
  
  return v;
}

void yyerror(const char *s, ...) {
  va_list ap;
  va_start(ap, s);
  fprintf(stderr, "%d:%d %s:", yylineno, 0, yytext);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}

/*
        -w         Suppress warnings.
        -l         Show library listing during load.
        -u         Update object(s). *
        -t         Update object(s) with tracing. *
        -s         Show structure of loaded object(s). *
        -o[=]ofile Redirect output to a file.
        -b[=]bfile Create equivalent binary library. *

        getoptlong
*/

int main(int argc, char **argv) {
  int index;
  int c;

  opterr = 0;

  while ((c = getopt(argc, argv, "wlutso:b:d")) != -1) {
    debug_console("[debug][main][options][%c]", c);
    switch (c) {
      case 'w':
        break;
      case 'l':
        break;
      case 'u':
        break;
      case 't':
        break;
      case 's':
        break;
      case 'o':
        debug_console("[%s]", optarg);
        break;
      case 'b':
        //debug_console("[%s]", optarg);
        break;
      case 'd':
        debug_flag = true;
        l_debug_flag = true;
        y_debug_flag = true;
        break;
      case '?':
        if (optopt == 'o') {
          fprintf(stderr, "option -%c requires log filename\n", optopt);
        } 
        else if (optopt == 'b') {
          fprintf(stderr, "option -%c requires binary filename\n", optopt);
        } else if (isprint(optopt)) {
          fprintf(stderr, "Unknown option `-%c`\n", optopt);
        }
        else {
          fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
        }
        return 1;
      default:
        abort();
    }
    debug_console("\n");
  }

  for (index = optind; index < argc; index++) {
    fprintf(stderr, "Not supported option argument %s\n", argv[index]);
  }

  printf("> ");
  return yyparse();
}