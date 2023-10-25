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
	if (lookahead == '-') {
		match(lookahead); /**/printf("- ");/**/
	} else if(lookahead == '+') {
		match(lookahead); /**/printf("+ ");/**/
	}
	
	T();

	while(lookahead == '+' || lookahead == '-') 
	{
		if (lookahead == '+') {
			match(lookahead); T(); /**/printf("+ ");/**/
		} else if(lookahead == '-') {
			match(lookahead); T(); /**/printf("- ");/**/
		}
		
	}
}
	
/* T -> F{['*''/']F} */
void T(void)
{ 
	F();
	while(lookahead == '*' || lookahead == '/') 
	{
		if (lookahead == '*') {
			match(lookahead); F(); /**/printf("* ");/**/
		} else if (lookahead == '/') {
			match(lookahead); F(); /**/printf("/ ");/**/
		}
	}
}

/* F -> ID | DEC | ( E ) */
void F(void)
{
	switch(lookahead)
	{
		case ID:
			/**/printf("%s ", lexeme);/**/
			match(ID);
			break;
		case DEC:
			/**/printf("%s ", lexeme);/**/
			match(DEC);
			break;
		case OCT:
			/**/printf("%s ", lexeme);/**/
			match(OCT);
			break;
		case HEX:
			/**/printf("%s ", lexeme);/**/
			match(HEX);
			break;
		case FLT:
			/**/printf("%s ", lexeme);/**/
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
