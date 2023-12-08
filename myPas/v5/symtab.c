#include <string.h>
#include <symtab.h>

#define MAXSYMTABSIZE 16384

struct _symtab_ symtab[MAXSYMTABSIZE];

int symtab_next_entry = 1;

int symtab_lookup(char *query) {
    int i;
    for (i = symtab_next_entry - 1; i > -1; i--) {
        if(strcmp(symtab[i].name, query) == 0) {
            //the object has the same name and it's on the same lexical level, so it already exists
            return i;
        }
    }
    return i;
}   

int symtab_append(char *name, int lex_level) {
    int position_found;
    if ((position_found = symtab_lookup(name)) < 0 || lex_level > symtab[position_found].lex_level) { //the symbol is not in the table
        strcpy(symtab[symtab_next_entry].name, name);
        symtab[symtab_next_entry].lex_level = lex_level;
        symtab_next_entry++;
        return 0; //success
    }
    return -2; //error, duplicated entry
}
