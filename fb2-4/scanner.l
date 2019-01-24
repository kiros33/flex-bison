/*
  Example 2-4. Concordance generator
  fb2-4 text concordance
*/

/* generate header file */
%option header-file = "scanner.h"

/* generate implementation file */
%option outfile = "scanner.cpp"

%option always-interactive nounistd

/* Visual Studio 빌드시 라이브러리 추가
   C:\cygwin64\usr\local\lib\liby.a
   C:\cygwin64\usr\local\lib\libfl.a
   64비트만 빌드됨
*/

/*
Visual Studio 에서 fopen_s 관련 오류를 해결하는 방법 1
#ifdef _MSC_VER
  #define _CRT_SECURE_NO_WARNINGS
#endif

Visual Studio 에서 fopen_s 관련 오류를 해결하는 방법 2
#ifndef _MSC_VER
  #define fopen_s(fp, fmt, mode) *(fp)=fopen((fmt), (mode))
#endif

  FILE *f;
  fopen_s(f, fn, "r");

한글 주석같은 문자가 포함된 경우 경고가 표시되는데 경고를 무시
#pragma warning(disable: 4819)
*/

%option noyywrap nodefault yylineno case-insensitive

%top{

#pragma warning(disable: 4819)

#ifdef _MSC_VER
  #define _CRT_SECURE_NO_WARNINGS
#endif

}

/* the symbol table */
%{
  
  struct symbol {   /* a word */
    struct ref *reflist;
    char *name;
  };

  struct ref {
    struct ref *next;
    char *filename;
    int flags;
    int lineno;
  };

  /* simple symtab of fixed size */
  #define NHASH 9997
  struct symbol symtab[NHASH];

  struct symbol *lookup(char*);
  void addref(int, char*, char*, int);
  void printrefs();

  char *curfilename;    /* name of current input file */

%}

%%

 /* skip common words */
a |
  an |
  and |
  are |
  as |
  at |
  be |
  but |
  for |
  in |
  is |
  it |
  not |
  of |
  on |
  or |
  that |
  the |
  this |
  to                  { /* ignore */ }
[a-z]+(\'(s|t))?    { addref(yylineno, curfilename, yytext, 0); }
.|\n                { /* ignore everything else */ }
%%

int nnew, nold, ndup;
int nprobe;

/* concordance main routine */
int main(int argc, char **argv) {
  int i;

  if (argc < 2) { /* just read stdin */
    curfilename = (char *)"(stdin)";
    yylineno = 1;
    yylex();
  } else
    for (i = 1; i < argc; i++) {
      FILE *f = fopen(argv[i], "r");

      if (!f) {
        perror(argv[1]);
        return (1);
      }
      curfilename = argv[i]; /* for addref */

      yyrestart(f);
      yylineno = 1;
      yylex();
      fclose(f);
    }

  printrefs();

  printf("\n");
  printf("old = %d, new = %d, dup = %d, total = %d, probes = %d, avg = %1.2f\n", nold,
         nnew, ndup, nold + nnew, nprobe, (float)nprobe / (nold + nnew));

  return 0;
}

/* hash a symbol */
static unsigned int symhash(char *sym) {
  //printf("*** %-10s symbol=%s", "symhash", sym);
  unsigned int hash = 0;
  unsigned c;

  while ((c = *sym++)) {
    //printf(" hash=%u", hash);
    hash = hash * 9 ^ c;
  }

  //printf(" hash=%u\n", hash);

  return hash;
}

struct symbol *lookup(char *sym) {
  //printf("*** %-10s symbol=%s\n", "lookup", sym);
  //printf("*** %-10s symbol=%s hash=%u\n", "lookup", sym, (symhash(sym) % NHASH));
  
  struct symbol *sp = &symtab[symhash(sym) % NHASH];
  int scount = NHASH; /* how many have we looked at */

  while (--scount >= 0) {
    //printf("*** %-10s scount=%d\n", "lookup", scount);
    nprobe++;
    if (sp->name && !strcmp(sp->name, sym)) { 
      printf("*** %-10s symbol '%s' exist\n", "lookup", sym);
      nold++; return sp;
    }

    if (!sp->name) {
      printf("*** %-10s symbol '%s' define\n", "lookup", sym);
      nnew++;
      sp->name = strdup(sym);
      sp->reflist = 0;
      return sp;
    }

    printf("--> %-10s '%s' already defined, try next entry\n", "lookup", sp->name);

    /* new entry */
    //printf("--> %-10s symbol address is %d and base address is %d ~ %d\n", "lookup", sp, symtab, (symtab + NHASH));
    if (++sp >= symtab + NHASH) { sp = symtab; } /* try the next entry */
    //printf("--> %-10s new symbol address is %d\n", "lookup", sp);

    ndup++;
  }

  fputs("symbol table overflow\n", stderr);
  abort(); /* tried them all, table is full */
}

void addref(int lineno, char *filename, char *word, int flags) {
  printf("**********\n");
  printf("*** %-10s line=%d, filename=%s, word=%s, flags=%d\n", "addref", lineno, filename, word, flags);

  struct ref *r;
  struct symbol *sp = lookup(word);

  /* don't do dups of same line and file */
  if (sp->reflist && sp->reflist->lineno == lineno && sp->reflist->filename == filename)
    return;

  r = (struct ref *)malloc(sizeof(struct ref));

  if (!r) {
    fputs("out of space\n", stderr);
    abort();
  }
  
  r->next = sp->reflist;
  r->filename = filename;
  r->lineno = lineno;
  r->flags = flags;
  sp->reflist = r;
}

/* 
 * print the references
 * sort the table alphabetically 
 * then flip each entry's reflist to get it into forward order 
 * and print it out 
 */

/* aux function for sorting */ 
static int symcompare(const void *xa, const void *xb) {
  const struct symbol *a = (struct symbol *)xa;
  const struct symbol *b = (struct symbol *)xb;

  if (!a->name) {
    if (!b->name) return 0; /* both empty */
    return 1; /* put empties at the end */
  }
  if (!b->name) return -1;
  return strcmp(a->name, b->name);
}

void printrefs() {
  struct symbol *sp;

  qsort(symtab, NHASH, sizeof(struct symbol), symcompare); /* sort the symbol table */

  printf("========== symbol table\n");
  
  for (sp = symtab; sp->name && sp < symtab + NHASH; sp++) {
    char *prevfn = NULL; /* last printed filename, to skip dups */

    /* reverse the list of references */
    struct ref *rp = sp->reflist;
    struct ref *rpp = 0; /* previous ref */
    struct ref *rpn;     /* next ref */

    do {
      rpn = rp->next;
      rp->next = rpp;
      rpp = rp;
      rp = rpn;
    } while (rp);

    /* now print the word and its references */ 
    printf("%-10s", sp->name);
    for (rp = rpp; rp; rp = rp->next) {
      if (rp->filename == prevfn) {
        printf(" → %d", rp->lineno);
      } else {
        printf(" → %s:%d", rp->filename, rp->lineno);
        prevfn = rp->filename;
      }
    }
    printf("\n");
  }
}