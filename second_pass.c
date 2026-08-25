/**
 * @file second_pass.c
 * @brief Implementation of the assembler's second pass.
 *
 * The program scans the source file again, completes missing binary encodings
 * for labels, updates the list of .entry attributes, and writes the output files.
 * Written strictly according to ANSI C (C90) standards.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "global.h"
#include "utils.h"
#include "symbol_table.h"
#include "second_pass.h"

/**
 * @brief Executes the second pass of the assembler.
 *
 * Resolves symbol addresses, finalizes machine code encodings, validates
 * entry directives, and coordinates output file creation.
 *
 * @param am_filename The name of the source .am file to process.
 * @param ob_filename The name of the target .ob output file.
 * @param ext_filename The name of the target .ext output file.
 * @param ent_filename The name of the target .ent output file.
 * @param icf Pointer to the final Instruction Counter value.
 * @param dcf Pointer to the final Data Counter value.
 * @return 1 on success, 0 if errors were found or the file could not be opened.
 */
int run_second_pass(char *am_filename, char *ob_filename, char *ext_filename, char *ent_filename, int *icf, int *dcf) {
    /* Variable declarations - must appear at the beginning of the block in C90 */
    int opcode;
    int line_number;
    int bytes_read;
    int error_found;
    size_t word_len;
    int offset = 0;
    
    char line[MAX_LINE_LENGTH];
    char word[MAX_LINE_LENGTH];
    char command_name[MAX_LINE_LENGTH];
    char entry_label[MAX_LABEL_LENGTH];
    char *parameters;
    char *next_part;
    char *rest;
    
    FILE *fp;

    /* Variable initialization */
    opcode = 0;
    line_number = 0;
    bytes_read = 0;
    error_found = 0;
    
    /* Reset the Instruction Counter (IC) for the second pass */
    IC = IC_INIT_VALUE;

    fp = fopen(am_filename, "r");
    if(fp == NULL) {
        fprintf(stderr, "Fatal Error: Cannot open file %s.\n", am_filename);
        return 0;
    }

    /* Step 1: Read the next line from the source code. If EOF, proceed to completion */
    while(fgets(line, sizeof(line), fp) != NULL) {
        line_number++;
        word[0] = '\0';
        command_name[0] = '\0';
        bytes_read = 0;

        /* Step 2: Skip comments and empty lines */
        if(is_empty_or_comment(line) == 1) {
            continue;
        }

        /* Extract the first word in the line */
        if(sscanf(line, "%s %n", word, &bytes_read) == 1) {
            word_len = strlen(word);
            
            /* Step 3: Skip label definitions at line beginning */
            if(word_len > 0 && word[word_len - 1] == ':') {
                next_part = line + bytes_read;
                if(sscanf(next_part, "%s %n", command_name, &bytes_read) != 1) {
                    continue; /* Handled in the first pass */
                }
                parameters = next_part + bytes_read;
            } else {
                strcpy(command_name, word);
                parameters = line + bytes_read;
            }

            /* Step 4: Skip data and extern directives */
            if(is_data_directive(command_name) || strcmp(command_name, ".extern") == 0) {
                continue; 
            }

            /* Step 5: Process .entry directives */
            if(strcmp(command_name, ".entry") == 0) {
                if(sscanf(parameters, "%s%n", entry_label, &offset) == 1) {
                    /* Step 6: Mark symbol as entry in the symbol table */
                    if (update_symbol_as_entry(entry_label) == 0) {
                        fprintf(stderr, "Error in line %d: Symbol '%s' for .entry not found in symbol table or or is defined as an external symbol\n", line_number, entry_label);
                        error_found++;
                    }
                    rest = skip_spaces(parameters + offset);
                    if (*rest != '\0') {
                        fprintf(stderr, "Error in line %d: Extraneous text '%s'\n", line_number, rest);
                        error_found++;
                    }
                }
                continue;
            }

            /* 
             * Step 7: Complete binary encodings for instructions.
             */
            opcode = get_opcode(command_name);
            if(opcode != -1) {
                /* Branch instructions - I type (opcodes 15-18: bne, beq, blt, bgt) */
                if(opcode >= MIN_BRANCH_OPCODE && opcode <= MAX_BRANCH_OPCODE) {
                    if(complete_I_branch_instruction(parameters, IC, line_number) == 0) {
                        error_found++;
                    }
                }
                /* Jump instructions - J type (opcodes 30-32: jmp, la, call) */
                else if(opcode >= MIN_JUMP_OPCODE && opcode <= MAX_JUMP_OPCODE) {
                    if(complete_J_instruction(parameters, IC, line_number) == 0) {
                        error_found++;
                    }
                    /* Step 8: External symbol tracking is handled inside complete_J_instruction */
                }

                /* Increment IC by instruction size */
                IC += INSTRUCTION_SIZE_BYTES;
            }
        }
    }

    fclose(fp);

    /* Step 9: Verify whether errors were encountered during processing */
    if(error_found > 0) {
        fprintf(stderr, "%d errors found during second pass in file %s. Output files will not be generated.\n", error_found, am_filename);
        return 0;
    }

    /* Step 10: Generate output files */
    if(write_output_files(ob_filename, ext_filename, ent_filename, *icf, *dcf) == 0) {
        fprintf(stderr, "Error: Failed to create output files for %s.\n", am_filename);
        return 0;
    }

    return 1;
}

