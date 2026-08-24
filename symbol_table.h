/**
 * @file symbol_table.h
 * @brief Header file for symbol table management.
 *
 * Defines the symbol data structures and declares the interface for 
 * symbol table operations during the assembly passes.
 */

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "global.h"

/**
 * @brief Enum defining the possible types of a symbol.
 */
typedef enum {external, code, data} SymbolType;

/**
 * @brief Represents a single record in the assembler's symbol table.
 */
typedef struct{
  char name[MAX_LABEL_LENGTH +1]; /**< The name of the symbol (label) */
  int value;                      /**< The address or value associated with the symbol */
  SymbolType type;                /**< The type of the symbol (code, data, or external) */
  int is_entry;                   /**< Boolean flag: 1 if marked as .entry, 0 otherwise */
} Symbol;

/* Symbol Table Interface Functions */

/**
 * @brief Initializes and allocates memory for the dynamic symbol table array.
 */
void init_symbol_table(void);

/**
 * @brief Adds a new symbol to the dynamic symbol table array.
 * @param name Identifier name of the symbol.
 * @param value Memory address/offset (IC or DC value).
 * @param type Classification type (code, data, external).
 * @param line_number Current line number in source file for error reporting.
 * @return 1 on successful addition, 0 if symbol already exists or input error.
 */
int add_symbol(char *name, int value, SymbolType type, int line_number);

/**
 * @brief Frees all allocated memory of the symbol table and resets counters.
 */
void free_symbol_table(void);

/**
 * @brief Searches for a symbol by name.
 * @param name The name of the symbol.
 * @return Index in table if found, or -1 if not found.
 */
int find_symbol_index(char *name);

/**
 * @brief Updates a symbol's entry attribute.
 * @param name The name of the symbol.
 * @return 1 on success, 0 if not found.
 */
int update_symbol_as_entry(char *name);

/**
 * @brief Updates data symbol values by adding the final IC.
 * @param final_ic The final instruction counter value.
 */
void update_data_symbol_table(int final_ic);

/**
 * @brief Gets the total number of symbols currently in the table.
 * @return The symbol count.
 */
int get_symbol_count(void);

/**
 * @brief Retrieves a pointer to a symbol by its index in the table.
 * @param index The index of the symbol.
 * @return Pointer to the Symbol structure, or NULL if out of bounds.
 */
Symbol *get_symbol_by_index(int index);

#endif /* SYMBOL_TABLE_H */
