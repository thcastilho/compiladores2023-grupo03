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
/**/int lex_level = 1;/**/
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
	symtab_append(lexeme, 0);

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
	if(symtab_append(lexeme, lex_level)) {
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
		if(symtab_append(lexeme, lex_level)) {
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
		lex_level++;
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

		lex_level--;
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
			idfactor();
			break;

		default: ;		
	}
}

void idfactor(void) {
/**/int obj_pos;/**/
/**/char var_name[MAXIDLEN+1];/**/
/**/int expr_type;/**/
/**/int var_type;/**/

/**/strcpy(var_name, lexeme);/**/
/**/
if((obj_pos = symtab_lookup(var_name)) < 0) {
		fprintf(stderr, "%s undeclared error\n", var_name);
		error_count++;
}
/**/
match(ID);
if(lookahead == '(') {
	//R-Value
	/**/
	if(symtab[obj_pos].objtype == 0 || symtab[obj_pos].objtype == 3) {
		fprintf(stderr, "error in line %d: %s must be a function or procedure error\n", line_counter, var_name);
		error_count++;
	}
	/**/
	match('(');
	exprlist(); 
	match(')');
} else if (lookahead == ASGN) {
	// L-Value
	var_type = symtab[obj_pos].type;
	match(ASGN); /**/expr_type = /**/ expr(0);
	/**/var_type = iscompat(expr_type, var_type); /**/
	if(var_type < expr_type) {
		fprintf(stderr, "error in line %d: incompatible types.\n", line_counter);
		printf("%s\n", symtab[obj_pos].name);
		error_count++;
	}
} else {
	if(symtab[obj_pos].objtype == 0 || symtab[obj_pos].objtype == 3) {
		fprintf(stderr, "error in line %d: %s is not a callable object.\n", line_counter, var_name);
	}
}
}

int expr(int expr_type) {
	int type1 = smpexpr(0);
	expr_type = iscompat(expr_type, type1);
	switch(lookahead) {
		case '=':
		case '<':
		case '>':
		case LEQ:
		case GEQ:
		case NEQ:
			match(lookahead);
			/**/int type2 = /**/ smpexpr(0);
			expr_type = iscompat(type1, type2);
			if(expr_type != -6) expr_type = 4;
		default: ;
	}

	return expr_type;
}

/* ifstmt -> IF expr THEN stmt [ ELSE stmt ]  */
void ifstmt(void) {
	/**/int expr_type;/**/ // a expressão deve ser booleana
	match(IF);
	/**/ expr_type = /**/expr(0);
	/**/
	if(expr_type != 4) {
		fprintf(stderr, "error: if-statement must be a logic expression.\n");
		error_count++;
	}
	/**/

	match(THEN);
	stmt();
	if(lookahead == ELSE) {
		match(ELSE);
		stmt();
	}
}

/* repstmt -> REPEAT stmtlist UNTIL expr */
void repstmt(void) {
	/**/int expr_type;/**/ // a expressão deve ser booleana

	match(REPEAT);
	stmtlist();
	match(UNTIL);
	/**/ expr_type = /**/ expr(0);
	
	/**/
	if(expr_type != 4) {
		fprintf(stderr, "error: repeat-statement must be a logic expression.\n");
		error_count++;
	}
	/**/
}

/* whlstmt -> WHILE expr DO stmt */
void whlstmt(void) {
	/**/int expr_type;/**/ // a expressão deve ser booleana

	match(WHILE);
	/**/ expr_type = /**/ expr(0);
	
	/**/
	if(expr_type != 4) {
		fprintf(stderr, "error: while-statement must be a logic expression.\n");
		error_count++;
	}
	/**/

	match(DO);
	stmt();
}

