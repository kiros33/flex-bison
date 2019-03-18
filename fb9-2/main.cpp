/*
  Example 3-8. Advanced calculator helper functions fb3-2func.c
  helper functions for fb3-2
*/
/*
  Example 9-5. Helper functions purecalcfuncs.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
//#include <unistd.h>
#include <ctype.h>
#include "main.h"
#include "parser.h"
#include "scanner.h"
#include "wingetopt.h"

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

//struct symbol symtab[NHASH];

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

struct symbol *lookup(struct pcdata *pp, char *sym) {
  struct symbol *sp = &(pp->symtab)[symhash(sym) % NHASH];
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

    if (++sp >= (pp->symtab) + NHASH) sp = (pp->symtab); /* try the next entry */
  }
  yyerror(pp, "symbol table overflow\n");
  abort(); /* tried them all, table is full */
}

struct ast *newast(struct pcdata *pp, int nodetype, struct ast *l, struct ast *r) {
  struct ast *a = (struct ast *)malloc(sizeof(struct ast));

  if (!a) {
    yyerror(pp, "out of space");
    exit(0);
  }

  a->nodetype = nodetype;
  a->l = l;
  a->r = r;

  return a;
}

struct ast *newnum(struct pcdata *pp, double d) {
  struct numval *a = (struct numval *)malloc(sizeof(struct numval));

  if (!a) {
    yyerror(pp, "out of space");
    exit(0);
  }

  a->nodetype = 'K';
  a->number = d;

  return (struct ast *)a;
}

struct ast *newcmp(struct pcdata *pp, int cmptype, struct ast *l, struct ast *r) {
  struct ast *a = (struct ast *)malloc(sizeof(struct ast));

  if (!a) {
    yyerror(pp, "out of space");
    exit(0);
  }

  a->nodetype = '0' + cmptype;
  a->l = l;
  a->r = r;

  return a;
}

struct ast *newfunc(struct pcdata *pp, int functype, struct ast *l) {
  struct fncall *a = (struct fncall *)malloc(sizeof(struct fncall));

  if (!a) {
    yyerror(pp, "out of space");
    exit(0);
  }

  a->nodetype = 'F';
  a->l = l;
  a->functype = static_cast<bifs>(functype);

  return (struct ast *)a;
}

struct ast *newcall(struct pcdata *pp, struct symbol *s, struct ast *l) {
  struct ufncall *a = (struct ufncall *)malloc(sizeof(struct ufncall));

  if (!a) {
    yyerror(pp, "out of space");
    exit(0);
  }

  a->nodetype = 'C';
  a->l = l;
  a->s = s;

  return (struct ast *)a;
}

struct ast *newref(struct pcdata *pp, struct symbol *s) {
  struct symref *a = (struct symref *)malloc(sizeof(struct symref));

  if (!a) {
    yyerror(pp, "out of space");
    exit(0);
  }

  a->nodetype = 'N';
  a->s = s;

  return (struct ast *)a;
}

struct ast *newasgn(struct pcdata *pp, struct symbol *s, struct ast *v) {
  struct symasgn *a = (struct symasgn *)malloc(sizeof(struct symasgn));

  if (!a) {
    yyerror(pp, "out of space");
    exit(0);
  }

  a->nodetype = '=';
  a->s = s;
  a->v = v;

  return (struct ast *)a;
}

struct ast *newflow(struct pcdata *pp, int nodetype, struct ast *cond, struct ast *tl, struct ast *el) {
  struct flow *a = (struct flow *)malloc(sizeof(struct flow));

  if (!a) {
    yyerror(pp, "out of space");
    exit(0);
  }

  a->nodetype = nodetype;
  a->cond = cond;
  a->tl = tl;
  a->el = el;

  return (struct ast *)a;
}

/* free a tree of ASTs */
void treefree(struct pcdata *pp, struct ast *a) {
  switch (a->nodetype) {
    /* two subtrees */
    case '+':
    case '-':
    case '*':
    case '/':
    case '1': case '2': case '3': case '4': case '5': case '6':
    case 'L':
      treefree(pp, a->r);

    /* one subtree */
    case '|':
    case 'M': case 'C': case 'F':
      treefree(pp, a->l);
      
    /* no subtree */
    case 'K': case 'N':
      break;

    case '=':
      free(((struct symasgn *)a)->v);
      break;

    /* up to three subtrees */
    case 'I': case 'W':
      free(((struct flow *)a)->cond);
      if (((struct flow *)a)->tl) treefree(pp, ((struct flow *)a)->tl);
      if (((struct flow *)a)->el) treefree(pp, ((struct flow *)a)->el);
      break;

    default:
      printf("internal error: free bad node %c\n", a->nodetype);
  }

  free(a); /* always free the node itself */
}

