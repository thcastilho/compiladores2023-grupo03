/*@<lexer.c>::*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <tokens.h>
#include <lexer.h>
#include <string.h>

int line_counter = 1;

char lexeme[MAXIDLEN+1];

int isASGN(FILE *tape)
{
	if ( (lexeme[0] = getc(tape)) == ':' ) { //if starts with ':'
		if ( (lexeme[1] = getc(tape) ) == '=' ) { //and follows a '='
			lexeme[2] = 0;
			return ASGN; //it is an assignment symbol
		}
		ungetc(lexeme[1], tape);
	}
	ungetc(lexeme[0], tape);
	return 0;
}

/*
 * ID = [A-Za-z][A-Za-z0-9]*
 */
int isID(FILE *tape)
{
	int kword;
	int i = 0;

	lexeme[i] = getc(tape);

	if (isalpha(lexeme[i])) {
		i = 1;
		while(isalnum(lexeme[i] = getc(tape))){
			if (i < MAXIDLEN) {
				i++;
			}
		};
		ungetc(lexeme[i], tape);
		lexeme[i] = 0;
		//checking if it is not a keyword
		if ( (kword = iskeyword(lexeme)) ) return kword; 
		//if it is not, it is an identifier
		return ID;
	}
	
	ungetc(lexeme[i], tape);
	lexeme[i] = 0;
	
	return 0;
}

/*
 * DEC = [1-9][0-9]* | 0
 */
int isDEC(FILE *tape)
{
	int i = 0; 

	lexeme[i] = getc(tape);

	if (isdigit(lexeme[i])) {
		if (lexeme[i] == '0') {
			return DEC;
		}
		i = 1;
		while(isdigit(lexeme[i] = getc(tape))){
			if(i < MAXIDLEN) {
				i++;
			}
		};
		ungetc(lexeme[i], tape);
		lexeme[i] = 0;

		return DEC;
	}
	ungetc(lexeme[i], tape);
	lexeme[i] = 0;
	
	return 0;
}

/*
 * OCT = 0[0-7]⁺
 */
int isOCT(FILE *tape)
{
	int i = 0;
	lexeme[i] = getc(tape);
	if (lexeme[i] == '0') { //if starts with 0,
		i = 1;
		lexeme[i] = getc(tape);
		if (lexeme[i] > '7' || lexeme[i] < '0') { //check if the next is not between 0 and 7
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
			i--;
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;

			return 0; //if is not, give back everything to the tape
		}
		do { //if it is, scan until it is not between 0 and 7
			if (i < MAXIDLEN) {
				i++;
			}
			lexeme[i] = getc(tape);
		} while (lexeme[i] <= '7' && lexeme[i] >= '0');
		
		ungetc(lexeme[i], tape);
		lexeme[i]= 0;
		
		return OCT;
	}
	ungetc(lexeme[i], tape);
	lexeme[i]= 0;
	
	return 0;
}

/*
 * HEX = 0[xX][0-9A-Fa-f]⁺  
 */
int isHEX(FILE *tape)
{
	int i = 0;
	lexeme[i] = getc(tape);
	if (lexeme[i] == '0') { //if the head is 0,
		i = 1;
		lexeme[i] = getc(tape);
		if (toupper(lexeme[i]) != 'X') { //the next has to be x or X
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
			i--;
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
			
			return 0; //if it is not, give back everything to the tape
		}
		i = 2;
		lexeme[i] = getc(tape);
		if (!isxdigit(lexeme[i])) { //the next character has to be a hex digit
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
			i--;
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
			i--;
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
			
			return 0; //if it is not, give back everything to the tape
		}
		do {
			if (i < MAXIDLEN) {
				i++;
			}
			lexeme[i] = getc(tape);
		}while(isxdigit(lexeme[i]));
		
		ungetc(lexeme[i], tape);
		lexeme[i]= 0;
		
		return HEX;
	}
	ungetc(lexeme[i], tape);
	lexeme[i]= 0;
	return 0;
}

/*
 * EE = [eE]['+''-']?[0-9]⁺
 */
