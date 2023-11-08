/**@<keywords.h>**/

enum {
    BEGIN = 2048, 
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
    CHARACTER, 
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