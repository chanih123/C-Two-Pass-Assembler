assembler: assembler.o utils.o
    gcc -ansi -Wall -pedantic assembler.o utils.o -o assembler

assembler.o: assembler.c
    gcc -c -ansi -pedantic -Wall assembler.c -o assembler.o

utils.o: utils.c utils.h
    gcc -c -ansi -pedantic -Wall utils.c -o utils.o