int smpexpr(int smpexpr_type)
{
	/**/ int isominus = 0, term_type; /**/
    if (lookahead == '+' || lookahead == '-' || lookahead == NOT) {
        /**/isominus = lookahead;/**/
		match(lookahead);
    }
_term:
    /**/ term_type = /**/ term(smpexpr_type); 
	/**/
	if(isominus) {
		smpexpr_type = iscompat(smpexpr_type, isominus);
		isominus = 0;
	}
	/**/
    if (lookahead == '+' || lookahead == '-' || lookahead == OR) {
		/**/smpexpr_type = iscompat(smpexpr_type, lookahead);/**/
		match(lookahead); goto _term;
    }

	return iscompat(smpexpr_type, term_type);
}
		
int term(int term_type)
{
	/**/int factor_type;/**/
_factor:
    /**/factor_type = factor(term_type);/**/
    if (lookahead == '*' || lookahead == '/' || lookahead == DIV || lookahead == MOD || lookahead == AND) {
        /**/factor_type = iscompat(factor_type, lookahead);/**/
		match(lookahead); goto _factor;
    }

	return iscompat(factor_type, term_type);
}

void exprlist(void) {
/**/int expr_type;/**/
_expr:
	/**/expr_type = expr(0);/**/

	if(lookahead == ',') {
		match(',');
		goto _expr;
	}
}

int factor(int fact_type)
{
	/**/int obj_pos;/**/
	/**/char var_name[MAXIDLEN+1];/**/
	/**/int expr_type;/**/

	switch(lookahead) {
		case ID:
			/**/strcpy(var_name, lexeme);/**/
			/**/
			if((obj_pos = symtab_lookup(var_name)) < 0) {
					fprintf(stderr, "%s undeclared error\n", var_name);
					error_count++;
			}
			/**/
			match(ID);
			if(lookahead == '(') {
				//R-Value
				/**/
				if(symtab[obj_pos].objtype == 0 || symtab[obj_pos].objtype == 3) {
					fprintf(stderr, "%s must be a function or procedure error\n", var_name);
					error_count++;
				}
				/**/
				match('(');
				exprlist();
				match(')');
			} else if (lookahead == ASGN) {
				// L-Value
				match(ASGN); /**/expr_type = /**/ expr(fact_type);
				/**/fact_type = iscompat(expr_type, fact_type); /**/
				if(expr_type > fact_type) {
					fprintf(stderr, "error in line %d: incompatible types.\n", line_counter);
					error_count++;
				}
			} else {
				/**/fact_type = iscompat(symtab[obj_pos].type, fact_type);/**/
				; // R-Value
			}
			break;
		case DEC:
			fact_type = 1;
			match(DEC);
			break;
		case OCT:
			match(OCT);
			break;
		case HEX:
			match(HEX);
			break;
		case FLT:
			fact_type = 2;
			match(FLT);
			break;
		default:
			match('('); /**/expr_type = /**/ expr(fact_type); match(')');
			/**/fact_type = iscompat(expr_type, fact_type); /**/		
	}
	/**/return fact_type;/**/
}

void match(int expected)
{
	if(lookahead != expected) {
		if(expected >= 33 && expected <= 126) { //the range is for special and normal characters
			fprintf(stderr, "token mismatch at line %d: expected %c\n", line_counter, expected);
		} else if(expected >= 10) { //line feed
			fprintf(stderr, "token mismatch at line %d: expected %d\n", line_counter, expected);
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

#define max(x, y) x>y?x:y

int iscompat(int type1, int type2) {
	if(type2 == 0) {
		return type1;
	} else if(type1 == 0) {
		return type2;
	}
	switch (type1) {
		case 1: //integer
			switch(type2) {
				case MOD:
				case DIV:
					return 1;
			}
		case 2: //float4
		case 3: //float8
			switch(type2) {
				case 1: //integer
				case 2: //float4
				case 3: //float8
					return max(type1, type2);
				case '+':
				case '-':
				case '*':
				case '/':
					return type1;
			}
			break;
		case 4: //boolean
			switch(type2) {
				case 4:
				case OR:
				case AND:
				case NOT:
					return 4;
			}
			break;
		case 5: //char
		case 6: //str
			switch(type2) {
				case 5:
				case 6:
					return max(type1, type2);
				case '+':
					return 6;
			}
			break;
	}
	return -6;
}