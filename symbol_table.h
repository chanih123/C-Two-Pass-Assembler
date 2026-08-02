#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "global.h"

typedef enum {external, code, data} SymbolType;
typedef struct{
  char name[MAX_LABEL_LENGTH +1];
  int value;
  SymbolType type;
  int is_entry;
} Symbol;

Symbol* init_symbol_table(int initial_capacity);
int add_symbol(Symbol **symbol_table, int *symbol_count, int *symbol_capacity, char *name, int value, SymbolType type, int is_entry, int line_number);
int find_symbol_index(Symbol *symbol_table, int symbol_count, char *name);
void update_data_symbol_table(Symbol **symbol_table, int *symbol_count, int *symbol_capacity, int ICF);
void free_symbol_table();


#endif
