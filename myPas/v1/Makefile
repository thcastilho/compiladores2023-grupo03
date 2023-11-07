# Makefile on 07/11/2023

CFLAGS = -I. -g

#$(CC) -o mybc main.o lexer.o parser.o

mypas: main.o lexer.o parser.o keywords.o
	$(CC) -o $@ $^

clean:
	$(RM) *.o