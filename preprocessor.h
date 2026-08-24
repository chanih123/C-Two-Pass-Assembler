/**
 * @file preprocessor.h
 * @brief Header file for the pre-assembler (macro processing) stage.
 *
 * This file defines the Macro data structure and declares the functions
 * responsible for expanding macros before the first pass of the assembler.
 */

#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <stdio.h>
#include "global.h"

/* Project status constants */
/** @brief Indicates a successful operation. */
#define SUCCESS 1
/** @brief Indicates a failed operation. */
#define FAILURE 0

/**
 * @brief Data structure representing a macro definition.
 * 
 * Defines a node in a linked list of macros, storing the macro's name,
 * its expanded content, the line number where it was defined, and a 
 * pointer to the next macro in the list.
 */
typedef struct Macro {
    char *name;          /**< The name/identifier of the macro */
    char *content;       /**< The body content of the macro */
    int line_number;     /**< Line number of definition (for error tracking) */
    struct Macro *next;  /**< Pointer to the next macro in the linked list */
} Macro;

/* Pre-assembler function prototypes */

/**
 * @brief Allocates and initializes a new Macro structure.
 * @param name The name of the macro.
 * @param line_number The line number where it was defined.
 * @return A pointer to the newly created Macro, or NULL on failure.
 */
Macro *create_macro(const char *name, int line_number);

/**
 * @brief Appends a new macro to the end of the macro linked list.
 * @param head A pointer to the head of the macro list.
 * @param new_macro The new macro node to append.
 * @return SUCCESS (1) or FAILURE (0).
 */
int add_macro(Macro **head, Macro *new_macro);

/**
 * @brief Searches for a macro by its name in the linked list.
 * @param head The head of the macro linked list.
 * @param name The name of the macro to search for.
 * @return A pointer to the found Macro, or NULL if not found.
 */
Macro *find_macro(Macro *head, const char *name);

/**
 * @brief Frees all memory allocated for the macro linked list.
 * @param head The head of the macro linked list to free.
 */
void free_macros(Macro *head);

/**
 * @brief Appends a line of text to the content of an existing macro.
 * @param macro The macro to which the line will be appended.
 * @param line The line of text to append.
 * @return 1 on success, 0 on failure.
 */
int append_macro_line(Macro *macro, const char *line);

/**
 * @brief Core function: processes macros in the input file and expands them.
 * @param input_file Pointer to the open source assembly file (.as).
 * @param output_file Pointer to the open target expanded file (.am).
 * @param macro_head Pointer to the head of the macro linked list.
 * @return SUCCESS (1) if processed without errors, FAILURE (0) otherwise.
 */
int process_macros(FILE *input_file, FILE *output_file, Macro **macro_head);

/**
 * @brief Validates a macro name against reserved keywords.
 * @param name The name to validate.
 * @return 1 if valid, 0 otherwise.
 */
int is_valid_macro_name(const char *name);

#endif /* PREPROCESSOR_H */
