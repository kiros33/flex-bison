#  include "test.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "need filename\n");
    return 1;
  }

  if (newfile(argv[1])) yylex();
}