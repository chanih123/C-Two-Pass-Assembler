/**
 * @file utils.h
 * @brief Header file containing definitions, macros, and utility function declarations.
 */

#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include "global.h"
#include "preprocessor.h"

/** @brief Return status when a command name is not found in the instruction table. */
#define CMD_NOT_FOUND -1

/** @brief Total number of supported machine instructions (R, I, J types). */
#define NUM_OF_INSTRUCTIONS 27

/** @brief Memory size of a single instruction word in bytes. */
#define MEMORY_WORD_SIZE 4

/** @brief Maximum string length of an instruction mnemonic name (e.g., "addi", "call"). */
#define MAX_INSTRUCTION_NAME_LENGTH 5

/* Directive name constants */
#define DIRECTIVE_DH_STR    ".dh"
#define DIRECTIVE_DW_STR   ".dw"
#define DIRECTIVE_DB_STR    ".db"
#define DIRECTIVE_ASCIZ_STR ".asciz"

/* Bit shift amounts for R-type instruction fields */
#define OPCODE_SHIFT    26
#define RS_SHIFT        21
#define RT_SHIFT        16
#define RD_SHIFT        11
#define FUNCT_SHIFT     6

/* Field bitmasks */
#define OPCODE_MASK     0x3F
#define REG_MASK        0x1F
#define FUNCT_MASK      0x1F
#define BYTE_MASK       0xFF
#define IMMED_16BIT_MASK 0xFFFF

/* Expected register operand counts */
#define R_ARITHMETIC_REG_COUNT 3
#define R_COPY_REG_COUNT       2

/* 16-bit signed integer limits for I-type immediate field */
#define MIN_IMMED_VAL  -32768
#define MAX_IMMED_VAL   32767

/* Opcode ranges for I-type instructions */
#define MIN_I_MATH_OPCODE 10
#define MAX_I_MATH_OPCODE 14
#define MIN_I_BRANCH_OPCODE 15
#define MAX_I_BRANCH_OPCODE 18
#define MIN_I_LOAD_STORE_OPCODE 19
#define MAX_I_LOAD_STORE_OPCODE 24

/* Opcode definitions for J-type instructions */
#define OPCODE_JMP  30
#define OPCODE_LA   31
#define OPCODE_CALL 32
#define OPCODE_HLT  63

/* J-type register bit flag and shift */
#define J_REG_FLAG_SHIFT 25
#define J_ADDRESS_MASK   0x1FFFFFF

/* Register definitions */
#define REGISTER_PREFIX   '$'
#define MIN_REGISTER_NUM  0
#define MAX_REGISTER_NUM  31
#define INVALID_REGISTER -1

#define STRING_QUOTE '"'

/* Data directive numerical ranges */
#define MIN_DB -128
#define MAX_DB 127
#define MIN_DH -32768
#define MAX_DH 32767
#define MIN_DW -2147483648L
#define MAX_DW 2147483647L

#define BITS_IN_BYTE 8

/** @brief Instruction classification category (R, I, or J). */
typedef enum {R,I,J} type;

/**
 * @brief Metadata representation of a supported machine instruction.
 */
typedef struct{
  char name[MAX_INSTRUCTION_NAME_LENGTH +1]; /**< Instruction mnemonic */
  type op_type;                              /**< Instruction format type */
  int funct;                                 /**< Funct field (if applicable) */
  int opcode;                                /**< Primary opcode value */
} instruction;

extern instruction instructions [];

/* External Reference Node Structure */
/**
 * @brief Node for a linked list tracking external symbol references.
 * 
 * Used to collect all addresses where an external symbol is used 
 * so they can be written to the .ext file.
 */
typedef struct ExtNode {
    char name[MAX_LABEL_LENGTH];
    int address;
    struct ExtNode *next;
} ExtNode;

/* Utils Interface Functions */

int get_opcode(char *name);
int get_funct(char *name);
int has_parameters(char *str);
int is_reserved_keyword(char *name);
int is_valid_label(char *word, Macro *macro_head, int line_number);
int is_data_directive(char *command_name);
int is_empty_or_comment(char *line);
char *extract_token(char *src, char *dest, int max_len);
char *match_comma(char *p, int line_number);
int check_and_enter_R_function_parameter(char *parameters, int opcode, int funct, int line_number);
void insert_to_code_image(unsigned int word);
int check_and_enter_I_function_parameter(char *parameters, int opcode, Macro *macro_head, int line_number);
int check_end_enter_J_function_parameter(char *parameters, int opcode, Macro *macro_head, int line_number);
int check_register(char *reg, int line_number);
int check_asciz_parameter(char *parameters, int line_number);
char* skip_spaces(char *ptr);
int check_directive_parameter(char *line, DirectiveType type, int line_number);
void enter_to_data_image(char *parameters, int size);
void enter_asciz_to_data_image(char *parameters);

/* Second Pass Completion Functions */
int complete_I_branch_instruction(char *parameters, int ic, int line_number);
int complete_J_instruction(char *parameters, int ic, int line_number);
int write_output_files(char *ob_filename, char *ext_filename, char *ent_filename, int icf, int dcf);
void free_ext_references(void);

#endif /* UTILS_H */
