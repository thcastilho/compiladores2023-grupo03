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

int minus = 0; //variable to check if there is a '-' before

/* E -> T{['+''-']T} | [['-''+']]T{['+''-']T}*/
void E(void)
{

	int sign = 0; //keep the sign of {['+''-']T}
	if (isOMinus(lookahead)) //check the ominus optional condition
	{
		if (lookahead == '-') {
			minus = 1; //signed
		}
		match(lookahead);
	}
_T:
	T();

	if(sign) {
		/**/
		switch(sign){
			case '+':
				printf("+ ");
				break;
			case '-':
				printf("- ");
				break;
			default:
				;
		}
		/**/
	}

	if(isOPlus(lookahead)) //check the oplus optional condition (note that is a Kleene closure)
	{
		sign = lookahead;
		match(lookahead);
		goto _T; //return to the beginning of the label
	}
}
	
/* T -> F{['*''/']F} */
void T(void)
{ 
	int sign = 0; //keep the sign of {['+''-']F}
_F:
	F();

	if(sign) {
		/**/
		switch(sign){
			case '*':
				printf("* ");
				break;
			case '/':
				printf("/ ");
				break;
			default:
				;
		}
		/**/
	}

	if(isOTimes(lookahead)) //check the otimes optional condition (note that is a Kleene closure)
	{
		sign = lookahead;
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

	if(minus) { //if it is signed
		/**/printf("- ");/**/
		minus = 0; //guarantees that it will pass here just once
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
