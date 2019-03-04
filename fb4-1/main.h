#ifndef __MAIN_H__
#define __MAIN_H__ 1

/*
  Example 3-5. Advanced calculator header fb3-2.h
  Declarations for a calculator fb3-1
*/

#ifdef _MSC_VER
  /* strdup 경고 없애기 */
  #define strdup _strdup
#endif

#ifdef _MSC_VER
  /* fopen 경고 없애기 */
  #define _CRT_SECURE_NO_WARNINGS
#endif

//#define YYDEBUG 1

extern bool debug_flag;
extern bool l_debug_flag;
extern bool y_debug_flag;

/* interface to the lexer */
extern int yylineno; /* from lexer */
extern char *yytext; /* from lexer */
void yyerror(const char *s, ...);

extern int yydebug;

#endif