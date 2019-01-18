/*
  Example 1-6. Calculator scanner fb1-5.y
*/

/* generate header file */
%defines "parser.h"

/* the parser implementation file name */
%output "parser.cpp"

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
%token OP CP

%%

calclist: /* nothing */
	| calclist exp EOL { printf("= %d\n", $2); }
	;

exp: factor
	| exp ADD factor { $$ = $1 + $3; }
	| exp SUB factor { $$ = $1 - $3; }
	;

factor: term
	| factor MUL term { $$ = $1 * $3; }
	| factor DIV term { $$ = $1 / $3; }
	;

term: NUMBER
	| ABS term { $$ = ($2 >= 0) ? $2 : -$2; }
	| OP exp CP { $$ = $2; }
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