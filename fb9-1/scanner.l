/*
  Example 2-1. Word count, reading one file fb2-1.l
  even more like Unix wc
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

%{

#include "main.h" 

%}

/* just like Unix wc */ 
%{
extern int chars; 
extern int words;
extern int lines;
%}

%%

[a-zA-Z]+    { words++; chars += (int)strlen(yytext); }
\n           { chars++; lines++; }
.            { chars++; }

%%

/* code */

void ppp(void) { }