#include <string.h>
#include <symtab.h>

#define MAXSYMTABSIZE 16384

struct _symtab_ symtab[MAXSYMTABSIZE];

int symtab_next_entry;

int symtab_lookup(char *query) {
    int i;
    for (i = symtab_next_entry - 1; i > -1; i--) {
        if(strcmp(symtab[i].name, query) == 0) {
            return i;
        }
    }
    return i;
}   

int symtab_append(char *name) {
    if (symtab_lookup(name) < 0) { //the symbol is not in the table
        strcpy([symtab_next_entry].name, name);
        symtab_next_entry++;
        return 0; //success
    }
    return -2; //error, duplicated entry
}
