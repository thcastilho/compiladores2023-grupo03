/*@<lexer.c>::*/

#include <stdio.h>
#include <ctype.h>
#include <tokens.h>
#include <lexer.h>

/*
 * ID = [A-Za-z][A-Za-z0-9]*
 */
int isID(FILE *tape)
{
	int head;

	head = getc(tape);

	if (isalpha(head)) {
		while(isalnum(head = getc(tape)));
		ungetc(head, tape);
		return ID;
	}
	
	ungetc(head, tape);
	return 0;
}

/*
 * DEC = [1-9][0-9]* | 0
 */
int isDEC(FILE *tape)
{
	int head;

	head = getc(tape);

	if (isdigit(head)) {
		if (head == '0') {
			return DEC;
		}
		while(isdigit(head = getc(tape)));
		ungetc(head, tape);
		return DEC;
	}
	ungetc(head, tape);
	return 0;
}

void skipspaces(FILE *tape)
{
	int head;

	while(isspace(head = getc(tape)));

	ungetc(head, tape);
}

int gettoken(FILE *source)
{
	int token;
	skipspaces(source);
	if ( (token = isID(source)) ) return token;
	if ( (token = isDEC(source)) ) return token;
	return getc(source);
}
