/**
 * @file utils.c
 * @brief Utility functions for the assembler.
 *
 * This file provides helper functions for instruction parsing, string 
 * manipulation, syntax validation, memory image encoding, and output generation.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include "utils.h"
#include "global.h"
#include "first_pass.h"
#include "symbol_table.h"

/**
 * Fixed lookup table containing all 27 supported machine instructions,
 * their operation types (R, I, J), funct values, and opcodes.
 */
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

/* Global pointer for the external references linked list */
static ExtNode *ext_head = NULL;

/**
 * @brief Looks up an instruction name in the instructions table.
 * 
 * @param name The name of the command to search for.
 * @return The opcode of the instruction if found, or -1 (CMD_NOT_FOUND) otherwise.
 */
int get_opcode(char *name)
{
  int i;

  if(name == NULL){
      return CMD_NOT_FOUND;
  }
  
  for(i = 0; i < NUM_OF_INSTRUCTIONS; i++){
      if(strcmp(name,instructions[i].name)==0)
          return instructions[i].opcode;
  }
  return CMD_NOT_FOUND;
}

/**
 * @brief Retrieves the funct code for a given instruction name.
 * 
 * @param name The name of the instruction.
 * @return The funct field value if found, or -1 (CMD_NOT_FOUND) if invalid.
 */
int get_funct(char *name)
{
  int i;
  
  if(name == NULL){
      return CMD_NOT_FOUND;
  }
    
  for(i = 0; i < NUM_OF_INSTRUCTIONS; i++){
      if(strcmp(name,instructions[i].name)==0)
          return instructions[i].funct;
  }
  return CMD_NOT_FOUND;
}

/**
 * @brief Checks if a string contains any non-whitespace characters (parameters).
 *
 * @param str The string to inspect.
 * @return 1 if at least one non-whitespace character is found, 0 otherwise.
 */
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

/**
 * @brief Checks whether a given string is a reserved language keyword
 * (instruction name, directive, macro keyword, or register).
 *
 * @param name The string to check.
 * @return 1 if reserved, 0 otherwise.
 */
 int is_reserved_keyword(char *name){
    int reg_num = 0;
    /* Check instruction names (add, sub, jmp, etc.) */
    if(get_opcode(name) != CMD_NOT_FOUND)
        return 1;

    /* Check directive names */
    if(strcmp(name, "db") == 0 || strcmp(name, "dh") == 0 || strcmp(name, "dw") == 0 || strcmp(name, "asciz") == 0 || strcmp(name, "entry") == 0 || strcmp(name, "extern") == 0)
        return 1;

    /* Check macro keywords */
    if(strcmp(name, "mcro") == 0 || strcmp(name, "mcroend") == 0)
        return 1;

    /* Check register names ($0 to $31) */
    if(name[0] == '$' && isdigit((unsigned char)name[1])){
        reg_num = atoi(name + 1);
        if(reg_num >= 0 && reg_num <= 31)
            return 1;
    }

    return 0;
}

/**
 * @brief Validates a label name according to language syntax rules:
 * 1. Must not be NULL or empty.
 * 2. Length must not exceed MAX_LABEL_LENGTH.
 * 3. Must start with an alphabetic letter.
 * 4. Must contain only alphanumeric characters.
 * 5. Must not be a reserved keyword.
 *
 * @param name The label string to validate (without trailing colon).
 * @param line_number Current line number for error reporting.
 * @return 1 if valid, 0 otherwise.
 */
int is_valid_label(char *name, int line_number){

    int length = strlen(name);
    int i;
    
    if(name == NULL || length == 0){
        fprintf(stderr, "Error in line %d: Label name cannot be empty.\n", line_number);
        return 0;
    }
    if(length > MAX_LABEL_LENGTH){
        fprintf(stderr, "Error in line %d: The label name is too long.\n", line_number);
        return 0;
    }
    /* First character must be a letter */
    if(!isalpha((unsigned char)name[0])){
        fprintf(stderr, "Error in line %d: The label name must start with an alphabetic letter.\n", line_number);
        return 0;
    }
    /* All subsequent characters must be alphanumeric */
    for(i = 1; i < length; i++){
        if(!isalnum((unsigned char)name[i])){
            fprintf(stderr, "Error in line %d: Invalid label name\n", line_number);
             return 0;
        }
    } 
    /* Label cannot be a reserved word */
    if(is_reserved_keyword(name)){
        fprintf(stderr, "Error in line %d: The label name is a reserved keyword.\n", line_number);
        return 0;
    }
    return 1;
}

