/*@<lexer.c>::*/

#include <stdio.h>
#include <stdlib.h>
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
	if (ch == '0') {
		ch = getc(tape);
		if (ch > '7' || ch < '0') {
			ungetc(ch, tape);
			ungetc('0', tape);
			return 0;
		}
		do {
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
	if (ch == '0') {
		ch = getc(tape);
		if (ch == 'x' || ch == 'X') {
			x = ch;
		} else {
			ungetc(ch, tape);
			ungetc('0', tape);
			return 0;
		}
		ch = getc(tape);
		if (!isxdigit(ch)) {
			ungetc(ch, tape);
			ungetc(x, tape);
			ungetc('0', tape);
			return 0;
		}
		do {
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
	if (toupper(ch) == 'E') {
		e = ch;
		ch = getc(tape);
		
		if (ch == '+' || ch == '-') {
			sign = ch;
		} else {
			ungetc(ch, tape);
		}
		
		ch = getc(tape);
		if (isdigit(ch)) { // se leu o primeiro e eh digito, entao...
			while(isdigit(ch = getc(tape)));
			ungetc(ch, tape);
			return EE;
		} else { // se o primeiro nao eh digito, devolvo tudo
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
	
	if (ch == '.') {
		ch = getc(tape);
		if (isdigit(ch)) {
			while(isdigit(ch = getc(tape)));
			ungetc(ch, tape); 
			token = isEE(tape);
			return FLT;
		} else {
			ungetc(ch, tape);
			return 0;
		}
	} else {
		ungetc(ch, tape);
		if (isDEC(tape)) {
			if (isEE(tape)) {
				return FLT;
			} else {
				ch = getc(tape);
				if (ch == '.') {
					while(isdigit(ch = getc(tape)));
					ungetc(ch, tape); 
					token = isEE(tape);
					return FLT;
				} else {
					ungetc(ch, tape);
					return DEC;
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

void skipSpacesAndComments(FILE *tape)
{
	int head;

jumpback:
	while(isspace(head = getc(tape))){
		if (head == '\n') {
			line_counter++;
		}
	};
	
	if (head == '{'){
		do {
			head = getc(tape);
			
			if (head == '\n') {
				line_counter++;
			}
			
		} while (head != '}' && head != EOF);
		goto jumpback;
	}
	
	ungetc(head, tape);
}

int gettoken(FILE *source)
{
	int token;
	skipSpacesAndComments(source);
	if ( (token = isID(source)) ) return token;
	if ( (token = isNUM(source)) ) return token;
	//if ( (token = isOCT(source)) ) return token;
	//if ( (token = isHEX(source)) ) return token;
	//if ( (token = isFLT(source)) ) return token;
	//if ( (token = isDEC(source)) ) return token;
	return getc(source);
}
