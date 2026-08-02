#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "global.h"
#include <stdlib.h>
#include <ctype.h>

/*לחשוב איפה הכי טוב לכתוב את זה*/

instruction instructions[] = {
    {"add", R, 1, 0},
    {"sub", R, 2, 0},
    {"and", R, 3, 0},
    {"or",  R, 4, 0},
    {"nor", R, 5, 0},
    {"move",R, 1, 1},
    {"mvhi",R, 2, 1},
    {"mvlo",R, 3, 1},
    {"addi",I, 0, 10},
    {"subi",I, 0, 11},
    {"andi",I, 0, 12},
    {"ori", I, 0, 13},
    {"nori",I, 0, 14},
    {"bne", I, 0, 15},
    {"beq", I, 0, 16},
    {"blt", I, 0, 17},
    {"bgt", I, 0, 18},
    {"lb",  I, 0, 19},
    {"sb",  I, 0, 20},
    {"lw",  I, 0, 21},
    {"sw",  I, 0, 22},
    {"lh",  I, 0, 23},
    {"sh",  I, 0, 24},
    {"jmp", J, 0, 30},
    {"la",  J, 0, 31},
    {"call",J, 0, 32},
    {"hlt", J, 0, 63}
};
int get_opcode(char *name)
{
  int i = 0;
  while(i < sizeof(instructions)/sizeof(instruction))
  {
    if(strcmp(name,instructions[i].name)==0)
    {
      return instructions[i].opcode;
    }
    i++;
  }
  return -1;
}
int get_funct(char * name)
{
  int i = 0;
  while(i < sizeof(instructions)/sizeof(instruction))
  {
    if(strcmp(name,instructions[i].name)==0)
    {
      return instructions[i].funct;
    }
    i++;
  }
  return -1;
}
int has_parameters(char *str){
if(str == NULL)
        return 0;
while(*str != '\0'){
     if(!isspace((unsigned char)*str))
        return 1; 
      str++;
}
return 0; 
}
int is_valid_label(char *word, int line_number){
    int length = strlen(word);
    int i;
    if (length < 2 || (length - 1) > MAX_LABEL_LENGTH){
        fprintf(stderr, "Error in line %d: The label name is too long\n", line_number);
        return 0;
        }
    if(!((word[0] >= 'a' && word[0] <= 'z') || (word[0] >= 'A' && word[0] <= 'Z'))){
      fprintf(stderr, "Error in line %d: Label name does not start with a letter\n", line_number);
      return 0;
      }
      for (i = 1; i < length - 1; i++){
        if(!((word[i] >= 'a' && word[i] <= 'z') || (word[i] >= 'A' && word[i] <= 'Z') || (word[i] >= '0' && word[i] <= '9'))){
            fprintf(stderr, "Error in line %d: Invalid label name\n", line_number);
             return 0;
             }
    } /*end for*/
    word[length - 1] = '\0';
    if(get_opcode(word) != -1){
         fprintf(stderr, "Error in line %d: Label name cannot be a reserved instruction word\n", line_number);
        word[length - 1] = ':';
        return 0;
        }
    return 1;
}
int is_data_directive(char *command_name){
  if(command_name[0] != '.')
    return 0;
  if((strcmp(command_name, ".dh") == 0) || (strcmp(command_name, ".dw") == 0) || (strcmp(command_name, ".db") == 0) || (strcmp(command_name, ".asciz") == 0))
      return 1;
  return 0;
}

