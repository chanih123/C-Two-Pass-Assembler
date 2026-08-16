#include <stdio.h>
#include <string.h>
#include "global.h"
#include "utils.h"
#include "symbol_table.h"

BYTE *code_image = NULL;
int code_capacity = 10;
BYTE *data_image = NULL;
int data_capacity = 100;
int IC = 100;
int DC = 0;
int ICF = 0;
int DCF = 0;
int symbol_capacity = 10;
int symbol_count = 0;

int run_first_pass(char *filename){

int opcode = 0;
int funct = 0;
int line_number = 0;
int bytes_read = 0;
int error_found = 0;


int there_is_label = 0;

char line[1024];
char word[1024];
char label_name[1024];
char command_name[1024];
char ext_label[32];
char *parameters;
char *next_part;

FILE *fp;

Symbol *symbol_table = init_symbol_table(symbol_capacity);
data_image = (BYTE *) malloc(data_capacity * sizeof(BYTE));
if (data_image == NULL) {
    fprintf(stderr, "Fatal Error: Memory allocation failed for Data Image\n");
    exit(1);
}
fp = fopen(filename, "r");
if(fp == NULL){
    fprintf(stderr, "Fatal Error: The file does not exist.\n");
    exit(1);
}
while(fgets(line, sizeof(line), fp) != NULL){
    /* Read a full line from standard input safely */
        line_number++;
        there_is_label = 0;
        word[0] = '\0';
        command_name[0] = '\0';
        label_name[0] = '\0';
        bytes_read = 0;
        if(is_empty_or_comment(line) == 0){
            if(sscanf(line, "%s %n", word, &bytes_read ) == 1) /* Extract the first word from the line */{
                if (word[strlen(word) - 1] == ':'){
                    if(is_valid_label(word, line_number) == 0){
                        error_found++;
                        continue;
                    }
                    there_is_label = 1;
                    strcpy(label_name, word);
                    label_name[strlen(label_name) -1] = '\0';
                    next_part = line + bytes_read;
                    if(sscanf(next_part, "%s %n", command_name, &bytes_read) != 1){
                        fprintf(stderr, "Error in line %d: There is no command name.\n", line_number);
                        error_found++;
                        continue;
                    }
                    parameters = next_part + bytes_read;
                    if((has_parameters(parameters) == 0) && (strcmp(command_name, "hlt") == 1)){
                        fprintf(stderr, "Error in line %d: There is no parameters\n", line_number);
                        error_found++;
                        continue;
                    }
                } /* end - (word[length - 1] == ':')*/
                else{
                    strcpy(command_name, word);
                    parameters = line + bytes_read;
                }
                if(is_data_directive(command_name)){
                    if(there_is_label == 1){
                        if(add_symbol(&symbol_table, &symbol_count, &symbol_capacity, label_name, DC, data, 0, line_number) != 1)
                          error_found++;
                    }
                       if(strcmp(command_name, ".asciz") == 0){
                          if(check_asciz_parameter(parameters, line_number))
                              enter_asciz_to_data_image(parameters);
                       }
                       else{
                          if(check_directive_parameter(parameters, line_number)){
                              if(strcmp(command_name, ".db") == 0)
                                  enter_to_data_image(parameters, 1);
                              if(strcmp(command_name, ".dh") == 0)
                                  enter_to_data_image(parameters, 2);
                              if(strcmp(command_name, ".dw") == 0)
                                  enter_to_data_image(parameters, 4);
                          } /* end if(check_directive_parameter*/ 
                        } /*end else */
                     continue;
                    } /* end - if(is_data_directive(next_part)) */
                if((strcmp(command_name, ".entry") == 0))
                    continue;
                if((strcmp(command_name, ".extern") == 0)){
                    if(parameters != NULL){
                       if(sscanf(parameters, "%s", ext_label) == 1){
                          if(add_symbol(&symbol_table, &symbol_count, &symbol_capacity, ext_label, 0, external, 0, line_number) == 0)
                              error_found++;
                       }
                    }
                continue;
                } /* end if((strcmp(command_name, ".extern")*/ 
                if(there_is_label == 1){
                    if(add_symbol(&symbol_table, &symbol_count, &symbol_capacity, label_name, IC, code, 0, line_number) == 0)
                      error_found++;
                }
                opcode = get_opcode(command_name);
                if(opcode == -1){
                    fprintf(stderr, "Error in line %d: Invalid command name\n", line_number);
                    error_found++;
                }
                else{
                    if(opcode == 0 || opcode == 1){
                        funct = get_funct(command_name);
                        if(check_and_enter_R_function_parameter(parameters, opcode, funct, line_number) == 0)
                          error_found++;
                    }
                    else{
                    if(opcode >= 10 && opcode <= 24){
                       if(check_and_enter_I_function_parameter(parameters, opcode, line_number) == 0)
                          error_found++;
                    }
                    else{
                       if(check_end_enter_J_function_parameter(parameters, opcode, line_number) == 0)
                       error_found++;
                    }
                    }
                }/*end else*/  
            } /* end if(sscanf(line, "%s %n", word, &bytes_read ) == 1) */
        }/* end - if(is_empty_or_comment(line) == 0)*/
  }/*end FOREVER*/
    fclose(fp);
    if(error_found > 0){
      fprintf(stderr, "%d errors found during first pass in file %s. Skipping second pass.\n", error_found, filename);
      return 0;
    } 
    DCF = DC;
    ICF = IC;
    update_data_symbol_table(symbol_table, symbol_count, ICF);
    return 1; 
}
      
        
