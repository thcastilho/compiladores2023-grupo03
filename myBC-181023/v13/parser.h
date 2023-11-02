/*@<parser.h>::*/

extern void mybc(void);
extern void cmd(void);
extern void E(void);
extern void T(void);
extern void R(void);
extern void F(void);
extern void Q(void);

extern void match(int);

extern FILE *src;

extern int lookahead;
extern int gettoken(FILE *);
