/*
  Example 3-7. Advanced calculator lexer fb3-2.l
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

"exit" { yyterminate(); } // exit 기능 추가

  /* single character ops */
"+" |
"-" |
"*" |
"/" |
"=" |
"|" |
"," |
";" |
"(" |
")" { return yytext[0]; }

  /* comparison ops, all are a CMP token */
">"  { yylval.fn = 1; return CMP; }
"<"  { yylval.fn = 2; return CMP; }
"<>" { yylval.fn = 3; return CMP; }
"==" { yylval.fn = 4; return CMP; }
">=" { yylval.fn = 5; return CMP; }
"<=" { yylval.fn = 6; return CMP; }

  /* keywords */
"if"    { return IF; }
"then"  { return THEN; }
"else"  { return ELSE; }
"while" { return WHILE; }
"do"    { return DO; }
"let"   { return LET ;}

  /* built-in functions */
"sqrt"  { yylval.fn = B_sqrt; return FUNC; }
"exp"   { yylval.fn = B_exp; return FUNC; }
"log"   { yylval.fn = B_log; return FUNC; }
"print" { yylval.fn = B_print; return FUNC; }

  /* names */
[a-zA-Z][a-zA-Z0-9]* { yylval.s = lookup(yytext); return NAME; }

[0-9]+"."[0-9]*{EXP}? |
"."?[0-9]+{EXP}? { yylval.d = atof(yytext); return NUMBER; }

"//".*

[ \t] { /* ignore whitespace */ }

\\\n { printf("c> "); } /* ignore line continuation */

\n { return EOL; }

. { yyerror("Mystery character %c\n", *yytext); }

%%
