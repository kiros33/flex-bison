#ifndef __TEST_H__
#define __TEST_H__

struct bufstack
{
struct bufstack *prev;    /* previous entry */
YY_BUFFER_STATE bs;       /* saved buffer */
int lineno;               /* saved line number */
char *filename;           /* name of this file */
FILE *f;                  /* current file */
} *curbs = 0;

char *curfilename;          /* name of current input file */

int newfile(char *fn);
int popfile(void);

#endif