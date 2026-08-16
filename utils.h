#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <stddef.h>
#include "global.h"



#define MIN_DB (-128)
#define MAX_DB (127)

#define MIN_DH (-32768)
#define MAX_DH (32767)

#define MIN_DW (-2147483648L)
#define MAX_DW (2147483647L)


int is_empty_or_comment(char *line);
int is_data_directive(char *word);
int is_valid_label(char *word, int line_number);
int get_opcode(char * name);


int get_opcode(char *name);
int get_funct(char *name);
int has_parameters(char *str);
int is_valid_label(char *word, int line_number);
int is_data_directive(char *command_name);
int is_empty_or_comment(char *line);
int check_and_enter_R_function_parameter(char *parameters, int opcode, int funct, int line_number);
int check_and_enter_I_function_parameter(char *parameters, int opcode, int line_number);
int check_end_enter_J_function_parameter(char *parameters, int opcode, int line_number);
int check_register(char *reg, int line_number);
int check_asciz_parameter(char *parameters, int line_number);
const char* skip_spaces(const char *ptr);
int check_directive_parameter(const char *line, DirectiveType type, int line_number);
void enter_to_data_image(char *parameters, int size);
void enter_asciz_to_data_image(char *parameters);
#endif
