/*
  Example 3-7. Advanced calculator lexer fb3-2.l
  recognize tokens for the calculator
*/
/*
  Example 9-3. Reentrant calculator scanner purecalc.l
*/

/* generate header file */
%option header-file = "scanner.h"

/* generate implementation file */
%option outfile = "scanner.cpp"

%option always-interactive nounistd

/* recognize tokens for the calculator */
/* pure scanner and parser version */
/* $Header: /usr/home/johnl/flnb/RCS/ch09.tr,v 1.4 2009/05/19 18:28:27 johnl Exp $ */

%option noyywrap nodefault yylineno reentrant bison-bridge

%option extra-type="struct pcdata *"

%x COMMENT

%top{

#pragma warning(disable: 4819)

#ifdef _MSC_VER
  /* fopen 경고 없애기 */
  #define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdarg.h>

extern int debug_console(const char *, ...);
extern int l_debug_console(const char *, ...);

}

%{
  #include "parser.h"
  #include "main.h" 
%}

/* float exponent */
EXP ([Ee][-+]?[0-9]+)

%%

%{
struct pcdata *pp = yyextra;
%}

"exit" { yyterminate(); } // exit 기능 추가

  /* environment variables */
"debug_flag"([ \t]+("on"|"off"))? { if(strstr(yytext + 10, "off")) debug_flag = false; else if (strstr(yytext + 10, "on")) debug_flag = true; l_debug_console("[debug][l][debug_flag][%s]", ((debug_flag) ? "true" : "false")); }
"l_debug_flag"([ \t]+("on"|"off"))? { if(strstr(yytext + 12, "off")) l_debug_flag = false; else if (strstr(yytext + 12, "on")) l_debug_flag = true; l_debug_console("[debug][l][l_debug_flag][%s]", ((l_debug_flag) ? "true" : "false")); }
"y_debug_flag"([ \t]+("on"|"off"))? { if(strstr(yytext + 12, "off")) y_debug_flag = false; else if (strstr(yytext + 12, "on")) y_debug_flag = true; l_debug_console("[debug][l][y_debug_flag][%s]", ((y_debug_flag) ? "true" : "false")); }

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
")" { l_debug_console("[debug][l][single char operator][%c][%d]", yytext[0], yytext[0]); return yytext[0]; }

  /* comparison ops, all are a CMP token */
">"  { yylval->fn = 1; l_debug_console("[debug][l][CMP][%s][%d]", yytext, yylval->fn); return CMP; }
"<"  { yylval->fn = 2; l_debug_console("[debug][l][CMP][%s][%d]", yytext, yylval->fn); return CMP; }
"<>" { yylval->fn = 3; l_debug_console("[debug][l][CMP][%s][%d]", yytext, yylval->fn); return CMP; }
"==" { yylval->fn = 4; l_debug_console("[debug][l][CMP][%s][%d]", yytext, yylval->fn); return CMP; }
">=" { yylval->fn = 5; l_debug_console("[debug][l][CMP][%s][%d]", yytext, yylval->fn); return CMP; }
"<=" { yylval->fn = 6; l_debug_console("[debug][l][CMP][%s][%d]", yytext, yylval->fn); return CMP; }

  /* keywords */
"if"    { l_debug_console("[debug][l][IF]");     return IF; }
"then"  { l_debug_console("[debug][l][THEN]");   return THEN; }
"else"  { l_debug_console("[debug][l][ELSE]");   return ELSE; }
"while" { l_debug_console("[debug][l][WHILE]");  return WHILE; }
"do"    { l_debug_console("[debug][l][DO]");     return DO; }
"let"   { l_debug_console("[debug][l][LET]");    return LET ;}

  /* built-in functions */
"sqrt"  { yylval->fn = B_sqrt;  l_debug_console("[debug][l][FUNC][%s][%d]", yytext, yylval->fn); return FUNC; }
"exp"   { yylval->fn = B_exp;   l_debug_console("[debug][l][FUNC][%s][%d]", yytext, yylval->fn); return FUNC; }
"log"   { yylval->fn = B_log;   l_debug_console("[debug][l][FUNC][%s][%d]", yytext, yylval->fn); return FUNC; }
"print" { yylval->fn = B_print; l_debug_console("[debug][l][FUNC][%s][%d]", yytext, yylval->fn); return FUNC; }

  /* names */
[a-zA-Z][a-zA-Z0-9_-]* { yylval->s = lookup(pp, yytext); l_debug_console("[debug][l][NAME][%s][%f]", yytext, yylval->s->value); return NAME; }

[0-9]+"."[0-9]*{EXP}? |
"."?[0-9]+{EXP}? { yylval->d = atof(yytext); l_debug_console("[debug][l][NUMBER][%s][%f]", yytext, yylval->d); return NUMBER; }


  /* comments */
"/*"                        { BEGIN(COMMENT); l_debug_console("[debug][l][COMMENT][begin][%s]", yytext); }
<COMMENT>"*/"               { BEGIN(INITIAL); l_debug_console("[debug][l][COMMENT][end][%s]", yytext); }
<COMMENT>([^*]|\n|\*[^/])+  { /* ignore */ l_debug_console("[debug][l][COMMENT][contents][%s]", yytext); }
<COMMENT><<EOF>>            { debug_console("==> %d: Unterminated comment\n", yylineno); return 0; }

"//".* { l_debug_console("[debug][l][COMMENT][inline][%s]", yytext); }

[ \t] { /* ignore whitespace */ }

\\\n { printf("c> "); } /* ignore line continuation */

\n { l_debug_console("[debug][l][EOL]"); return EOL; }

. { yyerror(pp, "Mystery character %c\n", *yytext); }

%%
