/*
  Example 3-6. Advanced calculator parser fb3-2.y
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

%start calclist

%%

stmt: IF exp THEN list          { $$ = newflow('I', $2, $4, NULL); y_debug_console("[debug][y][stmt][IF exp THEN list]"); }
  | IF exp THEN list ELSE list  { $$ = newflow('I', $2, $4, $6); y_debug_console("[debug][y][stmt][IF exp THEN list ELSE list]"); }
  | WHILE exp DO list           { $$ = newflow('W', $2, $4, NULL); y_debug_console("[debug][y][stmt][WHILE exp DO list"); }
  | exp                         { y_debug_console("[debug][y][stmt][exp]"); }
;

list: /* nothing */ { $$ = NULL; y_debug_console("[debug][y][list][null]"); }
  | stmt ';' list   { 
      y_debug_console("[debug][y][list][stmt ; list]"); 
      if ($3 == NULL) $$ = $1; 
      else $$ = newast('L', $1, $3);
    }
;

exp: exp CMP exp            { $$ = newcmp($2, $1, $3);      y_debug_console("[debug][y][exp][exp CMD exp][%c][%d][%c]", $1->nodetype, $2, $3->nodetype); }
  | exp '+' exp             { $$ = newast('+', $1, $3);     y_debug_console("[debug][y][exp][exp + exp][%c][%c]", $1->nodetype, $3->nodetype); }
  | exp '-' exp             { $$ = newast('-', $1, $3);     y_debug_console("[debug][y][exp][exp - exp][%c][%c]", $1->nodetype, $3->nodetype); }
  | exp '*' exp             { $$ = newast('*', $1, $3);     y_debug_console("[debug][y][exp][exp * exp][%c][%c]", $1->nodetype, $3->nodetype); }
  | exp '/' exp             { $$ = newast('/', $1, $3);     y_debug_console("[debug][y][exp][exp / exp][%c][%c]", $1->nodetype, $3->nodetype); }
  | '|' exp                 { $$ = newast('|', $2, NULL);   y_debug_console("[debug][y][exp][| exp][%c]", $2->nodetype); }
  | '(' exp ')'             { $$ = $2;                      y_debug_console("[debug][y][exp][( exp) ][%c]", $2->nodetype); }
  | '-' exp %prec UMINUS    { $$ = newast('M', $2, NULL);   y_debug_console("[debug][y][exp][- exp][%c]", $2->nodetype); }
  | NUMBER                  { $$ = newnum($1);              y_debug_console("[debug][y][exp][NUMBER][%f]", $1); }
  | NAME                    { $$ = newref($1);              y_debug_console("[debug][y][exp][NAME][%s][%f]", $1->name, $1->value); }
  | NAME '=' exp            { $$ = newasgn($1, $3);         y_debug_console("[debug][y][exp][NAME = exp][%s][%c]", $1->name, $3->nodetype); }
  | FUNC '(' explist ')'    { $$ = newfunc($1, $3);         y_debug_console("[debug][y][exp][FUNC ( explist )][%d]", $1); }
  | NAME '(' explist ')'    { $$ = newcall($1, $3);         y_debug_console("[debug][y][exp][NAME ( explist )][%s]", $1->name); }
;

explist: exp { y_debug_console("[debug][y][explist][exp]"); }
  | exp ',' explist { $$ = newast('L', $1, $3); y_debug_console("[debug][y][explist][exp, explist]"); }
;

symlist: NAME         { $$ = newsymlist($1, NULL); y_debug_console("[debug][y][symlist][NAME]"); }
  | NAME ',' symlist  { $$ = newsymlist($1, $3); y_debug_console("[debug][y][symlist][NAME , symlist]"); }
;

calclist: { /* nothing */ printf("\n"); y_debug_console("[debug][y][calclist][null]"); printf("> "); }
  | calclist stmt EOL {
      y_debug_console("[debug][y][calclist][calclist stmt EOL]"); 
      printf("= %4.4g\n> ", eval($2));
      treefree($2);
    }
  | calclist LET NAME '(' symlist ')' '=' list EOL {
      y_debug_console("[debug][y][calclist][calclist LET NAME ( symlist ) = list EOL]");
      dodef($3, $5, $8);
      printf("Defined %s\n> ", $3->name);
    }
  | calclist error EOL { y_debug_console("[debug][y][calclist][calclist err EOL]"); yyerrok; printf("> "); }
  | calclist EOL { y_debug_console("[debug][y][calclist][calclist EOL]"); /* ignore */ printf("> "); } /* 공백이나 개행문자만 들어간 경우 무시 */
;

%%
