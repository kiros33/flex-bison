/* generate header file */
%option header-file = "fb1-1.h"

/* generate implementation file */
%option outfile = "fb1-1.cpp"

/* %option c++ */

/* just like Unix wc */ 
%{
int chars = 0; 
int words = 0; 
int lines = 0;
%}

%%

[a-zA-Z]+    { words++; chars += strlen(yytext); }
\n           { chars++; lines++; }
.            { chars++; }

%%

int main(int argc, char **argv)
{
  yylex();
  printf("%8d%8d%8d\n", lines, words, chars); 

  return 0;
}