bsd = -D_BSD_SOURCE
gnu = -D_GNU_SOURCE

%.o: %.c
	gcc -c $<

all: myshell
clobber: clean
	rm -f *~ #*# core
clean:
	rm -f myshell *.o

myshell: myshell.o lexical.o syntatic.o dynarray.o token.o
	gcc -g -o $@ $? 
token.o: token.h
dynarray.o: dynarray.h
lexical.o: dynarray.h token.h 
syntatic.o: dynarray.h token.h lexical.h 
myshell.o: dynarray.h token.h lexical.h syntatic.h