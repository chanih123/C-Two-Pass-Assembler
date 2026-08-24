/**
 * @file first_pass.h
 * @brief Header file for the assembler's first pass.
 *
 * This file contains the macro definitions and the function prototype 
 * required for executing the first pass of the assembly process.
 */

#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "symbol_table.h"
#include "utils.h"
#include "global.h"

/* Data sizes in bytes */
/** @brief Size of a byte data directive (.db) in bytes. */
#define DB_BYTES 1
/** @brief Size of a half-word data directive (.dh) in bytes. */
#define DH_BYTES 2
/** @brief Size of a word data directive (.dw) in bytes. */
#define DW_BYTES 4

/* Opcode ranges */
/** @brief Opcode for R-type arithmetic and logic instructions. */
#define OPCODE_R_MATH 0
/** @brief Opcode for R-type copy instructions. */
#define OPCODE_R_COPY 1
/** @brief Minimum opcode value for I-type instructions. */
#define MIN_I_OPCODE 10
/** @brief Maximum opcode value for I-type instructions. */
#define MAX_I_OPCODE 24

/**
 * @brief Executes the first pass of the assembler.
 * 
 * Parses lines, builds the symbol table, and encodes data directives.
 *
 * @param filename The path to the source assembly file (.am) to process.
 * @param symbol_table Pointer to the symbol table structure.
 * @param icf Pointer to store the final Instruction Counter (IC) value.
 * @param dcf Pointer to store the final Data Counter (DC) value.
 * @return 1 on success, 0 if errors were encountered.
 */
int run_first_pass(char *filename, Symbol **symbol_table, int *icf, int *dcf);

#endif /* FIRST_PASS_H */

