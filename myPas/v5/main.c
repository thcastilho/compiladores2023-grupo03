/*@<main.c>::*/

#include <stdio.h>
#include <stdlib.h>
#include <main.h>

FILE *src;

int main(int argc, char *argv[])
{
	if (argc > 1) {
		src = fopen(argv[1], "r"); //now, the program is able to open extern .txt files

		if (src == NULL) { //error opening the file
			exit(-1);
		}
	} else {
		src = stdin;
	}

	lookahead = gettoken(src);

	mypas();

	exit(0);
}
