/**
 * @file second_pass.h
 * @brief Header file for the assembler's second pass.
 *
 * This file declares the interface for executing the second pass of the 
 * assembly process, updating symbol attributes, completing memory image 
 * encodings, and generating output files.
 */

#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include "global.h"

/* Instruction opcode boundaries */
/** @brief Minimum opcode for conditional branch instructions (I-type). */
#define MIN_BRANCH_OPCODE 15
/** @brief Maximum opcode for conditional branch instructions (I-type). */
#define MAX_BRANCH_OPCODE 18
/** @brief Minimum opcode for jump instructions (J-type). */
#define MIN_JUMP_OPCODE 30
/** @brief Maximum opcode for jump instructions (J-type). */
#define MAX_JUMP_OPCODE 32
/** @brief Size in bytes of a single machine instruction. */
#define INSTRUCTION_SIZE_BYTES 4

/**
 * @brief Executes the second pass of the assembler algorithm.
 * 
 * Scans the source file again to complete missing binary encodings for symbols,
 * updates entry attributes in the symbol table, and creates the output files.
 * 
 * @param am_filename The name of the source .am file to process.
 * @param ob_filename The name of the target .ob output file.
 * @param ext_filename The name of the target .ext output file.
 * @param ent_filename The name of the target .ent output file.
 * @param icf Pointer to the final Instruction Counter value.
 * @param dcf Pointer to the final Data Counter value.
 * @return 1 on success, 0 if errors were encountered or files could not be handled.
 */
int run_second_pass(char *am_filename, char *ob_filename, char *ext_filename, char *ent_filename, int *icf, int *dcf);

#endif /* SECOND_PASS_H */
