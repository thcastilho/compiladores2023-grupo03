/*@<parser.c>::*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
 */

/*1*/ int ominus = 0; /**/ //variable to check if there is a '-' before  

/* E -> T{['+''-']T} | [['-''+']]T{['+''-']T}*/
void E(void)
{
	
	if(lookahead == '+' || lookahead == '-') {
		/**/if(lookahead == '-') ominus = 1;/**/
		match(lookahead);
	}
	
	T();

	while(lookahead == '+' || lookahead == '-') 
	{
		if (lookahead == '+') {
			/*3*/
			printf("\tPush(Acc)\n");
			match(lookahead); 
			T();
			/**/printf("\tstack[sp] = stack[sp] + Acc\n");/**/ 
			/**/printf("\tAcc = Pop()\n");/**/
			/**/
		} else if(lookahead == '-') {
			/**/
			printf("\tPush(Acc)\n");
			match(lookahead); 
			T(); 
			/**/printf("\tstack[sp] = stack[sp] - Acc\n");/**/ 
			/**/printf("\tAcc = Pop()\n");/**/
			/**/
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
			printf("\tPush(Acc)\n");
			match(lookahead); 
			F(); 
			/**/printf("\tstack[sp] = stack[sp] * Acc\n");/**/ 
			/**/printf("\tAcc = Pop()\n");/**/
			/**/
		} else if (lookahead == '/') {
			printf("\tPush(Acc)\n");
			match(lookahead); 
			F(); 
			/**/printf("\tstack[sp] = stack[sp] / Acc\n");/**/ 
			/**/printf("\tAcc = Pop()\n");/**/
			/**/
		}
	}
}

/* F -> ID | NUM | ( E ) */
void F(void)
{
	/**/char varname[MAXIDLEN+1];/**/
	
	switch(lookahead)
	{
		case ID:
			/**/strcpy(varname, lexeme);/**/
			match(ID);
			
			if(lookahead == ASGN) {
				//L-value
				match(ASGN); E();
				/**/printf("\tStore(%s, Acc)\n", varname);/**/
			} else {
				// R-Value
				/**/printf("\tAcc = Recall(%s)\n", varname);/**/
		}
			break;
		case DEC:
			/**/printf("\tAcc = atof(%s)\n", lexeme);/**/
			match(DEC);
			break;
		case OCT:
			/**/printf("\tAcc = atof(%s)\n", lexeme);/**/
			match(OCT);
			break;
		case HEX:
			/**/printf("\tAcc = atof(%s)\n", lexeme);/**/
			match(HEX);
			break;
		case FLT:
			/**/printf("\tAcc = atof(%s)\n", lexeme);/**/
			match(FLT);
			break;
		default:
			match('('); E(); match(')');
	}

	/*2*/
	if(ominus) { //if it is signed
		printf("\tAcc = -Acc\n");
		ominus = 0; //guarantees that it will pass here just once
	}
	/**/
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
