#ifndef GLOBAL_H
#define GLOBAL_H

#define MAX_LABEL_LENGTH 31
#define MAX_LABEL_LEN MAX_LABEL_LENGTH
#define MAX_INSTRUCTION_NAME_LENGTH 4  /*The maximum possible instruction name length */
#define CAPACITY_SYMBOL_TABLE 10  /*Default symbol table size */

#define FOREVER while(1)
extern int IC;
extern int DC;
/* הגדרת טיפוס של בית בודד (8 סיביות ללא סימן) */
typedef unsigned char BYTE;

/* משתנים עבור תמונת הקוד (מנוהלת על ידי IC) */
extern BYTE *code_image;
extern int code_capacity;

/* משתנים עבור תמונת הנתונים (מנוהלת על ידי DC) */
extern BYTE *data_image;
extern int data_capacity;


typedef enum {R,I,J} type;
typedef struct{
  char name[MAX_INSTRUCTION_NAME_LENGTH +1];
  type op_type;
  int funct;
  int opcode;
} instruction;
extern instruction instructions [];

typedef enum {
    DIRECTIVE_DB,
    DIRECTIVE_DH,
    DIRECTIVE_DW
} DirectiveType;
  
#endif

