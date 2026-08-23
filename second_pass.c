#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "global.h"
#include "utils.h"
#include "symbol_table.h"

/*
 * The function that executes the second pass of the assembler.
 * The program scans the source file again, completes missing binary encodings
 * for labels, and updates the list of .entry attributes.
 * Written strictly in ANSI C (C90) standard.
 */
int run_second_pass(char *filename) {
    /* Variable declarations - must appear at the beginning of the block in C90 */
    int opcode;
    int line_number;
    int bytes_read;
    int error_found;
    size_t word_len;
    
    char line[MAX_LINE_LENGTH];
    char word[MAX_LINE_LENGTH];
    char command_name[MAX_LINE_LENGTH];
    char entry_label[MAX_LABEL_LENGTH];
    char *parameters;
    char *next_part;
    
    FILE *fp;

    /* Variable initialization */
    opcode = 0;
    line_number = 0;
    bytes_read = 0;
    error_found = 0;
    
    /* Reset the Instruction Counter (IC) to 100 for the second pass[cite: 1] */
    IC = 100;

    fp = fopen(filename, "r");
    if(fp == NULL) {
        fprintf(stderr, "Fatal Error: Cannot open file %s.\n", filename);
        return 0;
    }

    /* Step 1: Read the next line from the source code. If EOF, go to step 9[cite: 1] */
    while(fgets(line, sizeof(line), fp) != NULL) {
        line_number++;
        word[0] = '\0';
        command_name[0] = '\0';
        bytes_read = 0;

        /* Step 2: If this is a comment or an empty line, go back to step 1[cite: 1] */
        if(is_empty_or_comment(line) == 1) {
            continue;
        }

        /* Extract the first word in the line */
        if(sscanf(line, "%s %n", word, &bytes_read) == 1) {
            word_len = strlen(word);
            
            /* Step 3: If the first field in the line is a symbol (label), skip it[cite: 1] */
            if (word_len > 0 && word[word_len - 1] == ':') {
                next_part = line + bytes_read;
                if(sscanf(next_part, "%s %n", command_name, &bytes_read) != 1) {
                    continue; /* If there is nothing after the label, the error was handled in the first pass */
                }
                parameters = next_part + bytes_read;
            } else {
                strcpy(command_name, word);
                parameters = line + bytes_read;
            }

            /* Step 4: Is this a data directive or .extern? If so, go back to step 1[cite: 1] */
            if(is_data_directive(command_name) || strcmp(command_name, ".extern") == 0) {
                continue; 
            }

            /* Step 5: Is this an .entry directive? If not, go to step 7[cite: 1] */
            if(strcmp(command_name, ".entry") == 0) {
                if(sscanf(parameters, "%s", entry_label) == 1) {
                    /* Step 6: Add the entry attribute to the symbol in the symbol table. If not found, report an error[cite: 1] */
                    if (update_symbol_as_entry(symbol_table, entry_label) == 0) {
                        fprintf(stderr, "Error in line %d: Symbol '%s' for .entry not found in symbol table.\n", line_number, entry_label);
                        error_found++;
                    }
                }
                continue; /* Go back to step 1 */
            }

            /* 
             * Step 7: This is an instruction line. Complete the missing binary encoding in the memory image.
             * For J type instructions (except hlt), encode the label's address.
             * For conditional branch instructions (I type), calculate and encode the distance[cite: 1].
             */
            opcode = get_opcode(command_name);
            if(opcode != -1) {
                /* Branch instructions - I type (opcodes 15-18: bne, beq, blt, bgt) */
                if(opcode >= 15 && opcode <= 18) {
                    if(complete_I_branch_instruction(parameters, IC, line_number) == 0) {
                        error_found++;
                    }
                }
                /* Jump instructions - J type (opcodes 30-32: jmp, la, call) */
                else if(opcode >= 30 && opcode <= 32) {
                    if(complete_J_instruction(parameters, IC, line_number) == 0) {
                        error_found++;
                    }
                    /* Step 8: Handling of external symbols is executed inside complete_J_instruction[cite: 1] */
                }

                /* Increment IC by 4 for each instruction (each instruction takes 4 bytes)[cite: 1] */
                IC += 4;
            }
        }
    }

    fclose(fp);

    /* Step 9: The source code has been completely scanned. If errors were found, stop here[cite: 1] */
    if(error_found > 0) {
        fprintf(stderr, "%d errors found during second pass in file %s. Output files will not be generated.\n", error_found, filename);
        return 0; /* Return failure */
    }

    /* Step 10: Success, build the output files (returns 1 so main can call the output generation function)[cite: 1] */
    return 1;
}