int isEE(FILE *tape) {
	int sign = 0; //boolean variable to control if there is a signal or not
	int i = strlen(lexeme);
	lexeme[i] = getc(tape);
	
	if (toupper(lexeme[i]) == 'E') { //the head has to start with 'e' or 'E'
		i++; 
		lexeme[i] = getc(tape);
		
		if (lexeme[i] == '+' || lexeme[i] == '-') { //check if a signal come after the 'e' or 'E'
			sign = lexeme[i];
		} else {
			ungetc(lexeme[i], tape); //it is optional, so if it is not a signal, give back the character to the tape
			lexeme[i] = 0;
			i--; 
		}
		
		i++; 
		lexeme[i] = getc(tape);
		if (isdigit(lexeme[i])) { //if the head is a digit, so...
			i++; 
			while(isdigit(lexeme[i] = getc(tape))){
				if (i < MAXIDLEN) {
					i++;
				}
			};
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;

			return EE;
		} else { //if is not, give back everything to the tape
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
			if (sign) {
				i--;
				ungetc(lexeme[i], tape); //give back the signal, if exists
				lexeme[i] = 0;
			}
			i--;
			ungetc(lexeme[i], tape); //give back 'e' or 'E'
			lexeme[i] = 0;
			
			return 0;
		}
	}
	ungetc(lexeme[i], tape);
	lexeme[i] = 0;

	return 0;
}

/*
 * FLT = (DEC'.'[0-9]* | '.'[0-9]⁺)EE? | DEC EE 
 */
int isFLT(FILE *tape) {
	int i = 0;
	lexeme[i] = getc(tape);
	int token; //call isEE
	
	if (lexeme[i] == '.') { //if starts with '.'
		i = 1;
		lexeme[i] = getc(tape);
		if (isdigit(lexeme[i])) { //the next character has to be a digit
			i = 2;
			while(isdigit(lexeme[i] = getc(tape))) { //scan until it is not a digit
				if (i < MAXIDLEN) {
					i++;
				}
			};
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
			token = isEE(tape); //check if it follows an exponential notation (optional)
			return FLT;
		} else {
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
			i--;
			ungetc(lexeme[i], tape); //if the next character is not a digit, give back everything to the tape
			lexeme[i] = 0;
			return 0;
		}
	} else {
		ungetc(lexeme[i], tape);
		lexeme[i] = 0;
		if (isDEC(tape)) { //check if it starts with a dec number
			i = strlen(lexeme);  
			if (isEE(tape)) { //check if it follows an exponential notation
				return FLT; //it is already a float number
			} else {
				lexeme[i] = getc(tape); //if the exponential notation did not came
				if (lexeme[i] == '.') { //check if the next character is '.'
					i++; 
					while(isdigit(lexeme[i] = getc(tape))){ //scan until it is not a digit
						if (i < MAXIDLEN) {
							i++;
						}	
					};
					ungetc(lexeme[i], tape);
					lexeme[i] = 0;
					token = isEE(tape); //check if it follows an exponential notation (optional)
					return FLT;
				} else {
					ungetc(lexeme[i], tape); //if this does not follow either a exponential notation or a '.', it is just a dec number 
					lexeme[i] = 0;
					return DEC;
				}
			}
		} else {
			return 0;
		}
	}
	
}

void skipSpacesAndComments(FILE *tape)
{
	int head;

jumpback:
	while(isspace(head = getc(tape))){
		if (head == '\n') {
			line_counter++;
			break; //necessary to keep the program in a loop
		}
	};
	
	if (head == '{'){
		do {
			head = getc(tape);
			
			if (head == '\n') {
				line_counter++;
			}
			
		} while (head != '}' && head != EOF);
		goto jumpback; //check if after the end of a comment there is new spaces or new comments
	}
	
	ungetc(head, tape);
}

int isNUM (FILE* source) {
	int token;
	if ( (token = isOCT(source)) ) return token;
	if ( (token = isHEX(source)) ) return token;
	if ( (token = isFLT(source)) ) return token;
	return 0;
}

int gettoken(FILE *source)
{
	int token;
	skipSpacesAndComments(source);
	if ( (token = isID(source)) ) return token;
	if ( (token = isNUM(source)) ) return token;
	if ( (token = isASGN(source)) ) return token;
	
	return getc(source);
}
