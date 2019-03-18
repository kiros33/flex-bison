/*
  Example 3-6. Advanced calculator parser fb3-2.y
  calculator with AST
*/
/*
  Example 9-4. Reentrant calculator parser purecalc.y
*/

/* generate header file */
%defines "parser.h"

/* the parser implementation file name */
%output "parser.cpp"

/* calculator with AST */
%define api.pure
%lex-param { yyscan_t YYLEX_PARAM }
%parse-param { struct pcdata *pp }


%{
  #include <stdio.h>
  #include <stdlib.h>
  #include "main.h"
  #include "parser.h"
  #include "scanner.h"

  #define YYLEX_PARAM pp->scaninfo

  extern int y_debug_console(const char *, ...);
%}

%union { 
  struct ast *a;
  double d;
  struct symbol *s; /* which symbol */
  struct symlist *sl;
  int fn; /* which function */
}

/* declare tokens */
%token <d> NUMBER
%token <s> NAME
%token <fn> FUNC
%token EOL

%token IF THEN ELSE WHILE DO LET

%nonassoc <fn> CMP
%right '='
%left '+' '-'
%left '*' '/'
%nonassoc '|' UMINUS

%type <a> exp stmt list explist
%type <sl> symlist

%start calc

%%

stmt: IF exp THEN list          { $$ = newflow(pp, 'I', $2, $4, NULL); y_debug_console("[debug][y][stmt][IF exp THEN list]"); }
  | IF exp THEN list ELSE list  { $$ = newflow(pp, 'I', $2, $4, $6); y_debug_console("[debug][y][stmt][IF exp THEN list ELSE list]"); }
  | WHILE exp DO list           { $$ = newflow(pp, 'W', $2, $4, NULL); y_debug_console("[debug][y][stmt][WHILE exp DO list"); }
  | exp                         { y_debug_console("[debug][y][stmt][exp]"); }
;

list: /* nothing */ { $$ = NULL; y_debug_console("[debug][y][list][null]"); }
  | stmt ';' list   { 
      y_debug_console("[debug][y][list][stmt ; list]"); 
      if ($3 == NULL) $$ = $1; 
      else $$ = newast(pp, 'L', $1, $3);
    }
;

exp: exp CMP exp            { $$ = newcmp(pp, $2, $1, $3);      y_debug_console("[debug][y][exp][exp CMD exp][%c][%d][%c]", $1->nodetype, $2, $3->nodetype); }
  | exp '+' exp             { $$ = newast(pp, '+', $1, $3);     y_debug_console("[debug][y][exp][exp + exp][%c][%c]", $1->nodetype, $3->nodetype); }
  | exp '-' exp             { $$ = newast(pp, '-', $1, $3);     y_debug_console("[debug][y][exp][exp - exp][%c][%c]", $1->nodetype, $3->nodetype); }
  | exp '*' exp             { $$ = newast(pp, '*', $1, $3);     y_debug_console("[debug][y][exp][exp * exp][%c][%c]", $1->nodetype, $3->nodetype); }
  | exp '/' exp             { $$ = newast(pp, '/', $1, $3);     y_debug_console("[debug][y][exp][exp / exp][%c][%c]", $1->nodetype, $3->nodetype); }
  | '|' exp                 { $$ = newast(pp, '|', $2, NULL);   y_debug_console("[debug][y][exp][| exp][%c]", $2->nodetype); }
  | '(' exp ')'             { $$ = $2;                          y_debug_console("[debug][y][exp][( exp) ][%c]", $2->nodetype); }
  | '-' exp %prec UMINUS    { $$ = newast(pp, 'M', $2, NULL);   y_debug_console("[debug][y][exp][- exp][%c]", $2->nodetype); }
  | NUMBER                  { $$ = newnum(pp, $1);              y_debug_console("[debug][y][exp][NUMBER][%f]", $1); }
  | NAME                    { $$ = newref(pp, $1);              y_debug_console("[debug][y][exp][NAME][%s][%f]", $1->name, $1->value); }
  | NAME '=' exp            { $$ = newasgn(pp, $1, $3);         y_debug_console("[debug][y][exp][NAME = exp][%s][%c]", $1->name, $3->nodetype); }
  | FUNC '(' explist ')'    { $$ = newfunc(pp, $1, $3);         y_debug_console("[debug][y][exp][FUNC ( explist )][%d]", $1); }
  | NAME '(' explist ')'    { $$ = newcall(pp, $1, $3);         y_debug_console("[debug][y][exp][NAME ( explist )][%s]", $1->name); }
;

explist: exp { y_debug_console("[debug][y][explist][exp]"); }
  | exp ',' explist { $$ = newast(pp, 'L', $1, $3); y_debug_console("[debug][y][explist][exp, explist]"); }
;

symlist: NAME         { $$ = newsymlist(pp, $1, NULL); y_debug_console("[debug][y][symlist][NAME]"); }
  | NAME ',' symlist  { $$ = newsymlist(pp, $1, $3); y_debug_console("[debug][y][symlist][NAME , symlist]"); }
;

calc: EOL { /* nothing */ printf("\n"); y_debug_console("[debug][y][calc][null]"); pp->ast = NULL; YYACCEPT; }
  |  stmt EOL {
      y_debug_console("[debug][y][calc][calclist stmt EOL]"); 
      pp->ast = $1; YYACCEPT;
    }
  | LET NAME '(' symlist ')' '=' list EOL {
      y_debug_console("[debug][y][calc][calclist LET NAME ( symlist ) = list EOL]");
      dodef(pp, $2, $4, $7);
      printf("%d: Defined %s\n", yyget_lineno(pp->scaninfo), $2->name);
      pp->ast = NULL; YYACCEPT;
    }
;

%%
