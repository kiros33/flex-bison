/*
  Example 3-8. Advanced calculator helper functions fb3-2func.c
  helper functions for fb3-2
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
//#include <unistd.h>
#include <ctype.h>
#include "main.h"
#include "parser.h"
#include "wingetopt.h"

bool debug_flag = false;
bool l_debug_flag = false;
bool y_debug_flag = false;

int debug_console(const char *format, ...) {
  va_list arg;
  int done = 0;

  if(debug_flag) {
    va_start(arg, format);
    done = vfprintf(stdout, format, arg);
    va_end(arg);
  }

  return done;
}

int l_debug_console(const char *format, ...) {
  va_list arg;
  int done = 0;

  if(l_debug_flag) {
    va_start(arg, format);
    done = vfprintf(stdout, format, arg);
    printf("\n");
    va_end(arg);
  }

  return done;
}

int y_debug_console(const char *format, ...) {
  va_list arg;
  int done = 0;

  if(y_debug_flag) {
    va_start(arg, format);
    done = vfprintf(stdout, format, arg);
    printf("\n");
    va_end(arg);
  }

  return done;
}

void yyerror(const char *s, ...) {
  va_list ap;
  va_start(ap, s);

  fprintf(stderr, "%d: error: ", yylineno);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");

  va_end(ap);
}

/*
        -d           Debug
        -f[=]sqlfile Input file, SQL Syntax
*/

int main(int argc, char **argv) {
  extern FILE *yyin;

  int index;
  int c;
  int result_flag = 0;

  opterr = 0;

  int quit_flag = 0;

  while ((c = getopt(argc, argv, "df:")) != -1) {
    debug_console("[debug][main][options][%c]", c);
    switch (c) {
      case 'f':
        debug_console("[%s option]+[%s]", "sql input", optarg);

        yyin = fopen(optarg, "r");

        if(yyin == NULL) { 
          perror(optarg); 
          exit(1); 
        }
        break;
      case 'd':
        //yydebug = 1;
        debug_flag = true;
        l_debug_flag = true;
        y_debug_flag = true;
        debug_console("[%s option]", "debug");
        break;
      case '?':
        debug_console("-->");
        if (optopt == 'o') {
          fprintf(stderr, "option -%c requires log filename\n", optopt);
        } else if (optopt == 'b') {
          fprintf(stderr, "option -%c requires binary filename\n", optopt);
        } else if (isprint(optopt)) {
          fprintf(stderr, "Unknown option `-%c`\n", optopt);
        } else {
          fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
        }
        quit_flag++;
        break;
      default:
        debug_console("[%s][%c]", "default", c);
        abort();
    }
    debug_console("\n");
    if(quit_flag > 0) return 1;
  }

  for (index = optind; index < argc; index++) {
    fprintf(stderr, "Not supported option argument %s\n", argv[index]);
  }

  result_flag = yyparse();
  
  if (!result_flag) {
    printf("SQL parse worked\n");
  }
  else {
    printf("SQL parse failed\n");
  }

  return result_flag;
}