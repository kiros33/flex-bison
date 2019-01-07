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

%{
	enum yytokentype
	{
		NUMBER = 258,
		ADD = 259,
		SUB = 260,
		MUL = 261,
		DIV = 262,
		ABS = 263,
		EOL = 264
	};
	
	int yylval;
%}

%%
"+" { return ADD; }
"-" { return SUB; }
"*" { return MUL; }
"/" { return DIV; }
"|" { return ABS; }
[0-9]+ { yylval = atoi(yytext); return NUMBER; }
\n { return EOL; }
[ \t] { /* ignore whitespace */ }
. { printf("Mystery character %c\n", *yytext); }
%%

int main(int argc, char **argv)
{
	int tok;
	
	while((tok = yylex()))
	{
		printf("%d", tok);
		if(tok == NUMBER)
			printf(" = %d\n", yylval);
		else
			printf("\n");
	}
}