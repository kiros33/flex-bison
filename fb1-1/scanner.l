/* generate header file */
%option header-file = "scanner.h"

/* generate implementation file */
%option outfile = "scanner.cpp"

%option always-interactive nounistd

/* Visual Studio 빌드시 라이브러리 추가
   C:\cygwin64\usr\local\lib\liby.a
   C:\cygwin64\usr\local\lib\libfl.a
   64비트만 빌드됨
*/
/* %option noyywrap */

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