int is_empty_or_comment(char *line){
  int i = 0;
  while(line[i] == '\t' || line[i] == ' '){
      i++;
  }
  if(line[i] == '\0' || line[i] == '\n' || line[i] == '\r' || line[i] == ';'){
    return 1;
  }
  return 0;
}
int check_and_enter_R_function_parameter(char *parameters, int opcode, int funct, int line_number){
char *reg;
char copy[100];
int reg_num = 0;
unsigned int encoded = 0;
int count = 0;
int i;
BYTE *temp;

strcpy(copy, parameters);
encoded = encoded | (opcode & 0x3F) << 26;
encoded = encoded | (funct & 0x1F) << 6;
reg = strtok(copy, ", \t\n");

while(reg != NULL){
    reg_num = check_register(reg, line_number) ;
    if(reg_num == -1)
      return 0;
    if(opcode == 0){
      switch(count){
        case 0:
            encoded  = encoded | (reg_num & 0x1F) << 21;
            break;
        case 1:
            encoded  = encoded | (reg_num & 0x1F) << 16;
            break;
        case 2:
            encoded  = encoded | (reg_num & 0x1F) << 11;
            break;
      }
    }
    else{
      switch(count){
        case 0:
            encoded  = encoded | (reg_num & 0x1F) << 21;
            break;
        case 1:
            encoded  = encoded | (reg_num & 0x1F) << 11;
            break;
      }
    }    
    count++;
    reg = strtok(NULL, ", \t\n");
}
if((opcode == 0 && count != 3) || (opcode == 1 && count != 2)){
  fprintf(stderr, "Error in line %d: Invalid number of registers\n", line_number);
  return 0;
}
if(IC + 4 >= code_capacity){
     code_capacity = (code_capacity == 0) ? 100 : code_capacity * 2;
     temp = (BYTE *) realloc(code_image, code_capacity * sizeof(BYTE));
     if(temp == NULL){
     fprintf(stderr, "Fatal Error: Memory allocation failed for code_capacity\n");
     exit(1);
     }
     code_image = temp;
}
for(i = 0; i < 4; i++){
    code_image[IC] = (encoded >> (i * 8)) & 0xFF;
    IC++;
}
return 1;
}
int check_and_enter_I_function_parameter(char *parameters, int opcode, int line_number){

char *reg;
char *end;
char copy[100];
long immed;
int reg_num = 0;
int i;
unsigned int encoded = 0;
BYTE *temp;

encoded = encoded | (opcode & 0x3F) << 26;
strcpy(copy, parameters);
reg = strtok(copy, ", \t\n");
reg_num = check_register(reg, line_number);
if(reg == NULL || reg_num == -1)
    return 0;
encoded  = encoded | (reg_num & 0x1F) << 21;
if((opcode >= 10 && opcode <= 14) || (opcode >= 19 && opcode <=24)){
    reg = strtok(NULL, ", \t\n");
    if(reg == NULL){
        fprintf(stderr, "Error in line %d: Missing immediate value\n", line_number);
        return 0;
    }
    immed = strtol(reg, &end, 10);
    if(*end != '\0' || end == reg){
        fprintf(stderr, "Error in line %d: Invalid immediate\n", line_number);
        return 0;
    }
    if(immed > 32767 || immed < -32768){
       fprintf(stderr, "Error in line %d: Invalid immed\n", line_number);
       return 0;
    }
    reg = strtok(NULL, ", \t\n");
    reg_num = check_register(reg, line_number) ;
    if(reg == NULL || reg_num == -1)
      return 0;
    encoded  = encoded | (reg_num & 0x1F) << 16;
    encoded  = encoded | (immed & 0xFFFF);
}
else{
  if(opcode >= 15 && opcode <= 18){
     reg = strtok(NULL, ", \t\n");
     reg_num = check_register(reg, line_number) ;
     if(reg == NULL || reg_num == -1)
         return 0;
      encoded  = encoded | (reg_num & 0x1F) << 16;
      reg = strtok(NULL, ", \t\n");
      if(reg == NULL || is_valid_label(reg, line_number) == 0)
         return 0;
  }
}
reg = strtok(NULL, ", \t\n");
if(reg != NULL){
  fprintf(stderr, "Error in line %d: Superfluous parameters\n", line_number);
  return 0;
}
if(IC + 4 >= code_capacity){
     code_capacity = (code_capacity == 0) ? 100 : code_capacity * 2;
     temp = (BYTE *) realloc(code_image, code_capacity * sizeof(BYTE));
     if(temp == NULL){
        fprintf(stderr, "Fatal Error: Memory allocation failed for code_capacity\n");
        exit(1);
     }
     code_image = temp;
}
for(i = 0; i < 4; i++){
    code_image[IC] = (encoded >> (i * 8)) & 0xFF;
    IC++;
}
return 1;
}
int check_end_enter_J_function_parameter(char *parameters, int opcode, int line_number){
char *reg;
char copy[100];
int i;
int reg_num;
unsigned int encoded = 0;
BYTE *temp;

encoded = encoded | (opcode & 0x3F) << 26;
strcpy(copy, parameters);
reg = strtok(copy, ", \t\n");
if(opcode != 63){
if(has_parameters(parameters) == 0){
  fprintf(stderr, "Error in line %d: Superfluous parameters\n", line_number);
  return 0;
}
if(reg == NULL){
  fprintf(stderr, "Error in line %d: Missing parameter for J instruction\n", line_number);
  return 0;
}  
if(opcode == 31 || opcode == 32){
  if(is_valid_label(reg, line_number) == 0)
         return 0;
}
if(opcode == 30){
    if(reg[0] == '$'){
        reg_num = check_register(reg, line_number);
        if(reg_num == -1)
            return 0;
        encoded = encoded | (1U << 25);
        encoded |= (reg_num & 0x1FFFFFF);
    }
    else{
        if(is_valid_label(reg, line_number) == 0)
        return 0;
    }
}
reg = strtok(NULL, ", \t\n");
} /*end if(opcode != 63)*/
if(reg != NULL){
  fprintf(stderr, "Error in line %d: Superfluous parameters\n", line_number);
  return 0;
}
if(IC + 4 >= code_capacity){
     code_capacity = (code_capacity == 0) ? 100 : code_capacity * 2;
     temp = (BYTE *) realloc(code_image, code_capacity * sizeof(BYTE));
     if(temp == NULL){
        fprintf(stderr, "Fatal Error: Memory allocation failed for code_capacity\n");
        exit(1);
     }
     code_image = temp;
}
for(i = 0; i < 4; i++){
    code_image[IC] = (encoded >> (i * 8)) & 0xFF;
    IC++;
}
return 1;
}
int check_register(char *reg, int line_number){
char *end;
long num;
if(reg[0] != '$'){
  fprintf(stderr, "Error in line %d: Invalid Register name\n", line_number);
  return -1;
}
num = strtol(reg + 1, &end, 10);
if(*end != '\0' || end == reg + 1){
  fprintf(stderr, "Error in line %d: Invalid Register name\n", line_number);
  return -1;
}
if(num<0 || num >31){
  fprintf(stderr, "Error in line %d: Invalid Register name\n", line_number);
  return -1;
}
return (int)num; 
}
int check_asciz_parameter(char *parameters, int line_number){
int i = 0;
while(parameters[i] == '\t' || parameters[i] == ' '){
      i++;
}
if(parameters[i] == '"')
  i++;
else{
  fprintf(stderr, "Error in line %d: Invalid string\n", line_number);
  return 0;
}
while(parameters[i] != '"' && parameters[i] != '\0' && parameters[i] != '\n'){
      i++;
}
if(parameters[i] != '"'){
  fprintf(stderr, "Error in line %d: Invalid string\n", line_number);
  return 0;
}
i++;
while(parameters[i] != '\0' && parameters[i] != '\n'){
      if(parameters[i] != '\t' && parameters[i] != '\r' && parameters[i] != ' '){
          fprintf(stderr, "Error in line %d: Invalid string\n", line_number);
          return 0;
      }
      i++;
}
return 1;
}  

