/*
  Example 1-3. A simple flex scanner fb1-3.l
  recognize tokens for the calculator and print them out
*/

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

/* recognize tokens for the calculator and print them out */
%%
"+" { printf("PLUS\n"); }
"-" { printf("MINUS\n"); }
"*" { printf("TIMES\n"); }
"/" { printf("DIVIDE\n"); }
"|" { printf("ABS\n"); }
[0-9]+ { printf("NUMBER %s\n", yytext); }
\n { printf("NEWLINE\n"); }
[ \t] { }
. { printf("Mystery character %s\n", yytext); }
%%

int main(int argc, char **argv)
{
  yylex();

  return 0;
}

