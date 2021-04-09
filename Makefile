all: compresse.exe decompresse.exe 

node.o : node.c node.h
	gcc -Wall -g -c node.c

compresse.o : compresse.h compresse.c
	gcc -Wall -g -c compresse.c

compresse.exe : main.c compresse.o node.o
	gcc -Wall -g compresse.o node.o main.c -o compresse.exe

decompresse.exe : decompresse.c decompresse.h compresse.h compresse.c
	gcc -Wall -g compresse.o node.o decompresse.c -o decompresse.exe