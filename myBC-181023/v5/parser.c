/*@<parser.c>::*/

#include <stdio.h>
#include <stdlib.h>
#include <tokens.h>
#include <parser.h>
#include <lexer.h>

int lookahead;

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
 */

/* E -> T{['+''-']T} | [['-''+']]T{['+''-']T}*/
void E(void)
{
	if (isOMinus(lookahead)) //check the ominus optional condition
	{
		match(lookahead);
	}
_T:
	T();
	if(isOPlus(lookahead)) //check the oplus optional condition (note that is a Kleene closure)
	{
		match(lookahead);
		goto _T; //return to the beginning of the label
	}
}
	
/* T -> F{['*''/']F} */
void T(void)
{ 
_F:
	F();
	if(isOTimes(lookahead)) //check the otimes optional condition (note that is a Kleene closure)
	{
		match(lookahead);
		goto _F; //return to the beginning of the label
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
