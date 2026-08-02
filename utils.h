#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>
#include <string.h>
#include "global.h"
#include <stdio.h>
#include <stddef.h>


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
int check_directive_parameter(char *parameters, int line_number);
void enter_to_data_image(char *parameters, int size);
void enter_asciz_to_data_image(char *parameters);
#endif
