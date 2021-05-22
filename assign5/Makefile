bsd = -D_BSD_SOURCE
gnu = -D_GNU_SOURCE

%.o: %.c
	gcc -c $<

all: myshell
clobber: clean
	rm -f *~ #*# core
clean:
	rm -f myshell *.o

myshell: myshell.o builtin.o syntatic.o lexical.o dynarray.o token.o  
	gcc -g -o $@ $? 
token.o: token.h
dynarray.o: dynarray.h
lexical.o: lexical.h 
syntatic.o: syntatic.h 
builtin.o: builtin.h lexical.c syntatic.c 
myshell.o: lexical.h syntatic.h builtin.h lexical.c syntatic.c builtin.c