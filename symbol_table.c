/**
 * @file symbol_table.c
 * @brief Implementation of the dynamic symbol table for the assembler.
 *
 * This file manages the symbol table used during the first and second passes
 * of the assembly process. It provides functions to initialize the table,
 * add new symbols, find existing symbols, update entry attributes, and 
 * update data symbol addresses.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "global.h"
#include "symbol_table.h"

/* Static internal variables - visible ONLY within symbol_table.c */
static Symbol *symbol_table = NULL;
static int symbol_count = 0;
static int symbol_capacity = 0;

/**
 * @brief Initializes and allocates memory for the dynamic symbol table array.
 * 
 * Uses the default INITIAL_CAPACITY defined in global.h.
 */
void init_symbol_table(void){
  symbol_capacity = INITIAL_CAPACITY;
  symbol_count = 0;
  symbol_table = (Symbol *) malloc(symbol_capacity * sizeof(Symbol));
    
  if(symbol_table == NULL){
      fprintf(stderr, "Fatal Error: Memory allocation failed for symbol table\n");
      exit(1);
  }
}

/**
 * @brief Adds a new symbol to the dynamic symbol table array.
 * 
 * Validates that the symbol is not already defined and dynamically expands
 * table capacity if necessary.
 *
 * @param name Identifier name of the symbol.
 * @param value Memory address/offset (IC or DC value).
 * @param type Classification type (code, data, external).
 * @param line_number Current line number in source file for error reporting.
 * @return 1 on successful addition, 0 if symbol already exists or input error.
 */
int add_symbol(char *name, int value, SymbolType type, int line_number){
    Symbol *temp = NULL;

    if(name == NULL)
        return 0;
    
    /* Check if symbol already exists in table */
    if(find_symbol_index(name) != -1){
      fprintf(stderr, "Error in line %d: Symbol '%s' is already defined\n", line_number, name);
      return 0;
    }
    
    /* Expand array capacity dynamically if full */
    if(symbol_table == NULL || symbol_count >= symbol_capacity){
        symbol_capacity = (symbol_capacity == 0) ? INITIAL_CAPACITY : (symbol_capacity * 2);
        temp = (Symbol*)realloc(symbol_table, symbol_capacity * sizeof(Symbol));
        if(temp == NULL){
           printf("Expanding the symbol table memory failed.\n");
           exit(1);
        }
        symbol_table = temp;
    }
    
    /* Store symbol attributes */
    strncpy(symbol_table[symbol_count].name, name, MAX_LABEL_LENGTH);
    symbol_table[symbol_count].name[MAX_LABEL_LENGTH] = '\0';
    symbol_table[symbol_count].value = value;
    symbol_table[symbol_count].type = type;
    symbol_table[symbol_count].is_entry = 0;
    
    symbol_count++;
    return 1;
}

/**
 * @brief Frees all allocated memory of the symbol table and resets counters.
 */
void free_symbol_table(void){
    if(symbol_table != NULL){
       free(symbol_table);
       symbol_table = NULL;
    }
    symbol_count = 0;
    symbol_capacity = 0;    
}

/**
 * @brief Searches for a symbol by name.
 * 
 * @param name The name of the symbol to search for.
 * @return Index in table if found, or -1 if not found.
 */
int find_symbol_index(char *name){
    int i;
    if(name == NULL || symbol_table == NULL){
        return -1;
    }
    for(i = 0; i < symbol_count; i++){
       if(strcmp(symbol_table[i].name, name) == 0){
            return i; 
       }
    }
    return -1; 
}

/**
 * @brief Updates a symbol's entry attribute to true (1).
 *
 * @param name The name of the symbol to update.
 * @return 1 on success, 0 if the symbol was not found.
 */
int update_symbol_as_entry(char *name) {
    int index = find_symbol_index(name);
    if(index != -1) {
        if(symbol_table[index].type == external)
            return 0;
        symbol_table[index].is_entry = 1;
        return 1;
    }
    return 0;
}

/**
 * @brief Updates all data symbols in the symbol table.
 * 
 * Adds the final IC value (ICF) to their relative data offset (DC), 
 * as required at the end of First Pass.
 *
 * @param final_ic The final instruction counter value at the end of the first pass.
 */
void update_data_symbol_table(int final_ic){
    int i;
    for(i = 0; i < symbol_count; i++){
        if(symbol_table[i].type == data)
            symbol_table[i].value += final_ic;
    }
}

/****************************/

/**
 * @brief Gets the total number of symbols currently in the table.
 * 
 * @return The symbol count.
 */
int get_symbol_count(void) {
    return symbol_count;
}

/**
 * @brief Retrieves a pointer to a symbol by its index in the table.
 * 
 * @param index The index of the symbol.
 * @return Pointer to the Symbol structure, or NULL if the index is out of bounds.
 */
Symbol *get_symbol_by_index(int index) {
    if (index < 0 || index >= symbol_count) {
        return NULL;
    }
    return &symbol_table[index];
}
