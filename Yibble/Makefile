
yibble:		main.o errors.o text.o
	gcc -o yibble main.o errors.o text.o -lcurses

main.o:		main.c errors.h text.h
	gcc -c -Wall main.c

errors.o:	errors.c errors.h
	gcc -c -Wall errors.c

text.o:		text.c text.h
	gcc -c -Wall text.c

