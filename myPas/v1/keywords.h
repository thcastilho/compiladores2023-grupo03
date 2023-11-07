/**@<keywords.h>**/

enum {
    BEGIN = 1024, 
    IN, 
    AND, 
    OR, 
    NOT, 
    MOD, 
    DIV, 
    INTEGER, 
    REAL, 
    DOUBLE,
    BOOLEAN, 
    STRING, 
    PROGRAM, 
    PROCEDURE, 
    FUNCTION, 
    VAR, 
    IF, 
    THEN, 
    ELSE, 
    REPEAT, 
    UNTIL, 
    WHILE, 
    DO, 
    END
};

extern char *keywords[];

int iskeyword(char *identifier);