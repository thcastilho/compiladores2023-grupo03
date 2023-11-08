/*@<parser.c>::*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tokens.h>
#include <lexer.h>
#include <parser.h>
#include <keywords.h>

int lookahead;

/*****************************************************************************************************
 * non-terminals: {mypas, idlist, block, ...}
 * initial symbol: mypas
 *
 * mypas -> PROGRAM ID '(' idlist ')' ';' block '.'
 *
 * void mypas(void)
 * {
 * 	match(PROGRAM); match(ID); match('('); idlist(); match(')'); match(';'); block(); match('.');
 * }
 *
 * idlist -> ID { ',' ID }
 *
 * block -> header beginstmt
 *
 * header -> vardecl sbpdecl | <empty>
 *
 * vardecl -> VAR idlist ':' type ';' { idlist ':' type ';' } | <empty>
 *
 * sbpdecl -> PROCEDURE ID parmlist ';' block ';'
 *          | FUNCTION  ID parmlist ':' type ';' block ';'
 *          | <empty>
 *
 * parmlist -> '(' [ VAR ] idlist { ';' [ VAR ] idlist } ')' | <empty>
 *
 * beginstmt -> BEGIN stmtlist END
 *
 * stmtlist -> stmt { ';' stmt } 
 *
 * stmt -> beginstmt | ifstmt | repstmt | whlstmt | factor | <empty>
 *
 * ifstmt -> IF expr THEN stmt [ ELSE stmt ] 
 *
 * repstmt -> REPEAT stmtlist UNTIL expr
 *
 * whlstmt -> WHILE expr DO stmt
 *
 ****************************************************************************************************/

/* mypas -> PROGRAM ID '(' idlist ')' ';' block '.' */
void mypas(void) {
	match(PROGRAM); 
	match(ID); 
	match('('); 
	idlist(); 
	match(')'); 
	match(';'); 
	block(); 
	match('.');
}

/* idlist -> ID { ',' ID } */
void idlist(void) {
	/*se symtab_append der -2, simbolo duplicado*/
	//trocar pra goto. usar aqui, no vardecl, no sbpdecl, no parmlist
	match(ID);
	while(lookahead == ',') {
		match(','); 
		match(ID);
	}
}

/* block -> header beginstmt */
void block(void){
	header();
	beginstmt();
}

/* header -> vardecl sbpdecl | <empty> */
void header(void) {
	vardecl(); 
	sbpdecl();
}

/* type -> INTEGER | REAL | DOUBLE | BOOLEAN | CHARACTER | STRING*/
void type(void) {
	switch(lookahead) {
		case INTEGER:
			match(INTEGER);
			break;

		case REAL:
			match(REAL);
			break;

		case DOUBLE:
			match(DOUBLE);
			break;

		case BOOLEAN:
			match(BOOLEAN);
			break;

		case CHARACTER:
			match(CHARACTER);
			break;

		case STRING:
			match(STRING);
			break;

		default: ;
	}
}

/* vardecl -> VAR idlist ':' type ';' { idlist ':' type ';' } | <empty> */
void vardecl(void) {
	if(lookahead == VAR) {
		match(VAR);

_vardeclarations:
		idlist();
		match(':');
		type();
		match(';');
		if(lookahead == ID) {
			goto _vardeclarations;
		}
	} else {
		;
	}
	
	
}

/*
 * sbpdecl -> { PROCEDURE ID parmlist ';' block ';'
 *          | FUNCTION  ID parmlist ':' type ';' block ';'
 *          }
 */
void sbpdecl(void) {

_sbpdecl:
	switch(lookahead) {
		case PROCEDURE:
			match(PROCEDURE);
			match(ID);
			parmlist();
			match(';');
			block();
			match(';');
			break;

		case FUNCTION:
			match(FUNCTION);
			match(ID);
			parmlist();
			match(':');
			type();
			match(';');
			block();
			match(';');
			break;

		default: ;
	}

	if(lookahead == PROCEDURE || lookahead == FUNCTION) {
		goto _sbpdecl;
	}

}

