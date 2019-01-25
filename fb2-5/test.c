#include "test.h"

/* test.c
 * c source code for parsing test
 */

int main(int argc, char **argv) {
  if (argc < 2) { // check arguments length
    fprintf(stderr, "need filename\n");
    return 1;
  }

  if (newfile(argv[1])) yylex();

  return 0;
}