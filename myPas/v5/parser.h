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
extern int expr(int);
extern void ifstmt(void);
extern void repstmt(void);
extern void whlstmt(void);
extern int smpexpr(int);
extern void R(void);
extern int term(int);
extern void Q(void);
extern void exprlist(void);
extern int factor(int);
extern void idfactor(void);

extern void match(int);
extern int iscompat(int, int);

extern FILE *src;

extern int lookahead;
extern int gettoken(FILE *);

