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
	if (lookahead == '+') {
		match('+'); T(); R();
	} else if (lookahead == '-') {
		match('-'); T(); R();
	} else {
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
	if (lookahead == '*') {
		match('*'); F(); Q();
	} else if (lookahead == '/') {
		match('/'); F(); Q();
	} else {
		;
	}
}

/* F -> ID | DEC | ( E ) */
void F(void)
{
	if (lookahead == ID) {
		match(ID);
	} else if (lookahead == DEC) {
		match(DEC);
	} else {
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
