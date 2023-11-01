/*@<lexer.c>::*/

#include <stdio.h>
#include <ctype.h>
#include <tokens.h>
#include <lexer.h>

/*
 * ID = [A-Za-z][A-Za-z0-9]*
 */
int isID(FILE *tape)
{
	int head;

	head = getc(tape);

	if (isalpha(head)) {
		while(isalnum(head = getc(tape)));
		ungetc(head, tape);
		return ID;
	}
	
	ungetc(head, tape);
	return 0;
}

/*
 * DEC = [1-9][0-9]* | 0
 */
int isDEC(FILE *tape)
{
	int head;

	head = getc(tape);

	if (isdigit(head)) {
		if (head == '0') {
			return DEC;
		}
		while(isdigit(head = getc(tape)));
		ungetc(head, tape);
		return DEC;
	}
	ungetc(head, tape);
	return 0;
}

/*
 * OCT = 0[0-7]⁺
 */
int isOCT(FILE *tape)
{
	int ch = getc(tape);
	if (ch == '0') { //if starts with 0,
		ch = getc(tape);
		if (ch > '7' || ch < '0') { //check if the next is not between 0 and 7
			ungetc(ch, tape); 
			ungetc('0', tape);
			return 0; //if is not, give back everything to the tape
		}
		do { //if it is, scan until it is not between 0 and 7
			ch = getc(tape);
		} while (ch <= '7' && ch >= '0');
		ungetc(ch, tape);
		return OCT;
	}
	ungetc(ch, tape);
	return 0;
}

/*
 * HEX = 0[xX][0-9A-Fa-f]⁺  
 */
int isHEX(FILE *tape)
{
	int x;
	int ch = getc(tape);
	if (ch == '0') { //if the head is 0,
		ch = getc(tape);
		if (ch == 'x' || ch == 'X') { //the next has to be x or X
			x = ch;
		} else { //if it is not, give back everything to the tape
			ungetc(ch, tape);
			ungetc('0', tape);
			return 0;
		}
		ch = getc(tape);
		if (!isxdigit(ch)) { //the next character has to be a hex digit
			ungetc(ch, tape);
			ungetc(x, tape);
			ungetc('0', tape);
			return 0; //if it is not, give back everything to the tape
		}
		do { //if it is scan until it is not a hex digit
			ch = getc(tape);
		}while(isdigit(ch));
		ungetc(ch, tape);
		return HEX;
	}
	ungetc(ch, tape);
	return 0;
}

/*
 * EE = [eE]['+''-']?[0-9]⁺
 */
int isEE(FILE *tape) {
	int sign = 0;
	int e;
	int ch = getc(tape);
	if (toupper(ch) == 'E') { //the head has to start with 'e' or 'E'
		e = ch; //save the character
		ch = getc(tape);
		
		if (ch == '+' || ch == '-') { //check if a signal come after the 'e' or 'E'
			sign = ch;
		} else {
			ungetc(ch, tape); //it is optional, so if it is not a signal, give back the character to the tape
		}
		
		ch = getc(tape);
		if (isdigit(ch)) { //if the head is a digit, so...
			while(isdigit(ch = getc(tape)));
			ungetc(ch, tape);
			return EE;
		} else { //if is not, give back everything to the tape
			ungetc(ch, tape);
			if (sign) {
				ungetc(sign, tape);
			}
			ungetc(e, tape);
			return 0;
		}
	}
	ungetc(ch, tape);
	return 0;
}

/*
 * FLT = (DEC'.'[0-9]* | '.'[0-9]⁺)EE? | DEC EE 
 */
int isFLT(FILE *tape) {
	int ch = getc(tape);
	int token;
	
	if (ch == '.') { //if starts with '.'
		ch = getc(tape);
		if (isdigit(ch)) { //the next character has to be a digit
			while(isdigit(ch = getc(tape))); //scan until it is not a digit
			ungetc(ch, tape); 
			token = isEE(tape); //check if it follows an exponential notation (optional)
			return FLT;
		} else {
			ungetc(ch, tape); //if the next character is not a digit, give back everything to the tape
			return 0;
		}
	} else {
		ungetc(ch, tape);
		if (isDEC(tape)) { //check if it starts with a dec number
			if (isEE(tape)) { //check if it follows an exponential notation
				return FLT; //it is already a float number
			} else {
				ch = getc(tape); //if the exponential notation did not came
				if (ch == '.') { //check if the next character is '.'
					while(isdigit(ch = getc(tape))); //scan until it is not a digit
					ungetc(ch, tape); 
					token = isEE(tape); //check if it follows an exponential notation (optional)
					return FLT;
				} else {
					ungetc(ch, tape); 
					return DEC; //if this does not follow either a exponential notation or a '.', it is just a dec number 
				}
			}
		} else {
			return 0;
		}
	}
	
}

int isNUM (FILE* source) {
	int token;
	if ( (token = isOCT(source)) ) return token;
	if ( (token = isHEX(source)) ) return token;
	if ( (token = isFLT(source)) ) return token;
	return 0;
}

void skipspaces(FILE *tape)
{
	int head;

	while(isspace(head = getc(tape)));

	ungetc(head, tape);
}

int gettoken(FILE *source)
{
	int token;
	skipspaces(source);
	if ( (token = isID(source)) ) return token;
	if ( (token = isNUM(source)) ) return token; //check if it is a number (DEC, OCT, HEX or FLT)
	return getc(source);
}
