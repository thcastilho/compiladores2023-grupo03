/*@<lexer.c>::*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <tokens.h>
#include <lexer.h>
#include <string.h>

int line_counter = 1;

char lexeme[MAXIDLEN+1];
/*
 * ID = [A-Za-z][A-Za-z0-9]*
 */
int isID(FILE *tape)
{
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
	//int i = strlen(lexeme);

	int i = 0; //mudei aqui

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
	if (lexeme[i] == '0') {
		i = 1;
		lexeme[i] = getc(tape);
		if (lexeme[i] > '7' || lexeme[i] < '0') {
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
			i--;
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;

			return 0;
		}
		do {
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
	if (lexeme[i] == '0') {
		i = 1;
		lexeme[i] = getc(tape);
		if (toupper(lexeme[i]) != 'X') {
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
			i--;
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
			
			return 0;
		}
		i = 2;
		lexeme[i] = getc(tape);
		if (!isxdigit(lexeme[i])) {
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
			i--;
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
			i--;
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
			
			return 0;
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
	int sign = 0; //sera uma variavel de controle apenas, booleana
	int i = strlen(lexeme);
	lexeme[i] = getc(tape);
	
	if (toupper(lexeme[i]) == 'E') {
		i++; //mudei aqui
		lexeme[i] = getc(tape);
		
		if (lexeme[i] == '+' || lexeme[i] == '-') {
			sign = lexeme[i];
		} else {
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
			i--; //mudei aqui
		}
		
		i++; //mudei aqui
		lexeme[i] = getc(tape);
		if (isdigit(lexeme[i])) { // se leu o primeiro e eh digito, entao...
			i++; //mudei aqui
			while(isdigit(lexeme[i] = getc(tape))){
				if (i < MAXIDLEN) {
					i++;
				}
			};
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;

			return EE;
		} else { // se o primeiro nao eh digito, devolvo tudo
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
			if (sign) {
				i--;
				ungetc(lexeme[i], tape); //devolve sinal, se houver
				lexeme[i] = 0;
			}
			i--;
			ungetc(lexeme[i], tape); //devolve 'e' ou 'E'
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
	int token; //chamar o isEE
	
	if (lexeme[i] == '.') { //se encontra o .
		i = 1;
		lexeme[i] = getc(tape);
		if (isdigit(lexeme[i])) {
			i = 2;
			while(isdigit(lexeme[i] = getc(tape))) {
				if (i < MAXIDLEN) {
					i++;
				}
			};
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
			token = isEE(tape); //poderia ser if(isEE(tape) == EE)
			return FLT;
		} else {
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
			i--;
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
			return 0;
		}
	} else {
		ungetc(lexeme[i], tape); //devolvo o .
		lexeme[i] = 0;
		if (isDEC(tape)) {
			i = strlen(lexeme); //ja leu todos os caracteres la de isDEC 
			if (isEE(tape)) {
				return FLT;
			} else {
				//i++;
				lexeme[i] = getc(tape);
				if (lexeme[i] == '.') {
					i++; //mudei aqui
					while(isdigit(lexeme[i] = getc(tape))){
						if (i < MAXIDLEN) {
							i++;
						}	
					};
					ungetc(lexeme[i], tape);
					lexeme[i] = 0;
					token = isEE(tape);
					return FLT;
				} else {
					ungetc(lexeme[i], tape);
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
	//if ( (token = isOCT(source)) ) return token;
	//if ( (token = isHEX(source)) ) return token;
	//if ( (token = isFLT(source)) ) return token;
	//if ( (token = isDEC(source)) ) return token;
	return getc(source);
}
