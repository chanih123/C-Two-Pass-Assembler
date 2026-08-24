/**
 * @file global.h
 * @brief Global constants, macros, and variable declarations for the assembler.
 *
 * This file contains shared definitions and external declarations used across
 * multiple modules of the assembler project, such as memory counters, 
 * data structures, and constants.
 */

#ifndef GLOBAL_H
#define GLOBAL_H

#include <stddef.h>

/* Constants and Buffer Sizes */

/** @brief Maximum allowed length for a single line of assembly code. */
#define MAX_LINE_LENGTH 1024

/** @brief Maximum allowed length for a label name (including null terminator). */
#define MAX_LABEL_LENGTH 32

/** @brief Alias for the maximum label length. */
#define MAX_LABEL_LEN MAX_LABEL_LENGTH

/** @brief Initial allocation size for dynamic arrays. */
#define INITIAL_CAPACITY 10

/** @brief Starting memory address for the Instruction Counter (as specified in the project). */
#define IC_INIT_VALUE 100

/** @brief Infinite loop helper macro. */
#define FOREVER while(1)

/** @brief Represents a single 8-bit unsigned byte. */
typedef unsigned char BYTE;

/* External declarations of global counters */

/** @brief Extern declaration of the Instruction Counter. */
extern int IC;

/** @brief Extern declaration of the Data Counter. */
extern int DC;

/* External declarations of memory images */

/** @brief Extern declaration of the code image buffer. */
extern BYTE *code_image;

/** @brief Extern declaration of the code image's capacity. */
extern int code_capacity;

/** @brief Extern declaration of the data image buffer. */
extern BYTE *data_image;

/** @brief Extern declaration of the data image's capacity. */
extern int data_capacity;

/**
 * @brief Supported data directive types.
 * 
 * Used to identify the specific type of memory allocation requested by data directives.
 */
typedef enum {
    DIRECTIVE_DB, /**< Byte directive (.db) - allocates 1 byte */
    DIRECTIVE_DH, /**< Half-word directive (.dh) - allocates 2 bytes */
    DIRECTIVE_DW  /**< Word directive (.dw) - allocates 4 bytes */
} DirectiveType;

#endif /* GLOBAL_H */