/**
 * @brief Checks if a given command is a data allocation directive (.dh, .dw, .db, .asciz).
 *
 * @param command_name The name of the command to check.
 * @return 1 if it is a data directive, 0 otherwise.
 */
int is_data_directive(char *command_name){
  if(command_name == NULL || command_name[0] != '.')
        return 0;
  if((strcmp(command_name, DIRECTIVE_DH_STR) == 0) || (strcmp(command_name, DIRECTIVE_DW_STR) == 0) || (strcmp(command_name, DIRECTIVE_DB_STR) == 0) || (strcmp(command_name, DIRECTIVE_ASCIZ_STR) == 0))
      return 1;
      
  return 0;
}

/**
 * @brief Checks if a given line is empty (contains only whitespace) or is a comment line.
 *
 * @param line The string line to inspect.
 * @return 1 if the line is empty or a comment, 0 otherwise.
 */
int is_empty_or_comment(char *line){
  int i = 0;
  
  /* Safe guard against NULL pointers */
  if(line == NULL)
      return 1;
  
  /* Skip leading whitespace characters (spaces, tabs) */
  while(line[i] != '\0' && (line[i] == '\t' || line[i] == ' ')){
      i++;
  }
  
  /* Check if line is empty, ended, or begins with a comment symbol */
  if(line[i] == '\0' || line[i] == '\n' || line[i] == '\r' || line[i] == ';'){
    return 1;
  }
  return 0;
}

/**
 * @brief Inserts a 32-bit instruction word into the code image using Little-Endian order.
 * 
 * Automatically expands the code_image memory buffer if needed.
 *
 * @param word The 32-bit encoded instruction word.
 */
 void insert_to_code_image(unsigned int word) {
    int i;
    BYTE *temp = NULL;
    /* Reallocate code image buffer if capacity is reached */
    while(IC + MEMORY_WORD_SIZE >= code_capacity){
        code_capacity = (code_capacity == 0) ? INITIAL_CAPACITY : code_capacity * 2;
        temp = (BYTE *) realloc(code_image, code_capacity * sizeof(BYTE));
        if(temp == NULL){
        fprintf(stderr, "Fatal Error: Memory allocation failed for code_capacity\n");
        exit(1);
        }
    code_image = temp;
    }
    /* Store 32 bits as 4 bytes in Little-Endian byte order */
    for(i = 0; i < MEMORY_WORD_SIZE; i++){
        code_image[IC] = (word >> (i * 8)) & BYTE_MASK;
        IC++;
    }
}

/**
 * @brief Extracts the next token from the source string.
 *
 * Skips leading whitespaces, copies characters up to the next whitespace,
 * comma, or null terminator into the destination buffer, and ensures null-termination.
 *
 */
char *extract_token(char *src, char *dest, int max_len){
    int i = 0;
    src = skip_spaces(src);
    if(!src || *src == '\0'){
        dest[0] = '\0';
        return src;
    }
    while(*src && !isspace((unsigned char)*src) && *src != ','){
        if(i < max_len - 1)
          dest[i++] = *src;
        src++;
    }
    dest[i] = '\0';
    return src;
}

/**
 * @brief Validates and consumes a comma separator between operands.
 *
 * Skips whitespaces, ensures that exactly one comma exists, and validates
 * that no consecutive commas follow. Reports an error if syntax is invalid.
 *
 */
char *match_comma(char *p, int line_number){
    p = skip_spaces(p);
    if(p == NULL || *p == '\0'){
      fprintf(stderr, "Error in line %d: Missing comma between operands\n", line_number);
      return NULL;
    }
    if(*p != ','){
       fprintf(stderr, "Error in line %d: Missing comma separator\n", line_number);
      return NULL;
    }
    p++; 
    p = skip_spaces(p);
    if(p != NULL && *p == ','){
      fprintf(stderr, "Error in line %d: Multiple consecutive commas\n", line_number);
      return NULL;
    }
    return p;
}

/**
 * @brief Validates operands and encodes an R-type machine instruction into the code image.
 *
 * @param parameters String containing the register operands (e.g. "$3, $5, $9").
 * @param opcode The opcode of the instruction (0 for arithmetic/logic, 1 for copy/move).
 * @param funct The funct field value.
 * @param line_number The line number in the source file for error messaging.
 * @return 1 if valid and successfully encoded, 0 if errors were found.
 */
