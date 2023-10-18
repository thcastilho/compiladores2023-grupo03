/*@<parser.c>::*/

#include <stdio.h>
#include <stdlib.h>
#include <tokens.h>
#include <parser.h>
#include <lexer.h>

int lookahead;
int line_counter = 0;

int isOMinus(int check){
	if (check == '-' || check == '+') {
		return 1;
	} else return 0;
}

int isOPlus(int check){
	if (check == '+' || check == '-') {
		return 1;
	} else return 0;
}

int isOTimes(int check){
	if (check == '*' || check == '/') {
		return 1;
	} else return 0;
}

/* LR(1)-grammar for simple expressions - initial symbol: E
 *
 * E -> E + T | E - T | T
 * T -> T * F | T / F | F
 * F -> ID | DEC | ( E )
 *
 * LL(1)-grammar for simple expressions - initial symbol: E
 * E -> T R
 * R -> + T R | - T R | <epsilon>
 * T -> F Q
 * Q -> * F Q | / F Q | <epsilon>
 * F -> ID | DEC | ( E )
 */

/* E -> T{['+''-']T} | [['-''+']]T{['+''-']T}*/
void E(void)
{
	if (isOMinus(lookahead)) 
	{
		match(lookahead);
	}
_T:

_F:
	switch(lookahead)
	{
		case ID:
			match(ID);
			break;
		case DEC:
			match(DEC);
			break;
		case OCT:
			match(OCT);
			break;
		case HEX:
			match(HEX);
			break;
		case FLT:
			match(FLT);
			break;
		default:
			match('('); E(); match(')');
	}

	if(isOTimes(lookahead)) 
	{
		match(lookahead);
		goto _F;
	}

	if(isOPlus(lookahead)) 
	{
		match(lookahead);
		goto _T;
	}
}

void match(int expected)
{
	if (lookahead == expected) {
		lookahead = gettoken(src);
	} else if (lookahead == EOF) {
		fprintf(stderr, "premature EOF at line %d... exiting with error\n", line_counter);
		exit(-3);
	} else {
		fprintf(stderr, "token mismatch... exiting with error\n");
		exit(-2);
	}
}
