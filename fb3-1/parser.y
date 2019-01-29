/*
  Example 3-2. Bison parser for AST calculator, fb3-1.y
  calculator with AST
*/

/* generate header file */
%defines "parser.h"

/* the parser implementation file name */
%output "parser.cpp"

%{
  #include <stdio.h>
  #include <stdlib.h>
  #include "main.h"

  extern int yylex (void);
%}

%union {
  struct ast *a;
  double d;
}

/* declare tokens */
%token <d> NUMBER
%token EOL

%type <a> exp factor term

%%

calclist: /* nothing */
  | calclist exp EOL { 
      printf("= %4.4g\n", eval($2));  /* evaluate and print the AST */
      treefree($2);                   /* free up the AST */
      printf("> ");
    }
  | calclist EOL { printf("> "); }    /* blank line or a comment */
;

exp : factor 
  | exp '+' factor { $$ = newast('+', $1, $3); }
  | exp '-' factor { $$ = newast('-', $1, $3); }
;

factor: term
  | factor '*' term { $$ = newast('*', $1, $3); }
  | factor '/' term { $$ = newast('/', $1, $3); }
;

term: NUMBER { $$ = newnum($1); }
  | '|' term { $$ = newast('|', $2, NULL); }
  | '(' exp ')' { $$ = $2; }
  | '-' term { $$ = newast('M', $2, NULL); }
;

%%
