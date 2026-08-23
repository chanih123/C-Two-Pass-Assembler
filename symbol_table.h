#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "global.h"

/**
 * Represents a single record in the assembler's symbol table.
 */
typedef enum {external, code, data} SymbolType;
typedef struct{
  char name[MAX_LABEL_LENGTH +1];
  int value;
  SymbolType type;
  int is_entry;
} Symbol;

/* Symbol Table Interface Functions */
Symbol *init_symbol_table(int initial_capacity);
int add_symbol(char *name, int value, SymbolType type, int line_number);
void free_symbol_table(void);
int find_symbol_index(char *name);
void update_data_symbol_table(int final_ic);

/****************/
Symbol *init_symbol_table(int initial_capacity);
int get_symbol_count(void);
Symbol *get_symbol_by_index(int index);



#endif
