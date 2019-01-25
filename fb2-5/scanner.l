/*
  Example 2-5. C cross-referencer fb2-5.l
  fb2-5 C cross-ref
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

%option noyywrap nodefault yylineno

%x COMMENT
%x IFILE

%top{

#pragma warning(disable: 4819)

#ifdef _MSC_VER
  #define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdarg.h>

}

/* some complex named patterns */
/* Universal Character Name */
UCN    (\\u[0-9a-fA-F]{4}|\\U[0-9a-fA-F]{8})
/* float exponent */
EXP    ([Ee][-+]?[0-9]+)
/* integer length */
ILEN    ([Uu](L|l|LL|ll)?|(L|l|LL|ll)[Uu]?)

/* the symbol table */
%{
  
  struct symbol {           /* a variable name */
    struct ref *reflist;
    char *name;
  };

  struct ref {
    struct ref *next;
    char *filename;
    int flags;              /* 01 - definition */
    int lineno;
  };

  /* simple symtab of fixed size */
  #define NHASH 9997
  struct symbol symtab[NHASH];

  struct symbol *lookup(char*);
  void addref(int, char*, char*, int);
  void printrefs();

  char *curfilename;        /* name of current input file */

  /* include file stack */
  struct bufstack {
    struct bufstack *prev;  /* previous entry */
    YY_BUFFER_STATE bs;     /* saved buffer */
    int lineno;             /* saved line number in this file */
    char *filename;         /* name of this file */ 
    FILE *f;                /* current file */
  } *curbs;

  int newfile(char *);
  int popfile(void);

  int defining;             /* names are probably definitions */

  int debug_console(const char *, ...);

%}

%%

  /* comments */
"/*"                        { BEGIN(COMMENT); /* debug_console("==> start comments\n"); */ }
<COMMENT>"*/"               { BEGIN(INITIAL); /* debug_console("==> end comments\n"); */ }
<COMMENT>([^*]|\n|\*[^/])+  { /* ignore */ /* debug_console("==> comments: `%s`\n", yytext); */ }
<COMMENT><<EOF>>            { debug_console("==> %s:%d: Unterminated comment\n", curfilename, yylineno); return 0; }

  /* C++ comment, a common extension */
"//"(.*)\n

  /* declaration keywords */
_Bool |
_Complex |
_Imaginary |
auto |
char |
const |
double |
enum |
extern |
float |
inline |
int |
long |
register |
restrict |
short |
signed |
static |
struct |
typedef |
union |
unsigned |
void |
volatile                    { defining = 1; }

  /* keywords */
break
case
continue
default
do
else
for
goto
if
return
sizeof
switch
while

  /* constants */

  /* integers */
0[0-7]*{ILEN}?
[1-9][0-9]*{ILEN}?
0[Xx][0-9a-fA-F]+{ILEN}?

 /* decimal float */
([0-9]*\.[0-9]+|[0-9]+\.){EXP}?[flFL]?
[0-9]+{EXP}[flFL]?

  /* hex float */
0[Xx]([0-9a-fA-F]*\.[0-9a-fA-F]+|[0-9a-fA-F]+\.?)[Pp][-+]?[0-9]+[flFL]?

  /* char const */
