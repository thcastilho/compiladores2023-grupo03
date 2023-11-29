/*@<parser.h>::*/

extern void mypas(void);
extern void idlist(void);
extern void block(void);
extern int type(void);
extern void header(void);
extern void vardecl(void);
extern void sbpdecl(void);
extern void parmlist(void);
extern void beginstmt(void);
extern void stmtlist(void);
extern void stmt(void);
extern void expr(void);
extern void ifstmt(void);
extern void repstmt(void);
extern void whlstmt(void);
extern void smpexpr(void);
extern void R(void);
extern void term(void);
extern void Q(void);
extern void exprlist(void);
extern void factor(void);

extern void match(int);

extern FILE *src;

extern int lookahead;
extern int gettoken(FILE *);

