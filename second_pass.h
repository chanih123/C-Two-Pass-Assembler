#ifndef SECOND_PASS_H
#define SECOND_PASS_H

/*
 * Executes the second pass of the assembler algorithm.
 * Scans the file again to complete missing binary encodings for symbols,
 * and updates the entry attributes in the symbol table.
 * 
 * @param filename The name of the file to process.
 * @return 1 on success, 0 if errors were found or file could not be opened.
 */
int run_second_pass(char *filename);

#endif /* SECOND_PASS_H */