int check_and_enter_R_function_parameter(char *parameters, int opcode, int funct, int line_number){
  char reg[MAX_LINE_LENGTH];
  char *p = NULL;
  int reg_num = 0;
  unsigned int encoded = 0;
  int count = 0;
  int expected_count = 0;
  
  expected_count = (opcode == OPCODE_R_MATH) ? R_ARITHMETIC_REG_COUNT : R_COPY_REG_COUNT;
  
  /* Encode fixed opcode and funct fields */
  encoded = encoded | (opcode & OPCODE_MASK) << OPCODE_SHIFT;
  encoded = encoded | (funct & FUNCT_MASK) << FUNCT_SHIFT;
  
  p = skip_spaces(parameters);
  
  /*Check for an unnecessary comma at the beginning*/
  if(*p == ','){
      fprintf(stderr, "Error in line %d: Extraneous comma at beginning of parameters\n", line_number);
      return 0;
  }
  /* Parse each register operand */
  while(count < expected_count){
      p = extract_token(p, reg, sizeof(reg));
      if(reg[0] == '\0'){
          fprintf(stderr, "Error in line %d: Missing register operand\n", line_number);
          return 0;
      }
      
      reg_num = check_register(reg, line_number) ;
      if(reg_num == -1)
          return 0;
          
      if(opcode == OPCODE_R_MATH){
      /* Arithmetic/logical operations: 3 registers (rs, rt, rd) */
      switch(count){
        case 0:
            encoded  = encoded | (reg_num & REG_MASK) << RS_SHIFT;
            break;
        case 1:
            encoded  = encoded | (reg_num & REG_MASK) << RT_SHIFT;
            break;
        case 2:
            encoded  = encoded | (reg_num & REG_MASK) << RD_SHIFT;
            break;
      }
    }
    else{
    /* Copy/move operations (move, mvhi, mvlo): 2 registers (rs, rd), rt is 0 */
      switch(count){
        case 0:
            encoded  = encoded | (reg_num & REG_MASK) << RS_SHIFT;
            break;
        case 1:
            encoded  = encoded | (reg_num & REG_MASK) << RD_SHIFT;
            break;
      }
    }   
    count++;
    if(count < expected_count){
         if((p = match_comma(p, line_number)) == NULL)
             return 0;
    }
    }
    p = skip_spaces(p);
    /*Check  comma*/
    if(*p != '\0'){
        if(*p == ','){
            fprintf(stderr, "Error in line %d: Extraneous trailing comma\n", line_number);
        }
        else
            fprintf(stderr, "Error in line %d: Extraneous text after instruction: '%s'\n", line_number, p);
        return 0;
  }
  insert_to_code_image(encoded);
  
  return 1;
}

/**
 * @brief Validates operands and encodes an I-type machine instruction into the code image.
 * 
 * Handles both immediate arithmetic/memory operations and conditional branch instructions.
 *
 * @param parameters String containing operands (e.g. "$1, -50, $2" or "$1, $2, LABEL").
 * @param opcode The opcode of the I-type instruction.
 * @param line_number Current line number in source file for error reporting.
 * @return 1 if valid and successfully processed, 0 on syntax/operand error.
 */
