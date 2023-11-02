/*@<parser.c>::*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

/**/
#define MAXSTACKSIZE 1024
double Acc, stack[MAXSTACKSIZE];
int sp = -1;
void Push(double x)
{
	sp++;
	stack[sp] = x;
}
double Pop(void)
{
	double y = stack[sp];
	sp--;
	return y;
}
#define MAXSYMTABSIZE 16384
char symtab[MAXSYMTABSIZE][MAXIDLEN+1];
int symtab_next;
double vm[MAXSYMTABSIZE];
double Recall(char *varname)
{
	int i;
	for (i = 0; i < symtab_next; i++) {
		if (strcmp(symtab[i], varname) == 0) {
			return vm[i];
		}
	}
	strcpy(symtab[i], varname);
	symtab_next++;
	return 0.0;
}
void Store(char *varname, double x)
{
	int i;
	for (i = 0; i < symtab_next; i++) {
		if (strcmp(symtab[i], varname) == 0) break;
	}
	if (i == symtab_next) {
		strcpy(symtab[i], varname);
		symtab_next++;
	}
	vm[i] = x;
}
/**/

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

/* 
 * mybc -> cmd{cmdsep cmd} EOF
 * cmdsep = ';' | '\n'
 */
void mybc(void) {
	cmd();
	while(lookahead == ';' || lookahead == '\n') {
		match(lookahead);
		cmd();
	}
}

/* cmd -> E | exit | quit | <epsilon> */
void cmd(void) {
	switch(lookahead) {
		case ';':
		case '\n':
		case EOF:
			; // do nothing
			break;
		case QUIT:
		case EXIT:
			exit(0);
		default:
			E();
			printf("%lg\n", Acc); //prints what is in the accumulator
	}
}

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
				stack[sp] = stack[sp] + Acc;
				Acc = Pop();
				break;
			case '-':
				stack[sp] = stack[sp] - Acc;
				Acc = Pop();
				break;
			default:
				;
		}
		/**/
	}

	if(isOPlus(lookahead)) //check the oplus optional condition (note that is a Kleene closure)
	{
		sign = lookahead;
		Push(Acc);
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
				stack[sp] = stack[sp] * Acc;
				Acc = Pop();
				break;
			case '/':
				stack[sp] = stack[sp] / Acc;
				Acc = Pop();
				break;
			default:
				;
		}
		/**/
	}

	if(isOTimes(lookahead)) //check the otimes optional condition (note that is a Kleene closure)
	{
		sign = lookahead;
		Push(Acc);
		match(lookahead);
		goto _F; //return to the beginning of the label
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
				Store(varname, Acc);
			} else {
				// R-Value
				Acc = Recall(varname);
		}
			break;
		case DEC:
			Acc = atof(lexeme);
			match(DEC);
			break;
		case OCT:
			Acc = strtol(lexeme, NULL, 8); //oct to long int
			match(OCT);
			break;
		case HEX:
			Acc = strtol(lexeme, NULL, 16); //hex to long int
			match(HEX);
			break;
		case FLT:
			Acc = atof(lexeme);
			match(FLT);
			break;
		default:
			match('('); E(); match(')');
	}

	if(minus) { //if it is signed
		Acc = -Acc;
		minus = 0; //guarantees that it will pass here just once
	}
}

void match(int expected)
{
	if(lookahead != expected) {
		if(expected >= 33 && expected <= 126) { //the range is for special and normal characters
			fprintf(stderr, "token mismatch, expected %c\n", expected);
		} else if(expected >= 10) { //line feed
			fprintf(stderr, "token mismatch, expected %d\n", expected);
		}

		if(lookahead >= 33 && lookahead <= 126) { //the range is for special and normal characters
			fprintf(stderr, "got %c, which is what will be used\n", lookahead);
		} else if(lookahead >= 10) { //line feed
			fprintf(stderr, "got %d, which is what will be used\n", lookahead);
		}

		if(lookahead == '\n') return; //this allow us to continue the parsing; if we remove it, then the program will wait for a new input to show the result
	}

	lookahead = gettoken(src);
}