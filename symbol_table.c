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
 * Initializes and allocates memory for a new dynamic symbol table array.
 *
 * @param initial_capacity Initial number of Symbol elements to allocate.
 * @return Pointer to the allocated Symbol array.
 */
 
Symbol *init_symbol_table(int initial_capacity){
  Symbol *table = NULL;
  symbol_capacity = INITIAL_CAPACITY;
  symbol_count = 0;
  table = (Symbol *) malloc(initial_capacity * sizeof(Symbol));
    
  if(table == NULL){
      fprintf(stderr, "Fatal Error: Memory allocation failed for symbol table\n");
      exit(1);
  }
  return table;
}

/**
 * Adds a new symbol to the dynamic symbol table array.
 * Validates that the symbol is not already defined and dynamically expands
 * table capacity if necessary.
 *
 * @param symbol_table Pointer to the symbol table array pointer.
 * @param symbol_count Pointer to current count of stored symbols.
 * @param symbol_capacity Pointer to total allocated capacity.
 * @param name Identifier name of the symbol.
 * @param value Memory address/offset (IC or DC value).
 * @param type Classification type (CODE_SYMBOL, DATA_SYMBOL, EXTERNAL_SYMBOL).
 * @param is_entry Boolean flag indicating if symbol is an entry (0 or 1).
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
        symbol_capacity = (symbol_capacity == 0) ? INITIAL_CAPACITY : (symbol_capacity * 2);;
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
 * Frees all allocated memory of the symbol table and resets counters.
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
 * Searches for a symbol by name.
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
 * Updates all data symbols in the symbol table by adding the final IC value (ICF)
 * to their relative data offset (DC), as required at the end of First Pass (Step 17).
 *
 * @param final_ic The final instruction counter value at the end of first pass.
 */
void update_data_symbol_table(int final_ic){
    int i;
    for(i = 0; i < symbol_count; i++){
        if(symbol_table[i].type ==  data)
            symbol_table[i].value += final_ic;
    }
}
/****************************/
int get_symbol_count(void) {
    return symbol_count;
}

Symbol *get_symbol_by_index(int index) {
    if (index < 0 || index >= symbol_count) {
        return NULL;
    }
    return &symbol_table[index];
}