int check_and_enter_I_function_parameter(char *parameters, int opcode, int line_number){
  char token[MAX_LINE_LENGTH];
  char *endptr = NULL;
  char *p = NULL;
  long immed = 0;
  int reg_num = 0;
  unsigned int encoded = 0;

  /* Encode fixed opcode field */
  encoded = encoded | (opcode & OPCODE_MASK) << OPCODE_SHIFT;
  
  p = skip_spaces(parameters);
  
  /* Checks for unnecessary commas */
  if(*p == ','){
      fprintf(stderr, "Error in line %d: Extraneous comma at beginning of parameters\n", line_number);
      return 0;
  }
  /* 1. Parse first operand: source register (rs) */
  p = extract_token(p, token, sizeof(token));
  if(token[0] == '\0'){
      fprintf(stderr, "Error in line %d: Missing first register operand\n", line_number);
      return 0;
  }
  
  reg_num = check_register(token, line_number);
  if(reg_num == -1)
      return 0;
  encoded  = encoded | (reg_num & REG_MASK) << RS_SHIFT;
  
  /* Comma after rs */
  if((p = match_comma(p, line_number)) == NULL) 
      return 0;
  
  if((opcode >= MIN_I_MATH_OPCODE && opcode <= MAX_I_MATH_OPCODE) || (opcode >= MIN_I_LOAD_STORE_OPCODE && opcode <= MAX_I_LOAD_STORE_OPCODE)){
      /* 2. Parse second operand: 16-bit immediate value */
      p = extract_token(p, token, sizeof(token));
      if(token[0] == '\0'){
          fprintf(stderr, "Error in line %d: Missing immediate numeric value\n", line_number);
          return 0;
      }
      
      immed = strtol(token, &endptr, 10);
      if(*endptr != '\0' || endptr == token){
          fprintf(stderr, "Error in line %d: Invalid immediate\n", line_number);
          return 0;
      }
      if(immed > MAX_IMMED_VAL || immed < MIN_IMMED_VAL){
          fprintf(stderr, "Error in line %d: IValue exceeds 16-bit immediate range\n", line_number);
          return 0;
      }
      
      /* Comma after immed */
      if((p = match_comma(p, line_number)) == NULL) 
          return 0;
      
      /* 3. Parse third operand: target register (rt) */
      p = extract_token(p, token, sizeof(token));
      if(token[0] == '\0'){
          fprintf(stderr, "Error in line %d: Missing target register operand\n", line_number);
          return 0;
      }
      reg_num = check_register(token, line_number) ;
      if(reg_num == -1)
          return 0;
        
      encoded  = encoded | (reg_num & REG_MASK) << RT_SHIFT;
      encoded  = encoded | (immed & IMMED_16BIT_MASK);
  }
  else{
    if(opcode >= MIN_I_BRANCH_OPCODE && opcode <= MAX_I_BRANCH_OPCODE){
        /* Branch instructions (beq, bne, blt, bgt): format is (rs, rt, label) */

        /* 2. Parse second operand: second register (rt) */
        p = extract_token(p, token, sizeof(token));
        if(token[0] == '\0'){
            fprintf(stderr, "Error in line %d: Missing second register operand for branch\n", line_number);
            return 0;
        }
        reg_num = check_register(token, line_number);
        if(reg_num == -1) 
            return 0;
        encoded = encoded | (reg_num & REG_MASK) << RT_SHIFT;
        
        /* Comma after rt */
        if((p = match_comma(p, line_number)) == NULL) 
           return 0;
           
        /* Parse third operand: label name (verified now, address resolved in second pass) */  
        p = extract_token(p, token, sizeof(token));
        if(token[0] == '\0'){
            fprintf(stderr, "Error in line %d: Missing target label for branch\n", line_number);
            return 0;
        }
        if(!is_valid_label(token, line_number))
            return 0;
        /* Branch label address offset will be calculated and encoded in second pass */
        }
      }
      /* 4. Check for extraneous parameters */
      p = skip_spaces(p);
      if(*p != '\0'){
        if(*p == ','){
            fprintf(stderr, "Error in line %d: Extraneous trailing comma\n", line_number);
        } 
        else{
            fprintf(stderr, "Error in line %d: Extraneous text after instruction operands: '%s'\n", line_number, p);
        }
        return 0;
      }
      /* Store encoded instruction word in code image */
      insert_to_code_image(encoded);
      
      return 1;
}

/**
 * @brief Validates operands and encodes a J-type machine instruction into the code image.
 * 
 * Handles jump/call instructions (jmp, la, call) and the stop instruction (hlt).
 *
 * @param parameters String containing the operand (register or label), or empty/NULL for hlt.
 * @param opcode The opcode of the J-type instruction.
 * @param line_number Current line number in source file for error reporting.
 * @return 1 if valid and successfully processed, 0 on operand/syntax error.
 */
