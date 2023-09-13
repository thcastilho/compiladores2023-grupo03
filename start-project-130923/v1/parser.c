/*@<parser.c>::*/

#include <stdio.h>
#include <stdlib.h>
#include <tokens.h>
#include <parser.h>

int lookahead;

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

/* E -> T R */
void E(void)
{
	T(); R();
}

/* R -> + T R | - T R | <epsilon> */
void R(void)
{
	switch(lookahead)
	{
		case '+':
			match('+'); T(); R();
			break;
		case '-':
			match('-'); T(); R();
			break;
		default:
			;
	}
}
		
/* T -> F Q */
void T(void)
{
	F(); Q();
}

/* Q -> * F Q | / F Q | <epsilon> */
void Q(void)
{
	switch(lookahead)
	{
		case '*':
			match('*'); F(); Q();
			break;
		case '/':
			match('/'); F(); Q();
			break;
		default:
			;
	}
}

/* F -> ID | DEC | ( E ) */
void F(void)
{
	switch(lookahead)
	{
		case ID:
			match(ID);
			break;
		case DEC:
			match(DEC);
			break;
		default:
			match('('); E(); match(')');
	}
}

void match(int expected)
{
	if (lookahead == expected) {
		lookahead = gettoken(src);
	} else {
		fprintf(stderr, "token mismatch... exiting with error\n");
		exit(-2);
	}
}
