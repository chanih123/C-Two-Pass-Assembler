assembler: main.o assembler.o preprocessor.o first_pass.o second_pass.o symbol_table.o global.o utils.o
	gcc -g -Wall -ansi -pedantic main.o assembler.o preprocessor.o first_pass.o second_pass.o symbol_table.o global.o utils.o -o assembler

main.o: main.c assembler.h global.h preprocessor.h first_pass.h second_pass.h
	gcc -c -Wall -ansi -pedantic main.c -o main.o

assembler.o: assembler.c assembler.h global.h
	gcc -c -Wall -ansi -pedantic assembler.c -o assembler.o

preprocessor.o: preprocessor.c preprocessor.h global.h utils.h
	gcc -c -Wall -ansi -pedantic preprocessor.c -o preprocessor.o

first_pass.o: first_pass.c first_pass.h global.h symbol_table.h utils.h
	gcc -c -Wall -ansi -pedantic first_pass.c -o first_pass.o

second_pass.o: second_pass.c second_pass.h global.h symbol_table.h utils.h
	gcc -c -Wall -ansi -pedantic second_pass.c -o second_pass.o

symbol_table.o: symbol_table.c symbol_table.h global.h
	gcc -c -Wall -ansi -pedantic symbol_table.c -o symbol_table.o

global.o: global.c global.h
	gcc -c -Wall -ansi -pedantic global.c -o global.o

utils.o: utils.c utils.h global.h
	gcc -c -Wall -ansi -pedantic utils.c -o utils.o

