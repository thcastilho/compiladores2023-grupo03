/*@<parser.c>::*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tokens.h>
#include <lexer.h>
#include <symtab.h>
#include <parser.h>
#include <keywords.h>

int lookahead;
int error_count = 0;
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
	symtab_append(lexeme);

	//register the program in the symbol table
	symtab[1].objtype = 3;
	symtab[1].type = 0; 

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
	/**/int obj_pos = symtab_next_entry;/**/

	_idlist:
	
	//if it's true, the symbol is already on the table
	if(symtab_append(lexeme)) {
		fprintf(stderr, "%s already defined error\n", lexeme);
		obj_pos = -1;
		error_count++;
	}

	//if it's not on the table, register the type
	if(obj_pos > -1) {
		symtab[obj_pos].objtype = 0;
	}

	match(ID);
	if(lookahead == ',') {
		match(',');
		/**/obj_pos = symtab_next_entry/**/;
		goto _idlist;
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
int type(void) {
	/**/int obj_type = -1;/**/

	switch(lookahead) {
		case INTEGER:
			match(INTEGER);
			obj_type = 1;
			break;

		case REAL:
			match(REAL);
			obj_type = 2;
			break;

		case DOUBLE:
			match(DOUBLE);
			obj_type = 3;
			break;

		case BOOLEAN:
			match(BOOLEAN);
			obj_type = 4;
			break;

		case CHARACTER:
			match(CHARACTER);
			obj_type = 5;
			break;

		case STRING:
			match(STRING);
			obj_type = 6;
			break;

		default: ;
	}
	return obj_type;
}

/* vardecl -> VAR idlist ':' type ';' { idlist ':' type ';' } | <empty> */
void vardecl(void) {
	if(lookahead == VAR) {
		match(VAR);

/**/int init_pos = symtab_next_entry;/**/
/**/int symtype;/**/

_vardeclarations:
		idlist();
		match(':');
		/**/symtype = /**/ type();
		/**/
		for(int i = init_pos; i < symtab_next_entry; i++) {
			//register the type for all the parsed symbols 
			symtab[i].type = symtype;
		}
		/**/
		match(';');
		if(lookahead == ID) {
			/**/init_pos = symtab_next_entry;/**/
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
	int isFunc = (lookahead == FUNCTION); 
	/**/int obj_pos = symtab_next_entry;/**/
	/**/int ret_type;/**/
_sbpdecl:
	if(isFunc || lookahead == PROCEDURE) {
		match(lookahead);
		/**/
		//if it's true, the symbol is already on the table
		if(symtab_append(lexeme)) {
			fprintf(stderr, "%s already defined error\n", lexeme);
			obj_pos = -1;
			error_count++;
		}
		//if it's not on the table, register the type (function or procedure)
		if(obj_pos > -1 && isFunc) {
			symtab[obj_pos].objtype = 1;
		} else if(obj_pos > -1) {
			symtab[obj_pos].objtype = 2;
		}
		/**/
		match(ID);
		parmlist();

		if(isFunc) {
				match(':');
				/**/ret_type = /**/ type(); //takes the function's return type
				symtab[obj_pos].type = ret_type;
		}

		match(';');
		block();
		match(';');
	}
	if(lookahead == PROCEDURE || lookahead == FUNCTION) {
		/**/obj_pos = symtab_next_entry;/**/
		isFunc = (lookahead == FUNCTION);
		goto _sbpdecl;
	}
}

/* parmlist -> '(' [ VAR ] idlist : type { ';' [ VAR ] idlist : type} ')' | <empty> */
void parmlist(void) {
	if(lookahead == '(') {
		match('(');
		
	/**/int obj_type;/**/
	/**/int init_pos = symtab_next_entry;/**/
	_parameterlist:
		if(lookahead == VAR) match(VAR);
		

		idlist();
		match(':');

		/**/obj_type/**/ = type();
		
		/**/
		for(int i = init_pos; i < symtab_next_entry; i++) {
			//register the type for all the parsed symbols 
			symtab[i].type = obj_type;
		}
		/**/

		if(lookahead == ';') {
			match(';');
		/**/init_pos = symtab_next_entry;/**/
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
	/**/int var_pos;/**/
	/**/char var_name[MAXIDLEN+1];/**/
	
	switch(lookahead) {
		case ID:
			/**/strcpy(var_name, lexeme);/**/
			/**/
			if((var_pos = symtab_lookup(var_name)) < 0) {
					fprintf(stderr, "%s undeclared error\n", var_name);
					error_count++;
			}
			/**/
			match(ID);
			if(lookahead == '(') {
				//R-Value
				/**/
				if(symtab[var_pos].objtype == 0 || symtab[var_pos].objtype == 3) {
					fprintf(stderr, "%s must be a function or procedure error\n", var_name);
					error_count++;
				}
				/**/
				match('(');
				exprlist();
				match(')');
			} else if (lookahead == ASGN) {
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
			match('('); expr(); match(')');		
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