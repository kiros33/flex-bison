/*
  Example 2-1. Word count, reading one file fb2-1.l
  even more like Unix wc
*/
/*
  Example 9-1. Pure version of word count program
*/

/* generate header file */
%option header-file = "scanner.h"

/* generate implementation file */
%option outfile = "scanner.cpp"

%option always-interactive nounistd

/* pure version of word count program */
%option noyywrap nodefault yylineno reentrant

%top{

#pragma warning(disable: 4819)

#ifdef _MSC_VER
  #pragma warning(disable: 4005)
  /* 윈도우에서 Flex만 사용할 경우 unistd.h 참조 오류가 발생 */
  #define YY_NO_UNISTD_H
#endif

#ifdef _MSC_VER
  /* fopen 경고 없애기 */
  #define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdarg.h>
#include <string.h>

extern int debug_console(const char *, ...);
extern int l_debug_console(const char *, ...);

}

/* just like Unix wc */ 
%{

#include "main.h" 

%}

%option extra-type="struct pwc *"

%%

%{

struct pwc *pp = yyextra;

%}

[a-zA-Z]+    { pp->words++; pp->chars += (int)strlen(yytext); }
\n           { pp->chars++; pp->lines++; }
.            { pp->chars++; }

%%

/* code */