struct symlist *newsymlist(struct pcdata *pp, struct symbol *sym, struct symlist *next) {
  struct symlist *sl = (struct symlist *)malloc(sizeof(struct symlist));

  if (!sl) {
    yyerror(pp, "out of space");
    exit(0);
  }

  sl->sym = sym;
  sl->next = next;

  return sl;
}

/* free a list of symbols */
void symlistfree(struct pcdata *pp, struct symlist *sl) {
  struct symlist *nsl;

  while (sl) {
    nsl = sl->next;
    free(sl);
    sl = nsl;
  }
}

static double callbuiltin(struct pcdata *pp, struct fncall *); 
static double calluser(struct pcdata *pp, struct ufncall *);

double eval(struct pcdata *pp, struct ast *a) {
  double v; /* calculated value of this subtree */

  if (!a) {
    yyerror(pp, "internal error, null eval");
    return 0.0;
  }

  switch (a->nodetype) {
    /* constant */
    case 'K': v = ((struct numval *)a)->number; break;

      /* name reference */
    case 'N': v = ((struct symref *)a)->s->value; break;

    /* assignment */
    case '=': v = ((struct symasgn *)a)->s->value = eval(pp, ((struct symasgn *)a)->v); break;

      /* expressions */
    case '+': v = eval(pp, a->l) + eval(pp, a->r); break;
    case '-': v = eval(pp, a->l) - eval(pp, a->r); break;
    case '*': v = eval(pp, a->l) * eval(pp, a->r); break;
    case '/': v = eval(pp, a->l) / eval(pp, a->r); break;
    case '|': v = fabs(eval(pp, a->l)); break;
    case 'M': v = -eval(pp, a->l); break;

    /* comparisons */
    case '1': v = (eval(pp, a->l) > eval(pp, a->r))? 1 : 0; break;
    case '2': v = (eval(pp, a->l) < eval(pp, a->r))? 1 : 0; break;
    case '3': v = (eval(pp, a->l) != eval(pp, a->r))? 1 : 0; break;
    case '4': v = (eval(pp, a->l) == eval(pp, a->r))? 1 : 0; break;
    case '5': v = (eval(pp, a->l) >= eval(pp, a->r))? 1 : 0; break;
    case '6': v = (eval(pp, a->l) <= eval(pp, a->r))? 1 : 0; break;

    /* control flow */
    /* null expressions allowed in the grammar, so check for them */

    /* if/then/else */
    case 'I':
      if (eval(pp, ((struct flow *)a)->cond) != 0) { /* check the condition */
        if (((struct flow *)a)->tl) { /* the true branch */
          v = eval(pp, ((struct flow *)a)->tl);
        }
        else {
          v = 0.0; /* a default value */
        }
      } else {
        if (((struct flow *)a)->el) { /* the false branch */
          v = eval(pp, ((struct flow *)a)->el);
        } else {
          v = 0.0; /* a default value */
        }
      }
      break;

    /* while/do */
    case 'W':
      v = 0.0; /* a default value */

      if (((struct flow *)a)->tl) {
        while (eval(pp, ((struct flow *)a)->cond) != 0) { /* evaluate the condition */
          v = eval(pp, ((struct flow *)a)->tl); /* evaluate the target statements */
        }
      }
      break; /* value of last statement is value of while/do */
    
    /* list of statements */
    case 'L': eval(pp, a->l); v = eval(pp, a->r); break;
    case 'F': v = callbuiltin(pp, (struct fncall *)a); break;
    case 'C': v = calluser(pp, (struct ufncall *)a); break;

    default: printf("internal error: bad node %c\n", a->nodetype);
  }
  return v;
}

static double callbuiltin(struct pcdata *pp, struct fncall *f) {
  enum bifs functype = f->functype;
  double v = eval(pp, f->l);

  switch (functype) {
    case B_sqrt: return sqrt(v);
    case B_exp: return exp(v);
    case B_log: return log(v);
    case B_print: printf("= %4.4g\n", v); return v;
    default: yyerror(pp, "Unknown built-in function %d", functype); return 0.0;
  }
}

