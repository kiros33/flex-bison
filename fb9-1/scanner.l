/*
  Example 2-1. Word count, reading one file fb2-1.l
  even more like Unix wc
*/

/* generate header file */
%option header-file = "scanner.h"

/* generate implementation file */
%option outfile = "scanner.cpp"

%option always-interactive nounistd

%option noyywrap

/* just like Unix wc */ 
%{
extern int chars; 
extern int words;
extern int lines;
%}

%%

[a-zA-Z]+    { words++; chars += strlen(yytext); }
\n           { chars++; lines++; }
.            { chars++; }

%%

/* code */