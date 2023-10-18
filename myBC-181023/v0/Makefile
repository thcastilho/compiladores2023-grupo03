# Makefile on 09/08/2023

CFLAGS = -I. -g

#$(CC) -o mybc main.o lexer.o parser.o

mybc: main.o lexer.o parser.o
	$(CC) -o $@ $^

clean:
	$(RM) *.o