/* define a function
  > let max(x,y) = if x >= y then x; else y;;
  > max(4+5,6+7)
*/
void dodef(struct pcdata *pp, struct symbol *name, struct symlist *syms, struct ast *func) {
  if (name->syms) symlistfree(pp, name->syms);
  if (name->func) treefree(pp, name->func);

  name->syms = syms;
  name->func = func;
}

static double calluser(struct pcdata *pp, struct ufncall *f) {
  struct symbol *fn = f->s;   /* function name */
  struct symlist *sl;         /* dummy arguments */
  struct ast *args = f->l;    /* actual arguments */
  double *oldval, *newval;    /* saved arg values */
  double v;
  int nargs;
  int i;

  if (!fn->func) {
    yyerror(pp, "call to undefined function", fn->name);
    return 0;
  }

  /* count the arguments */
  sl = fn->syms;
  for (nargs = 0; sl; sl = sl->next) nargs++;

  /* prepare to save them */
  oldval = (double *)malloc(nargs * sizeof(double));
  newval = (double *)malloc(nargs * sizeof(double));

  if (!oldval || !newval) {
    yyerror(pp, "Out of space in %s", fn->name);
    return 0.0;
  }

  /* evaluate the arguments */
  for (i = 0; i < nargs; i++) {
    if (!args) {
      yyerror(pp, "too few args in call to %s", fn->name);
      free(oldval);
      free(newval);
      return 0.0;
    }

    if (args->nodetype == 'L') { /* if this is a list node */
      newval[i] = eval(pp, args->l);
      args = args->r;
    } else { /* if it's the end of the list */
      newval[i] = eval(pp, args);
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
  v = eval(pp, fn->func);

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

void yyerror(struct pcdata *pp, const char *s, ...) {
  va_list ap;
  va_start(ap, s);
  fprintf(stderr, "%d: error", yyget_lineno(pp->scaninfo));
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

  int quit_flag = 0;

  while ((c = getopt(argc, argv, "wlutso:b:d")) != -1) {
    debug_console("[debug][main][options][%c]", c);
    switch (c) {
      case 'w':
        debug_console("[%s option]", "w");
        break;
      case 'l':
        debug_console("[%s option]", "l");
        break;
      case 'u':
        debug_console("[%s option]", "u");
        break;
      case 't':
        debug_console("[%s option]", "t");
        break;
      case 's':
        debug_console("[%s option]", "s");
        break;
      case 'o':
        debug_console("[%s option]+[%s]", "logfile output", optarg);
        break;
      case 'b':
        debug_console("[%s option]+[%s]", "binary output", optarg);
        break;
      case 'd':
        debug_flag = true;
        l_debug_flag = true;
        y_debug_flag = true;
        debug_console("[%s option]", "debug");
        break;
      case '?':
        debug_console("-->");
        if (optopt == 'o') {
          fprintf(stderr, "option -%c requires log filename\n", optopt);
        } else if (optopt == 'b') {
          fprintf(stderr, "option -%c requires binary filename\n", optopt);
        } else if (isprint(optopt)) {
          fprintf(stderr, "Unknown option `-%c`\n", optopt);
        } else {
          fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
        }
        quit_flag++;
        break;
      default:
        debug_console("[%s][%c]", "default", c);
        abort();
    }
    debug_console("\n");
    if(quit_flag > 0) return 1;
  }

  for (index = optind; index < argc; index++) {
    fprintf(stderr, "Not supported option argument %s\n", argv[index]);
  }

  struct pcdata p = { NULL, 0, NULL };

  /* set up scanner */
  if(yylex_init_extra(&p, &p.scaninfo)) {
    perror("init alloc failed");
    exit(1);
  }

  /* allocate and zero out the symbol table */
  if(!(p.symtab = (struct symbol *)calloc(NHASH, sizeof(struct symbol)))) {
    perror("sym alloc failed");
    return 1;
  }

  for (;;) {
    printf("> ");
    yyparse(&p);
    if (p.ast) {
      printf("= %4.4g\n", eval(&p, p.ast));
      treefree(&p, p.ast);
      p.ast = 0;
    }
  }

  return 0;
}