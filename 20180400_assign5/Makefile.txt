all: ish
clobber: clean
	rm -f *~ \#*\# core
clean:
	rm -f ish *.o
ish: dynarray.o lex.o syn.o myish.o
	gcc209 dynarray.o lex.o syn.o myish.o -o ish
dynarray.o: dynarray.c dynarray.h
	gcc209 -c dynarray.c
lex.o: dynarray.h lex.c lex.h
	gcc209 -c lex.c
syn.o: dynarray.h lex.h syn.c syn.h
	gcc209 -c syn.c
myish.o: dynarray.h lex.h syn.h myish.c
	gcc209 -c myish.c
