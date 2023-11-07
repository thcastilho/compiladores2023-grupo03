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
	if(lookahead == ID) {
		match(ID);

		while(lookahead == ID || lookahead == ',') {
			match(',');
			match(ID);
		}
	}
}

/* block -> header beginstmt */
void block(void){
	header();
	beginstmt();
}

/* header -> vardecl sbpdecl | <empty> */
void header(void) {
	vardec1(); 
	sbpdec1();
}

/* vardecl -> VAR idlist ':' type ';' { idlist ':' type ';' } | <empty> */
void vardec1(void) {
	match(VAR);
	
	_vardeclarations:
	idlist();
	match(':');
	type();
	match(';');
	if(lookahead == ID) goto _vardeclarations;
}

/*
 * sbpdecl -> PROCEDURE ID parmlist ';' block ';'
 *          | FUNCTION  ID parmlist ':' type ';' block ';'
 *          | <empty>
 */
void sbpdec1(void) {
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
	}
}

/* parmlist -> '(' [ VAR ] idlist { ';' [ VAR ] idlist } ')' | <empty> */
void parmlist(void) {
	match('(');
	
	_parameterlist:
	if(lookahead == VAR) match(VAR);
	
	idlist();
	
	if(lookahead == ';') {
		match(';');
		goto _parameterlist;
	}

	match(')');
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

void factor(void)
{
	if (lookahead == ID) {
		match(ID);
		if (lookahead == ASGN) {
			// L-Value
			match(ASGN); smpexpr();
		} else {
			// R-Value
		}
	} else if (lookahead == DEC) {
		match(DEC);
	} else {
		match('('); smpexpr(); match(')');
	}
}

void match(int expected)
{
	if (lookahead == expected) {
		lookahead = gettoken(src);
	} else {
		fprintf(stderr, "token mismatch... exiting with error\n");
		exit(-2);
	}
}