int check_directive_parameter(char *parameters, int line_number)
{
    char *p = parameters;
    int expect_number = 1;

    /* דילוג על רווחים בתחילת השורה */
    while (*p == ' ' || *p == '\t')
        p++;

    /* אין פרמטרים */
    if (*p == '\0') {
        fprintf(stderr, "Error in line %d: Missing parameter\n", line_number);
        return 0;
    }

    while (1) {

        if (expect_number) {

            /* פסיק לפני המספר הראשון או אחרי פסיק */
            if (*p == ',') {
                fprintf(stderr,
                        "Error in line %d: Comma before the first number\n",
                        line_number);
                return 0;
            }

            /* סימן אופציונלי */
            if (*p == '+' || *p == '-')
                p++;

            /* חייבת להיות לפחות ספרה אחת */
            if (!isdigit(*p)) {
                fprintf(stderr,
                        "Error in line %d: Illegal number\n",
                        line_number);
                return 0;
            }

            /* קריאת כל הספרות */
            while (isdigit(*p))
                p++;

            expect_number = 0;
        }

        /* רווחים אחרי מספר */
        while (*p == ' ' || *p == '\t')
            p++;

        /* סוף המחרוזת */
        if (*p == '\0')
            return 1;

        /* חייב להיות פסיק */
        if (*p != ',') {
            fprintf(stderr,
                    "Error in line %d: Missing comma between numbers\n",
                    line_number);
            return 0;
        }

        /* נמצא פסיק */
        p++;

        /* רווחים אחרי פסיק */
        while (*p == ' ' || *p == '\t')
            p++;

        /* פסיק אחרי הפסיק הראשון */
        if (*p == ',') {
            fprintf(stderr,
                    "Error in line %d: Multiple consecutive commas\n",
                    line_number);
            return 0;
        }

        /* פסיק בסוף */
        if (*p == '\0') {
            fprintf(stderr,
                    "Error in line %d: Comma after the last number\n",
                    line_number);
            return 0;
        }

        expect_number = 1;
    }
}
/*
int check_directive_parameter(char *parameters, int line_number){
int i = 0;
int expect_number = 1; 
int has_number = 0;
int length = strlen(parameters);
while(i < length){    
    if(parameters[i] == '\t' || parameters[i] == ' '){
      i++;
      continue;
    }
     if(parameters[i] == ','){
         if(expect_number == 1){
            if(has_number == 0)
              fprintf(stderr, "Error in line %d: Comma before the first number\n", line_number);
            else
                fprintf(stderr, "Error in line %d: Invalid Comma\n", line_number);
            return 0;
         }
         expect_number = 1;
         i++;
         continue;
    } end - if(parameters[i] == ',')
    if(expect_number == 1){
       if(parameters[i] == '+' || parameters[i] == '-')
          i++;
       if(i >= length || parameters[i] < '0' || parameters[i] > '9') {
          fprintf(stderr, "Error in line %d: Expected digits for number\n", line_number);
           return 0;
        }
        while(i < length && parameters[i] >= '0' && parameters[i] <= '9')
              i++;
        has_number = 1; 
        expect_number = 0;
        continue;
    }
    if((parameters[i] >= '0' && parameters[i] <= '9') || parameters[i] == '+' || parameters[i] == '-'){
        fprintf(stderr, "Error in line %d: Missing comma between numbers\n", line_number);
        return 0;
    }
    fprintf(stderr, "Error in line %d: Invalid character\n", line_number);
        return 0;
   
}
if(expect_number == 1){
  if(!has_number) 
      fprintf(stderr, "Error in line %d: No parameters provided\n", line_number);
  else
      fprintf(stderr, "Error in line %d: Comma after the last number\n", line_number);
  return 0;
}
return 1;
}*/
void enter_to_data_image(char *parameters, int size){
char *p = (char *)parameters;
char *end;
long num;
long part;
BYTE *temp;
while(*p != '\0'){
      while(*p == ' ' || *p == ','){
          p++;
      }
      if(*p == '\0') 
          break;
      num = strtol(p, &end, 10);
      while(DC + size >= data_capacity){
          data_capacity = (data_capacity == 0) ? 100 : data_capacity * 2;
          temp = (BYTE *) realloc(data_image, data_capacity * sizeof(BYTE));
          if(temp == NULL){
            fprintf(stderr, "Fatal Error: Memory allocation failed for data_image\n");
            exit(1);
          }
          data_image = temp;
      }
      switch(size){
          case 1:
                data_image[DC] = (BYTE) num;
                DC++;
                break;
          case 2:
                part = num & 0xFF;
                data_image[DC] = (BYTE) part;
                part = (num >> 8) & 0xFF;
                data_image[DC + 1] = (BYTE) part;
                DC += 2;
                break;
           case 4:
                part = num & 0xFF;
                data_image[DC] = (BYTE) part;
                part = (num >> 8) & 0xFF;
                data_image[DC + 1] = (BYTE) part;
                part = (num >> 16) & 0xFF;
                data_image[DC + 2] = (BYTE) part;
                part = (num >> 24) & 0xFF;
                data_image[DC + 3] = (BYTE) part;
                DC += 4;
                break;
      }
      p = end;
    } 
}
void enter_asciz_to_data_image(char *parameters){
int i = 0;
BYTE *temp;
while(DC + strlen(parameters) >= data_capacity){
      data_capacity = (data_capacity == 0) ? 100 : data_capacity * 2;
      temp = (BYTE *) realloc(data_image, data_capacity * sizeof(BYTE));
      if(temp == NULL){
         fprintf(stderr, "Fatal Error: Memory allocation failed for data_image\n");
         exit(1);
      }
      data_image = temp;
}
while(parameters[i] == '\t' || parameters[i] == ' ')
      i++;
if(parameters[i] == '"')
    i++;
while(parameters[i] != '\0' && parameters[i] != '"'){
    data_image[DC] = (BYTE) parameters[i];
    DC++;
    i++;
}
data_image[DC] = (BYTE) 0;
DC++;
} 
