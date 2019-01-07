/* generate header file */
%defines "parser.h"

/* the parser implementation file name */
%output "parser.cpp"

/* Visual Studio 빌드시 라이브러리 추가
   C:\cygwin64\usr\local\lib\liby.a
   C:\cygwin64\usr\local\lib\libfl.a
   64비트만 빌드됨
*/
/* %option noyywrap */

/* %option c++ */

/* simplest version of calculator */
%{
	#include <stdio.h>
%}

%code {
	extern int yylex();
	void yyerror (const char*);
}

/* declare tokens */
%token NUMBER
%token ADD SUB MUL DIV ABS
%token EOL

%%

calclist: /* nothing */
	| calclist exp EOL { printf("= %d\n", $2); }
	;

exp: factor { $$ = $1; }
	| exp ADD factor { $$ = $1 + $3; }
	| exp SUB factor { $$ = $1 - $3; }
	;

factor: term { $$ = $1; }
	| factor MUL term { $$ = $1 * $3; }
	| factor DIV term { $$ = $1 / $3; }
	;

term: NUMBER { $$ = $1; }
	| ABS term { $$ = ($2 >= 0) ? $2 : -$2; }
	;

%%

int main(int argc, char **argv)
{
	yyparse();
	return 0;
}

void yyerror(const char *s)
{
	fprintf(stderr, "error: %s\n", s);
}