int check_end_enter_J_function_parameter(char *parameters, int opcode, int line_number){
  char *token = NULL;
  char copy[MAX_LINE_LENGTH];
  int reg_num;
  unsigned int encoded = 0;

  /* Encode fixed opcode field (bits 26-31) */
  encoded = encoded | (opcode & OPCODE_MASK) << OPCODE_SHIFT;

  strcpy(copy, parameters);
  token = strtok(copy, ", \t\n");
  /* 1. Handle 'hlt' instruction (expects no parameters) */
  if(opcode == OPCODE_HLT){
      if(has_parameters(parameters) == 1){
          fprintf(stderr, "Error in line %d: Superfluous parameters\n", line_number);
          return 0;
      }
    insert_to_code_image(encoded);
    return 1;
  }
  if(token == NULL){
      fprintf(stderr, "Error in line %d: Missing parameter for J instruction\n", line_number);
      return 0;
  }  
  /* 3. Validate 'la' and 'call' (only accept a label target) */
  if(opcode == OPCODE_LA || opcode == OPCODE_CALL){
      if(is_valid_label(token, line_number) == 0)
          return 0;
      /* Target label address will be resolved and filled during second pass */
  }
  /* 4. Validate 'jmp' (accepts either a register with $ or a label) */
  if(opcode == OPCODE_JMP){
      if(token[0] == '$'){
          reg_num = check_register(token, line_number);
          if(reg_num == -1)
              return 0;
          /*Turn on the reg bit (bit 25) and encode register number in address field */
        encoded = encoded | (1U << J_REG_FLAG_SHIFT);
        encoded  = encoded | (reg_num & J_ADDRESS_MASK);
    }
    else{
        if(is_valid_label(token, line_number) == 0)
            return 0;
        /*Target label address will be resolved during second pass (bit 25 remains 0)*/
    }
  }
  /* 5. Check for extraneous parameters */
  token = strtok(NULL, ", \t\n");
  if(token != NULL){
      fprintf(stderr, "Error in line %d: Extraneous text after instruction parameter\n", line_number);
      return 0;
  }
 
  /* Store encoded instruction word into code image */
  insert_to_code_image(encoded);
  
  return 1;
}

/**
 * @brief Validates a register operand string (must start with '$' followed by an integer 0-31).
 *
 * @param reg The register string (e.g. "$0", "$31").
 * @param line_number Current line number in source file for error reporting.
 * @return The integer register number (0 to 31) if valid, or -1 (INVALID_REGISTER) on error.
 */
int check_register(char *reg, int line_number){
  char *end = NULL;
  long num = 0;
  if(reg == NULL || strlen(reg) == 0 || reg[0] != REGISTER_PREFIX){
      fprintf(stderr, "Error in line %d: Invalid Register name\n", line_number);
      return INVALID_REGISTER;
  }
  
  num = strtol(reg + 1, &end, 10);
  if(*end != '\0' || end == reg + 1){
      fprintf(stderr, "Error in line %d: Invalid Register name\n", line_number);
      return INVALID_REGISTER;
  }
  if(num < MIN_REGISTER_NUM || num > MAX_REGISTER_NUM){
    fprintf(stderr, "Error in line %d: Invalid Register name\n", line_number);
    return INVALID_REGISTER;;
  }
  return (int)num; 
}

/**
 * @brief Validates the operand syntax of an .asciz directive.
 * 
 * Ensures the string is enclosed in double quotes and has no trailing non-whitespace characters.
 *
 * @param parameters The operand string to validate (e.g. " \"Hello World\" ").
 * @param line_number Current line number in source file for error reporting.
 * @return 1 if valid string syntax, 0 otherwise.
 */
int check_asciz_parameter(char *parameters, int line_number){
  int i = 0;
  
  /* 1. Skip leading whitespace */
  while(parameters[i] == '\t' || parameters[i] == ' '){
      i++;
  }
  /* 2. Verify opening quote */
  if(parameters[i] == STRING_QUOTE)
      i++;
  else{
      fprintf(stderr, "Error in line %d: Invalid string\n", line_number);
      return 0;
  }
  
  /* 3. Scan characters until closing quote or line end */
  while(parameters[i] != STRING_QUOTE && parameters[i] != '\0' && parameters[i] != '\n'){
      i++;
  }
  
  /* 4. Verify closing quote was reached */
  if(parameters[i] != STRING_QUOTE){
      fprintf(stderr, "Error in line %d: Invalid string\n", line_number);
      return 0;
  }
  i++; /* Move past the closing quote */
  
  /* 5. Ensure no extraneous characters remain after closing quote */
  while(parameters[i] != '\0' && parameters[i] != '\n'){
      if(parameters[i] != '\t' && parameters[i] != '\r' && parameters[i] != ' '){
          fprintf(stderr, "Error in line %d: Invalid string\n", line_number);
          return 0;
      }
      i++;
  } 
  return 1;
}  

/**
 * @brief Advances the pointer past any leading whitespace characters.
 *
 * @param ptr Pointer to the start of a string.
 * @return Pointer to the first non-whitespace character, or NULL if ptr is NULL.
 */
char* skip_spaces(char *ptr){
    if(ptr == NULL)
        return NULL;

    while (*ptr != '\0' && isspace((unsigned char)*ptr)){
        ptr++;
    }
    return ptr;
}

