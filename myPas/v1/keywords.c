/**@:<keywords.c>**/

#include <string.h>
#include <keywords.h>

char *keywords[] = {
    "begin", 
    "in", 
    "and", 
    "or", 
    "not", 
    "mod", 
    "div", 
    "integer", 
    "real", 
    "double", 
    "boolean", 
    "character",
    "string", 
    "program", 
    "procedure", 
    "function", 
    "var", 
    "if", 
    "then", 
    "else", 
    "repeat", 
    "until", 
    "while", 
    "do", 
    "end"
};

int iskeyword(char *identifier) {
    for (int i = BEGIN; i <= END; i++)
        if (strcmp(keywords[i - BEGIN], identifier) == 0)
            return i;

    return 0;
}