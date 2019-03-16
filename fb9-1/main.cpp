/*
  Example 9-1. Pure version of word count program
*/

#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "scanner.h"
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

  fprintf(stderr, "%d: error: ", 0);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");

  va_end(ap);
}

/*
        -d              Debug
        -f[=]textfile   Input file for word count
*/

int main(int argc, char **argv) {
  int index;
  int optc;
  int result_flag = 0;

  opterr = 0;

  int quit_flag = 0;


  FILE *fp;

  struct pwc mypwc = { 0, 0, 0 };   /* my instance data */
  yyscan_t scanner;                 /* flex instance data */

  if(yylex_init_extra(&mypwc, &scanner)) { 
    perror("init alloc failed");
    return 1;
  }

  while ((optc = getopt(argc, argv, "df:")) != -1) {
    debug_console("[debug][main][options][%c]", optc);
    switch (optc) {
      case 'f':
        debug_console("[%s option]+[%s]", "text input", optarg);

        debug_console("[%s]", "opening");
        if(!(fp = fopen(optarg, "r"))) {
          debug_console("[%s]", "error");
          perror(optarg);
          exit(1);
        }
        debug_console("[%s]", "opened");
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
        if (optopt == 'f') {
          fprintf(stderr, "option -%c requires text filename\n", optopt);
        } else if (isprint(optopt)) {
          fprintf(stderr, "Unknown option `-%c`\n", optopt);
        } else {
          fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
        }
        quit_flag++;
        break;
      default:
        debug_console("[%s][%c]", "default", optc);
        abort();
    }
    debug_console("\n");
    if(quit_flag > 0) return 1;
  }

  for (index = optind; index < argc; index++) {
    fprintf(stderr, "Not supported option argument %s\n", argv[index]);
  }

  if (fp) {
    yyset_in(fp, scanner);
  }
  else {
    yyset_in(stdin, scanner);
  }

  result_flag = yylex(scanner);
  
  if (!result_flag) {
    printf("Success: %8d%8d%8d\n", mypwc.lines, mypwc.words, mypwc.chars); 
  }
  else {
    printf("Failure: count failed!\n");
  }

  if(fp) {
    debug_console("[debug][main][%s]", "file");
    debug_console("[%s]", "closing");
    fclose(yyget_in(scanner));
    debug_console("[%s]", "closed");
    debug_console("\n");
  }
  else {
    debug_console("[debug][main][%s]", "stdin");
    debug_console("[%s]", "closed");
    debug_console("\n");
  }

  yylex_destroy(scanner);

  return result_flag;
}