/**
 * @brief Validates the comma-separated numeric parameters of a data directive (.db, .dh, .dw).
 * 
 * Checks for syntax errors (consecutive, leading, or trailing commas) and verifies
 * that values fit within their respective bit-width signed limits.
 *
 * @param line The string containing comma-separated numeric arguments.
 * @param type The type of data directive (DIRECTIVE_DB, DIRECTIVE_DH, DIRECTIVE_DW).
 * @param line_number Current line number in source file for error reporting.
 * @return 1 if all arguments are valid, 0 on syntax or range error.
 */
int check_directive_parameter(char *line, DirectiveType type, int line_number){
  char *ptr;
  char *endptr = NULL;
  long value = 0; 
  int expecting_number = 1;

  if(line == NULL){
     fprintf(stderr, "Error in line %d: No parameters\n", line_number);
      return 0;
  }
  ptr = skip_spaces(line);
  
  /* Check for empty argument list */
  if(*ptr == '\0'){
      fprintf(stderr, "Error in line %d: No parameters\n", line_number);
      return 0;
  }
  
  /* Check for illegal leading comma */
  if(*ptr == ','){
      fprintf(stderr, "Error in line %d: Illegal comma before the first number\n", line_number);
      return 0;
  }
  while(*ptr != '\0'){
      if(expecting_number){
          errno = 0;
          value = strtol(ptr, &endptr, 10);
          
          /* Check if no digits could be parsed */
          if(ptr == endptr){
              fprintf(stderr, "Error in line %d: Expected an integer but found '%c'\n", line_number, *ptr);
              return 0;
          }
          
          /* Validate numeric boundaries based on directive bit-width */
          if(type == DIRECTIVE_DB){
              if(value < MIN_DB || value > MAX_DB){
                  fprintf(stderr, "Error in line %d: Value %ld exceeds 8-bit range\n", line_number, value);
                  return 0;
              }
          } 
          else if(type == DIRECTIVE_DH){
                  if(value < MIN_DH || value > MAX_DH){
                     fprintf(stderr, "Error in line %d: Value %ld exceeds 16-bit range\n", line_number, value);
                    return 0;
                  }
          } 
          else if(type == DIRECTIVE_DW){
                 if(value < MIN_DW || value > MAX_DW || errno == ERANGE){
                     fprintf(stderr, "Error in line %d: Value %ld exceeds 32-bit range\n", line_number, value);
                     return 0;
                 }
          }
          ptr = skip_spaces(endptr);
          expecting_number = 0;
        }
        else{
            /* Expecting a separating comma */
            if(*ptr == ','){
              ptr++;
              ptr = skip_spaces(ptr);
              if(*ptr == ','){
                 fprintf(stderr, "Error in line %d: Multiple consecutive commas\n", line_number);
                  return 0;
              }
              if (*ptr == '\0') {
                  fprintf(stderr, "Error in line %d: Illegal comma at end of line\n", line_number);
                  return 0;
              }
              expecting_number = 1;
            } 
            else{
              fprintf(stderr, "Error in line %d: Missing comma between numbers or invalid character '%c'\n", line_number, *ptr);
              return 0;
            }
        }
    }

    return 1;
}

/**
 * @brief Parses numeric parameters and encodes them into the data image array
 * using Little-Endian byte order. Supports 1, 2, or 4 byte values (.db, .dh, .dw).
 *
 * @param parameters String containing comma-separated numbers.
 * @param size Number of bytes allocated per number (1 for .db, 2 for .dh, 4 for .dw).
 */
void enter_to_data_image(char *parameters, int size){
  char *p = parameters;
  char *end = NULL;
  long num = 0;
  int i;
  BYTE *temp = NULL;
  
  while(*p != '\0'){
      /* Skip whitespace and commas between numbers */
      while(*p != '\0' && (isspace((unsigned char)*p) || *p == ',')){
          p++;
      }
      
      if(*p == '\0') 
          break;
          
      num = strtol(p, &end, 10);
      if(p == end)
          break;
      
      /* Ensure sufficient capacity in data_image buffer */
      while(DC + size >= data_capacity){
          data_capacity = (data_capacity == 0) ? INITIAL_CAPACITY : data_capacity * 2;
          temp = (BYTE *) realloc(data_image, data_capacity * sizeof(BYTE));
          if(temp == NULL){
            fprintf(stderr, "Fatal Error: Memory allocation failed for data_image\n");
            exit(1);
          }
          data_image = temp;
      }
      /* Store the number into data_image in Little-Endian format */
      for(i = 0; i < size; i++){
          data_image[DC] = (BYTE)((num >> (i * BITS_IN_BYTE)) & BYTE_MASK);
          DC++;
      }
        
      p = end;
    } 
}

