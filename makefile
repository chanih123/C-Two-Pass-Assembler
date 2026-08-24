# Makefile for the Assembler Project

CC = gcc
CFLAGS = -Wall -ansi -pedantic

# List of all object files needed to build the final executable
OBJS = main.o preprocessor.o first_pass.o second_pass.o symbol_table.o utils.o global.o

# The final executable target
assembler: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o assembler

# Object file rules and dependencies
main.o: main.c preprocessor.h global.h first_pass.h
	$(CC) -c $(CFLAGS) main.c -o main.o

preprocessor.o: preprocessor.c preprocessor.h global.h
	$(CC) -c $(CFLAGS) preprocessor.c -o preprocessor.o

first_pass.o: first_pass.c first_pass.h global.h utils.h symbol_table.h
	$(CC) -c $(CFLAGS) first_pass.c -o first_pass.o

second_pass.o: second_pass.c second_pass.h global.h utils.h symbol_table.h
	$(CC) -c $(CFLAGS) second_pass.c -o second_pass.o

symbol_table.o: symbol_table.c symbol_table.h global.h
	$(CC) -c $(CFLAGS) symbol_table.c -o symbol_table.o

utils.o: utils.c utils.h global.h first_pass.h symbol_table.h
	$(CC) -c $(CFLAGS) utils.c -o utils.o

global.o: global.c global.h
	$(CC) -c $(CFLAGS) global.c -o global.o

# Clean target to remove generated files and start fresh
clean:
	rm -f *.o assembler *.am *.ob *.ext *.ent
