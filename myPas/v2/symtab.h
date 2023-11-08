#pragma once

struct _symtab_ {
    char name[MAXIDLEN+1];
    int objtype; // = 0, if var; = 1, if function; = 2, if proc
    int type; // = 0, if proc; = 1, if INTEGER ...
    char offset[MAXIDLEN+1]; // name in Assembly
}