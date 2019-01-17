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

%option noyywrap

/* %option c++ */

/* just like Unix wc */ 
%{
int chars = 0; 
int words = 0; 
int lines = 0;

int totchars = 0;
int totwords = 0;
int totlines = 0;
%}

%%

[a-zA-Z]+    { words++; chars += strlen(yytext); }
\n           { chars++; lines++; }
.            { chars++; }

%%

int main(int argc, char **argv)
{
  int i;

  if (argc < 2)
  { /* just read stdin */
    yylex();
    printf("%8d%8d%8d\n", lines, words, chars);
    return 0;
  }

  for (i = 1; i < argc; i++)
  {
    FILE *f = fopen(argv[i], "r");

    if (!f)
    {
      perror(argv[i]);
      return 1;
    }

    yyrestart(f);
    yylex();

    fclose(f);

    printf("%8d%8d%8d \t\t%s\n", lines, words, chars, argv[i]);

    totchars += chars;
    chars = 0;
    
    totwords += words;
    words = 0;

    totlines += lines;
    lines = 0;
  }

  if(argc > 1) /* print total if more than one file */
    printf("%8d%8d%8d total\n", totlines, totwords, totchars);

  return 0;
}