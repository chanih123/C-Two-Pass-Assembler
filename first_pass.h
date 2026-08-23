#include "symbol_table.h"
#include "utils.h"
#include "global.h"

/* Data sizes in bytes */
#define DB_BYTES 1
#define DH_BYTES 2
#define DW_BYTES 4

/* Opcode ranges */
#define OPCODE_R_MATH 0
#define OPCODE_R_COPY 1
#define MIN_I_OPCODE 10
#define MAX_I_OPCODE 24

/* First pass Interface Function */
int run_first_pass(char *filename, Symbol **symbol_table, int *icf, int *dcf);