\'([^'\\]|\\['"?\\abfnrtv]|\\[0-7]{1,3}|\\[Xx][0-9a-fA-F]+|{UCN})+\'

  /* string literal */
L?\"([^"\\]|\\['"?\\abfnrtv]|\\[0-7]{1,3}|\\[Xx][0-9a-fA-F]+|{UCN})*\"

 /* punctuators */
"{"|"<%"|";"                { defining = 0; }

"["|"]"|"("|")"|"{"|"}"|"."|"->"
"++"|"--"|"&"|"*"|"+"|"-"|"~"|"!"
"/"|"%"|"<<"|">>"|"<"|">"|"<="|">="|"=="|"!="|"^"|"|"|"&&"|"||"
"?"|":"|";"|"..."
"="|"*="|"/="|"%="|"+="|"-="|"<<="|">>="|"&="|"^=""|="
","|"#"|"##"
"<:"|":>"|"%>"|"%:"|"%:%:"

  /* identifier */
([_a-zA-Z]|{UCN})([_a-zA-Z0-9]|{UCN})* { addref(yylineno, curfilename, yytext, defining); }

  /* whitespace */
[ \t\n]+

  /* continued line */
\\$

  /* some preprocessor stuff */
"#"" "*if.*\n
"#"" "*else.*\n
"#"" "*endif.*\n
"#"" "*define.*\n
"#"" "*line.*\n

  /* recognize an include */
^"#"[ \t]*include[ \t]*[\"<] { BEGIN IFILE; }
<IFILE>[^>\"]+              {
  {
    int c;
    while((c = yyinput()) && c != '\n') ; 
  } 
  newfile(strdup(yytext)); 
  BEGIN INITIAL; 
}

<IFILE>.|\n                 { 
  fprintf(stderr, "%s:%d bad include line\n", curfilename, yylineno);
  BEGIN INITIAL;
}

<<EOF>>                     { if(!popfile()) yyterminate(); }

  /* invalid character */
.                           { /* ignore */ debug_console("==> %s:%d: Mystery character '%s'\n", curfilename, yylineno, yytext); }

%%

bool debug_flag = true;

int nnew, nold, ndup;
int nprobe;

  /* concordance main routine */
int main(int argc, char **argv) {
  int i;

  if (argc < 2) {           /* just read stdin */
    curfilename = (char *)"(stdin)";
    yylineno = 1;
    yylex();
  } else {
    for (i = 1; i < argc; i++) {
      if (newfile(argv[i])) yylex();
    }
  }

  printrefs();
  return 0;
}

/* nested input files */
int newfile(char *fn) {
  FILE *f = fopen(fn, "r");
  struct bufstack *bs;

  /* check if no file */
  if (!f) {
    perror(fn);
    return 0;
  }

  bs = (struct bufstack *)malloc(sizeof(struct bufstack));
  if (!bs) {
    perror("malloc");
    exit(1);
  }

  /* remember state */
  if (curbs) curbs->lineno = yylineno;

  bs->prev = curbs;
  bs->f = f;
  bs->filename = fn;

  /* set up current entry */
  bs->bs = yy_create_buffer(f, YY_BUF_SIZE);
  yy_switch_to_buffer(bs->bs);
  curbs = bs;
  yylineno = 1;
  curfilename = fn;

  return 1;
}

int popfile(void) {
  struct bufstack *bs = curbs;
  struct bufstack *prevbs;

  if (!bs) return 0;

  /* get rid of current entry */ 
  fclose(bs->f);
  yy_delete_buffer(bs->bs);

  /* switch back to previous */ 
  prevbs = bs->prev;
  free(bs);

  if (!prevbs) return 0;

  yy_switch_to_buffer(prevbs->bs);
  curbs = prevbs;
  yylineno = curbs->lineno;
  curfilename = curbs->filename;
  return 1;
}

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
  //debug_console("*** %-10s symbol=%s\n", "lookup", sym);
  //debug_console("*** %-10s symbol=%s hash=%u\n", "lookup", sym, (symhash(sym) % NHASH));
  
  struct symbol *sp = &symtab[symhash(sym) % NHASH];
  int scount = NHASH; /* how many have we looked at */

  while (--scount >= 0) {
    //debug_console("*** %-10s scount=%d\n", "lookup", scount);
    nprobe++;
    if (sp->name && !strcmp(sp->name, sym)) { 
      debug_console("*** %-10s symbol '%s' exist\n", "lookup", sym);
      nold++; return sp;
    }

    if (!sp->name) {
      debug_console("*** %-10s symbol '%s' define\n", "lookup", sym);
      nnew++;
      sp->name = strdup(sym);
      sp->reflist = 0;
      return sp;
    }

    debug_console("--> %-10s '%s' already defined, try next entry\n", "lookup", sp->name);

    /* new entry */
    //debug_console("--> %-10s symbol address is %d and base address is %d ~ %d\n", "lookup", sp, symtab, (symtab + NHASH));
    if (++sp >= symtab + NHASH) { sp = symtab; } /* try the next entry */
    //debug_console("--> %-10s new symbol address is %d\n", "lookup", sp);

    ndup++;
  }

  fputs("symbol table overflow\n", stderr);
  abort(); /* tried them all, table is full */
}

void addref(int lineno, char *filename, char *word, int flags) {
  debug_console("**********\n");
  debug_console("*** %-10s line=%d, filename=%s, word=%s, flags=%d\n", "addref", lineno, filename, word, flags);

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

  //qsort(symtab, NHASH, sizeof(struct symbol), symcompare); /* sort the symbol table */

  debug_console("========== symbol table\n");
  
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
    debug_console("%-10s", sp->name);
    for (rp = rpp; rp; rp = rp->next) {
      if (rp->filename == prevfn) {
        debug_console(" → %d", rp->lineno);
      } else {
        debug_console(" → %s:%d", rp->filename, rp->lineno);
        prevfn = rp->filename;
      }
      if(rp->flags & 01) debug_console("*");
    }
    debug_console("\n");
  }
}

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