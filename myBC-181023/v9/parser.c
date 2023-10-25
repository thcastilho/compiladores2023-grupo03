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
 * F -> ID | DEC | ( E )
 *
 * LL(1)-grammar for simple expressions - initial symbol: E
 * E -> T R
 * R -> + T R | - T R | <epsilon>
 * T -> F Q
 * Q -> * F Q | / F Q | <epsilon>
 * F -> ID | DEC | ( E )
 */

int minus = 0; //variavel booleana para saber se vem '-' antes 

/* E -> T{['+''-']T} | [['-''+']]T{['+''-']T}*/
void E(void)
{

	int sign = 0; //guarda o sinal de {['+''-']T}
	if (isOMinus(lookahead)) 
	{
		if (lookahead == '-') {
			minus = 1; //existe sinal
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

	if(isOPlus(lookahead)) 
	{
		sign = lookahead;
		match(lookahead);
		goto _T;
	}
}
	
/* T -> F{['*''/']F} */
void T(void)
{ 
	int sign = 0;
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

	if(isOTimes(lookahead)) 
	{
		sign = lookahead;
		match(lookahead);
		goto _F;
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

	if(minus) { //se ha sinal '-'
		/**/printf("- ");/**/
		minus = 0; //isso garante que so vai passar aqui uma vez
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