/**
 * @brief Parses a string from an .asciz directive and stores each character 
 * (plus a terminating null byte) sequentially into the data image.
 *
 * @param parameters String containing the quoted text argument (e.g. " \"abc\" ").
 */
void enter_asciz_to_data_image(char *parameters){

  int i = 0;
  BYTE *temp = NULL;
  
  /* Ensure sufficient memory capacity in data_image */
  while(DC + strlen(parameters) + 1 >= data_capacity){
      data_capacity = (data_capacity == 0) ? INITIAL_CAPACITY : data_capacity * 2;
      temp = (BYTE *) realloc(data_image, data_capacity * sizeof(BYTE));
      if(temp == NULL){
         fprintf(stderr, "Fatal Error: Memory allocation failed for data_image\n");
         exit(1);
      }
      data_image = temp;
  }
  
  /* Skip leading whitespace */
  while(parameters[i] == '\t' || parameters[i] == ' ')
      i++;
      
  /* Skip the opening quote */
  if(parameters[i] == STRING_QUOTE)
     i++;
     
  /* Copy characters into data image until closing quote or string end */
  while(parameters[i] != '\0' && parameters[i] != STRING_QUOTE){
    data_image[DC] = (BYTE) parameters[i];
    DC++;
    i++;
  }
  
  /* Append null terminator '\0' */
  data_image[DC] = (BYTE) 0;
  DC++;
} 

/**
 * @brief Adds a new external symbol reference to the tracking list.
 *
 * @param name The name of the external symbol.
 * @param address The IC address where the symbol was referenced.
 */
static void add_ext_reference(char *name, int address) {
    ExtNode *new_node = (ExtNode *)malloc(sizeof(ExtNode));
    ExtNode *curr;
    if (new_node == NULL) {
        fprintf(stderr, "Fatal Error: Memory allocation failed for external reference.\n");
        exit(1);
    }
    strcpy(new_node->name, name);
    new_node->address = address;
    new_node->next = NULL;

    if (ext_head == NULL) {
        ext_head = new_node;
    } else {
        curr = ext_head;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = new_node;
    }
}

/**
 * @brief Frees the memory allocated for the external references list.
 */
void free_ext_references(void) {
    ExtNode *curr = ext_head;
    ExtNode *next;
    while (curr != NULL) {
        next = curr->next;
        free(curr);
        curr = next;
    }
    ext_head = NULL;
}

/**
 * @brief Completes the encoding of an I-type branch instruction.
 * 
 * Calculates the relative distance to the target label and encodes 
 * it into the lower 16 bits of the instruction[cite: 1].
 *
 * @param parameters The instruction parameters string.
 * @param ic The address of the instruction.
 * @param line_number The line number for error reporting.
 * @return 1 on success, 0 on error.
 */
int complete_I_branch_instruction(char *parameters, int ic, int line_number) {
    char copy[MAX_LINE_LENGTH];
    char *token;
    int index;
    long distance;
    Symbol *sym;

    strcpy(copy, parameters);
    
    /* Skip the first two register operands */
    token = strtok(copy, ", \t\n"); 
    token = strtok(NULL, ", \t\n"); 
    
    /* Extract the target label */
    token = strtok(NULL, ", \t\n"); 
    if (token == NULL) {
        return 0; /* Should have been caught in pass 1 */
    }

    index = find_symbol_index(token);
    if (index == -1) {
        fprintf(stderr, "Error in line %d: Label '%s' is not defined.\n", line_number, token);
        return 0;
    }

    sym = get_symbol_by_index(index);
    if (sym->type == external) {
        fprintf(stderr, "Error in line %d: Branch target '%s' cannot be an external symbol.\n", line_number, token);
        return 0;
    }

    /* Calculate distance: (target address) - (current instruction address) */
    distance = sym->value - ic;
    if (distance < MIN_IMMED_VAL || distance > MAX_IMMED_VAL) {
        fprintf(stderr, "Error in line %d: Branch distance out of 16-bit range.\n", line_number);
        return 0;
    }

    /* Encode the 16-bit distance into bytes 0 and 1 (Little-Endian) */
    code_image[ic] |= (BYTE)(distance & BYTE_MASK);
    code_image[ic + 1] |= (BYTE)((distance >> BITS_IN_BYTE) & BYTE_MASK);

    return 1;
}

/**
 * @brief Completes the encoding of a J-type instruction.
 * 
 * Resolves the address of the target label and encodes it into the 
 * lower 25 bits. Tracks external references if used.
 *
 * @param parameters The instruction parameters string.
 * @param ic The address of the instruction.
 * @param line_number The line number for error reporting.
 * @return 1 on success, 0 on error.
 */
