#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>
#include <string.h>
typedef enum {R,I,J} type;
typedef struct{
  char name[5];
  int opcode;
  int funct;
  type op_type;
} instruction;

int get_opcode();


#endif
