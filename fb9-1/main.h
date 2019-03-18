#ifndef __MAIN_H__
#define __MAIN_H__ 1

/*
  Example 2-1. Word count, reading one file fb2-1.l
  even more like Unix wc
*/
/*
  Example 9-1. Pure version of word count program
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
void yyerror(const char *s, ...);

extern int yydebug;

struct pwc {
  int chars;
  int words;
  int lines;
};

#endif