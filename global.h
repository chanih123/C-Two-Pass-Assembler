#ifndef GLOBAL_H
#define GLOBAL_H

#include <stddef.h>

/* Constants and Buffer Sizes */
#define MAX_LINE_LENGTH 1024
#define MAX_LABEL_LENGTH 32
#define MAX_LABEL_LEN MAX_LABEL_LENGTH
#define INITIAL_CAPACITY 10
#define IC_INIT_VALUE 100

/** Infinite loop helper macro. */
#define FOREVER while(1)

/** Represents a single 8-bit unsigned byte. */
typedef unsigned char BYTE;

extern int IC;
extern int DC;

extern BYTE *code_image;
extern int code_capacity;

extern BYTE *data_image;
extern int data_capacity;

/**
 * Supported data directive types.
 */
typedef enum {
    DIRECTIVE_DB,
    DIRECTIVE_DH,
    DIRECTIVE_DW
} DirectiveType;

 
#endif

