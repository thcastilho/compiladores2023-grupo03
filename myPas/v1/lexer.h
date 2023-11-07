/*@<lexer.h>::*/
#define MAXIDLEN 30

extern char lexeme[MAXIDLEN+1];
extern int line_counter;

extern int iskeyword(char *identifier);
