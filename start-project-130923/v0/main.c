/*@<main.c>::*/

#include <stdio.h>
#include <stdlib.h>
#include <main.h>

FILE *src;

int main(void)
{
	src = stdin;

	lookahead = gettoken(src);

	E();

	exit(0);
}
