/*
  Example 2-3. Skeleton for include files fb2-3.l
  fb2-3 skeleton for include files
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

%option noyywrap
%x IFILE

%top{

#pragma warning(disable: 4819)

#ifdef _MSC_VER
  #define _CRT_SECURE_NO_WARNINGS
#endif

}

%{

#ifdef _MSC_VER
  #define _CRT_SECURE_NO_WARNINGS
#endif

  struct bufstack
  {
    struct bufstack *prev;    /* previous entry */
    YY_BUFFER_STATE bs;       /* saved buffer */
    int lineno;               /* saved line number */
    char *filename;           /* name of this file */
    FILE *f;                  /* current file */
  } *curbs = 0;

  char *curfilename;          /* name of current input file */

  int newfile(char *fn);
  int popfile(void);

%}

%%

  /* match #include statement up through the quote or <  */
^"#"[ \t]*"include"[ \t]*[\"<] { BEGIN IFILE; }

  /* handle filename up to the closing quote, >, or end of line */
<IFILE>[^ \t\n\">]+ {
  {
    int c;
    while((c = yyinput()) && c != '\n');
  }
  yylineno++;
  if(!newfile(yytext)) yyterminate(); /* no such file */

  BEGIN INITIAL;
}

  /* handle bad input in IFILE state */
<IFILE>.|\n {
  fprintf(stderr, "%4d bad include line\n", yylineno);
  yyterminate();
}

  /* pop the file stack at end of file, terminate if it's the outermost file */
<<EOF>> { 
  if(!popfile()) {
    yyterminate();
  }
  else {
    // add newline when files are remain
    fprintf(yyout, "\n");
  }
}

  /* print the line number at the beginning of each line
     and bump the line number each time a \n is read */
^.      { fprintf(yyout, "%4d %s", yylineno, yytext); }
^\n     { fprintf(yyout, "%4d %s", yylineno++, yytext); }
\n      { ECHO; yylineno++; }
.       { ECHO; }

%%

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "need filename\n");
    return 1;
  }

  if (newfile(argv[1])) yylex();
}

int newfile(char *fn) {
  FILE *f = fopen(fn, "r");

  struct bufstack *bs = (struct bufstack *)malloc(sizeof(struct bufstack));

  /* die if no file or no room */
  if (!f) {
    perror(fn);
    return 0;
  }

  if (!bs) {
    perror("malloc");
    exit(1);
  }

  /* remember state */
  if (curbs) curbs->lineno = yylineno;
  bs->prev = curbs;

  /* set up current entry */
  bs->bs = yy_create_buffer(f, YY_BUF_SIZE);

  bs->f = f;
  bs->filename = fn;
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