/* parmlist -> '(' [ VAR ] idlist : type { ';' [ VAR ] idlist : type} ')' | <empty> */
void parmlist(void) {
	if(lookahead == '(') {
		match('(');
		
	_parameterlist:
		if(lookahead == VAR) match(VAR);
		
		idlist();
		match(':');
		type();
		
		if(lookahead == ';') {
			match(';');
			goto _parameterlist;
		}

		match(')');
	} else {
		;
	}

}

/* beginstmt -> BEGIN stmtlist END */
void beginstmt(void) {
	match(BEGIN);
	stmtlist();
	match(END);
}

/* stmtlist -> stmt { ';' stmt } */
void stmtlist(void) {
	stmt();
	while(lookahead == ';') {
		match(';'); 
		stmt();
	}
}

/* stmt -> beginstmt | ifstmt | repstmt | whlstmt | factor | <empty> */
void stmt(void) {
	switch(lookahead) {
		case BEGIN:
			beginstmt();
			break;
		
		case IF:
			ifstmt();
			break;
		
		case REPEAT:
			repstmt();
			break;
		
		case WHILE:
			whlstmt();
			break;

		case ID:
			factor();
			break;

		default: ;		
	}
}

void expr(void){
	smpexpr();
	switch(lookahead) {
		case '=':
		case '<':
		case '>':
		case LEQ:
		case GEQ:
		case NEQ:
			match(lookahead);
			smpexpr();
		default: ;
	}
}

/* ifstmt -> IF expr THEN stmt [ ELSE stmt ]  */
void ifstmt(void) {
	match(IF);
	expr();
	match(THEN);
	stmt();
	if(lookahead == ELSE) {
		match(ELSE);
		stmt();
	}
}

/* repstmt -> REPEAT stmtlist UNTIL expr */
void repstmt(void) {
	match(REPEAT);
	stmtlist();
	match(UNTIL);
	expr();
}

/* whlstmt -> WHILE expr DO stmt */
void whlstmt(void) {
	match(WHILE);
	expr();
	match(DO);
	stmt();
}

void smpexpr(void)
{
	if (lookahead == '+' || lookahead == '-') {
		match(lookahead);
	}
    
	term(); R();
}

void R(void)
{
	if (lookahead == '+' || lookahead == '-') {
		match(lookahead); term(); R();
	} else {
		;
	}
}
		
void term(void)
{
	factor(); Q();
}

void Q(void)
{
	if (lookahead == '*' || lookahead == '/') {
		match(lookahead); factor(); Q();
	} else {
		;
	}
}

void exprlist(void) {

_expr:
	expr();

	if(lookahead == ',') {
		match(',');
		goto _expr;
	}
}

void factor(void)
{
	switch(lookahead) {
		case ID:
			match(ID);
			if(lookahead == '(') {
				match('(');
				exprlist();
				match(')');
			}
			if (lookahead == ASGN) {
				// L-Value
				match(ASGN); smpexpr();
			} else {
				; // R-Value
			}
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
			match('('); smpexpr(); match(')');		
	}
}

void match(int expected)
{
        if (lookahead == expected) {
                lookahead = gettoken(src);
        } else {
                if (expected >= 33 && expected <= 126)
                {
                        fprintf(stderr, "token mismatch...expected: %c ", expected);
                }
                else if (expected >= 10)
                {
                        fprintf(stderr, "token mismatch...expected: %d ", expected);
                }

                if (lookahead >= 33 && lookahead <= 126)
                {
                        fprintf(stderr, "in has: %c ", lookahead);
                }
                else if (lookahead >= 10)
                {

                        fprintf(stderr, "in has: %d ", lookahead);
                        
                }
                fprintf(stderr,"in line: %d\n",line_counter);
                exit(-2);
        }
}
