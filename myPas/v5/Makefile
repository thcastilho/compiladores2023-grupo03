# Makefile on 07/11/2023

CFLAGS = -I. -g

#$(CC) -o mypas main.o lexer.o parser.o keywords.o symtab.o

mypas: main.o lexer.o parser.o keywords.o symtab.o
	$(CC) -o $@ $^

clean:
	$(RM) *.o