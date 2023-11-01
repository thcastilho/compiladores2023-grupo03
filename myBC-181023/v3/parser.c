/*@<parser.c>::*/

#include <stdio.h>
#include <stdlib.h>
#include <tokens.h>
#include <parser.h>
#include <lexer.h>

int lookahead;

/* LR(1)-grammar for simple expressions - initial symbol: E
 *
 * E -> E + T | E - T | T
 * T -> T * F | T / F | F
 * F -> ID | NUM | ( E )
 *
 * LL(1)-grammar for simple expressions - initial symbol: E
 * E -> T R
 * R -> + T R | - T R | <epsilon>
 * T -> F Q
 * Q -> * F Q | / F Q | <epsilon>
 * F -> ID | NUM | ( E )
 * 
 * EBNF-grammar for simples expressions - initial symbol: E
 * E -> T{['+''-']T} | [['-''+']]T{['+''-']T}
 * T -> F{['*''/']F}
 * F -> ID | NUM | ( E )
 * 
 */

/* E -> T{['+''-']T} | [['-''+']]T{['+''-']T}*/
void E(void)
{
	if (lookahead == '-' || lookahead == '+') 
	{
		match(lookahead);
	}
	T();
	while(lookahead == '+' || lookahead == '-') 
	{
		match(lookahead); T();
	}
}
	
/* T -> F{['*''/']F} */
void T(void)
{ 
	F();
	while(lookahead == '*' || lookahead == '/') 
	{
		match(lookahead); F();
	}
}

/* F -> ID | NUM | ( E ) */
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
