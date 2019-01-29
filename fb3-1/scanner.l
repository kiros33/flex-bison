/*
  Example 3-3. Lexer for AST calculator
  recognize tokens for the calculator
*/

/* generate header file */
%option header-file = "scanner.h"

/* generate implementation file */
%option outfile = "scanner.cpp"

%option always-interactive nounistd

%option noyywrap nodefault yylineno

%top{

#pragma warning(disable: 4819)

#ifdef _MSC_VER
  #define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdarg.h>

int debug_console(const char *, ...);

}

%{
  #include "main.h" 
  #include "parser.h"
%}

/* float exponent */
EXP ([Ee][-+]?[0-9]+)

%%

"+" |
"-" |
"*" |
"/" |
"|" |
"(" |
")" { return yytext[0]; }

[0-9]+"."[0-9]*{EXP}? |
"."?[0-9]+{EXP}? { yylval.d = atof(yytext); return NUMBER; }

\n { return EOL; }

"//".*

[ \t] { /* ignore whitespace */ }

. { yyerror("Mystery character %c\n", *yytext); }

%%