int complete_J_instruction(char *parameters, int ic, int line_number) {
    char copy[MAX_LINE_LENGTH];
    char *token;
    int index;
    unsigned long addr;
    Symbol *sym;

    strcpy(copy, parameters);
    token = strtok(copy, ", \t\n");

    /* If no token, it's a 'hlt' instruction which requires no address */
    if (token == NULL) {
        return 1; 
    }

    /* If the target is a register (for jmp), it was fully encoded in Pass 1 */
    if (token[0] == REGISTER_PREFIX) {
        return 1; 
    }

    index = find_symbol_index(token);
    if (index == -1) {
        fprintf(stderr, "Error in line %d: Label '%s' is not defined.\n", line_number, token);
        return 0;
    }

    sym = get_symbol_by_index(index);

    /* If the symbol is external, track it and leave the address field as 0 */
    if (sym->type == external) {
        add_ext_reference(sym->name, ic);
        return 1;
    }

    /* Encode the 25-bit address into bytes 0 to 3 (Little-Endian) */
    addr = sym->value & J_ADDRESS_MASK;
    code_image[ic] |= (BYTE)(addr & BYTE_MASK);
    code_image[ic + 1] |= (BYTE)((addr >> BITS_IN_BYTE) & BYTE_MASK);
    code_image[ic + 2] |= (BYTE)((addr >> (2 * BITS_IN_BYTE)) & BYTE_MASK);
    code_image[ic + 3] |= (BYTE)((addr >> (3 * BITS_IN_BYTE)) & BYTE_MASK);

    return 1;
}

/**
 * @brief Generates the final object, entry, and external output files.
 *
 * @param ob_filename Name of the output object file.
 * @param ext_filename Name of the output external file.
 * @param ent_filename Name of the output entry file.
 * @param icf Final instruction counter value.
 * @param dcf Final data counter value.
 * @return 1 on success, 0 on error.
 */
int write_output_files(char *ob_filename, char *ext_filename, char *ent_filename, int icf, int dcf) {
    FILE *ob_file = NULL;
    FILE *ext_file = NULL;
    FILE *ent_file = NULL;
    int i, j;
    int has_entry = 0;
    int data_addr;
    int data_idx;
    Symbol *sym;
    ExtNode *ext_curr;

    /* 1. Create .ob file */
    ob_file = fopen(ob_filename, "w");
    if (ob_file == NULL) {
        return 0;
    }
    
    fprintf(ob_file, "\t%d %d\n", icf - IC_INIT_VALUE, dcf);

    /* Write Instruction Image (4 bytes per line) */
    for (i = IC_INIT_VALUE; i < icf; i += MEMORY_WORD_SIZE) {
        fprintf(ob_file, "%04d %02X %02X %02X %02X\n", i, 
                code_image[i], code_image[i+1], code_image[i+2], code_image[i+3]);
    }

    /* Write Data Image (4 bytes per line) */
    data_addr = icf;
    data_idx = 0;
    while (data_idx < dcf) {
        fprintf(ob_file, "%04d", data_addr);
        for (j = 0; j < MEMORY_WORD_SIZE && data_idx < dcf; j++) {
            fprintf(ob_file, " %02X", data_image[data_idx]);
            data_idx++;
            data_addr++;
        }
        fprintf(ob_file, "\n");
    }
    fclose(ob_file);

    /* 2. Create .ent file (Only if entries exist) */
    for (i = 0; i < get_symbol_count(); i++) {
        sym = get_symbol_by_index(i);
        if (sym->is_entry) {
            if (has_entry == 0) {
                ent_file = fopen(ent_filename, "w");
                if (ent_file == NULL) return 0;
                has_entry = 1;
            }
            fprintf(ent_file, "%s %04d\n", sym->name, sym->value);
        }
    }
    if (has_entry == 1) {
        fclose(ent_file);
    }

    /* 3. Create .ext file (Only if external references exist) */
    if (ext_head != NULL) {
        ext_file = fopen(ext_filename, "w");
        if (ext_file == NULL) return 0;
        
        ext_curr = ext_head;
        while (ext_curr != NULL) {
            fprintf(ext_file, "%s %04d\n", ext_curr->name, ext_curr->address);
            ext_curr = ext_curr->next;
        }
        fclose(ext_file);
    }

    /* Clean up external references list for the next file */
    free_ext_references();
    return 1;
}
