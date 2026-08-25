/**
 * @file first_pass.c
 * @brief Implementation of the assembler's first pass phase.
 *
 * This file contains the logic for the first pass of the assembly process.
 * It reads the expanded source file (.am) line by line, identifies label definitions,
 * updates the Instruction Counter (IC) and Data Counter (DC), builds the symbol table, 
 * and performs preliminary syntax validation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global.h"
#include "utils.h"
#include "symbol_table.h"
#include "first_pass.h"

/**
 * @brief Executes the first pass of the assembler on a given source file.
 *
 * This function parses each line of the assembly file, builds the symbol table, 
 * and encodes data directives. It counts instructions and data to calculate the 
 * final memory requirements (ICF and DCF).
 *
 * @param filename The path to the source assembly file (.am) to process.
 * @param symbol_table Pointer to the symbol table (Note: currently relies on global scope).
 * @param icf Pointer to store the final Instruction Counter (ICF) value.
 * @param dcf Pointer to store the final Data Counter (DCF) value.
 * @return 1 if the first pass completed successfully without errors, 0 otherwise.
 */
int run_first_pass(char *filename, Symbol **symbol_table, int *icf, int *dcf){

  int opcode = 0;
  int funct = 0;
  int line_number = 0;
  int bytes_read = 0;
  int error_found = 0;
  int there_is_label = 0;

  char line[MAX_LINE_LENGTH];
  char word[MAX_LINE_LENGTH];
  char label_name[MAX_LABEL_LENGTH];
  char command_name[MAX_LABEL_LENGTH];
  char ext_label[MAX_LABEL_LENGTH];
  char *parameters = NULL;
  char *next_part = NULL;

  FILE *fp = NULL;

  fp = fopen(filename, "r");
  if(fp == NULL){
      fprintf(stderr, "Fatal Error: The file does not exist.\n");
      return 0;
  }
  
  /* Reset memory counters for the new file */
  IC = IC_INIT_VALUE;
  DC = 0;
  
  /* Read and process the source file line by line */
  while(fgets(line, sizeof(line), fp) != NULL){
        line_number++;
        there_is_label = 0;
        word[0] = '\0';
        command_name[0] = '\0';
        label_name[0] = '\0';
        bytes_read = 0;
        
        /* Ignore empty lines and comments (lines starting with ';') */
        if(is_empty_or_comment(line))
            continue;
        
        /* Read the first word/token in the current line */
        if(sscanf(line, "%s %n", word, &bytes_read ) != 1) 
            continue;
            
        /* Check if the first word is a label declaration (ends with ':') */
        if(word[strlen(word) - 1] == ':'){
            word[strlen(word) -1] = '\0'; /* Strip the colon from the label name */
            
            /* Validate label naming conventions */
            if(!is_valid_label(word, line_number)){
                error_found++;
                continue;
            }
            
            there_is_label = 1;
            strcpy(label_name, word);
            
            /* Advance pointer to extract the command that follows the label */
            next_part = line + bytes_read;
            if(sscanf(next_part, "%s %n", command_name, &bytes_read) != 1){
                fprintf(stderr, "Error in line %d: There is no command name.\n", line_number);
                error_found++;
                continue;
            }
            /* Set parameters pointer to the rest of the line */
            parameters = next_part + bytes_read;
            
            /* Verify that commands (other than 'hlt') are provided with parameters */
            if((!has_parameters(parameters)) && (strcmp(command_name, "hlt") != 0)){
                fprintf(stderr, "Error in line %d: There is no parameters\n", line_number);
                error_found++;
                continue;
            }
        } /* end - (word[length - 1] == ':')*/
        else{
            /* If no label is present, the first word is assumed to be the command */
            strcpy(command_name, word);
            parameters = line + bytes_read;
        }
        
        /* 1. Handle data storage directives (.db, .dh, .dw, .asciz) */
        if(is_data_directive(command_name)){
            if(there_is_label == 1){
                /* Add the label to the symbol table with the data attribute */
                if(!add_symbol(label_name, DC, data, line_number))
                     error_found++;
            }
            
            /* Parse and store specific data types in the data image */
            if(strcmp(command_name, DIRECTIVE_ASCIZ_STR) == 0){
                if(check_asciz_parameter(parameters, line_number))
                    enter_asciz_to_data_image(parameters);
               else
                    error_found++;
            }
            else if(strcmp(command_name, DIRECTIVE_DB_STR) == 0){
                    if(check_directive_parameter(parameters, DIRECTIVE_DB, line_number))
                        enter_to_data_image(parameters, 1);
                    else
                        error_found++;
            }
            else if(strcmp(command_name, DIRECTIVE_DH_STR) == 0){
                     if(check_directive_parameter(parameters, DIRECTIVE_DH, line_number))
                          enter_to_data_image(parameters, 2);
                     else
                          error_found++;
            }
            else if(strcmp(command_name, DIRECTIVE_DW_STR) == 0){
                    if(check_directive_parameter(parameters, DIRECTIVE_DW, line_number))
                        enter_to_data_image(parameters, 4);
                    else
                        error_found++;
            }
            continue; /* Move to the next line after processing the data directive */
        } /* end - if(is_data_directive(next_part)) */
        
        /* 2. Handle entry directives (these are fully processed in the second pass) */
        if((strcmp(command_name, ".entry") == 0))
            continue;
            
        /* 3. Handle external symbol directives */
        if((strcmp(command_name, ".extern") == 0)){
            if(parameters != NULL && sscanf(parameters, "%s", ext_label) == 1){
                  /* Validate label naming conventions */
                  if(!is_valid_label(ext_label, line_number)){
                      error_found++;
                      continue;
                  }
                 /* Add external symbols to the table with address 0 */
                 if(!add_symbol(ext_label, 0, external, line_number))
                    error_found++;
            }
            continue;
        }
        
        /* 4. Handle machine instruction statements */
        if(there_is_label == 1){
            /* Add the instruction label to the symbol table with the code attribute */
            if(!add_symbol(label_name, IC, code, line_number))
                  error_found++;
        }
        
        /* Retrieve the opcode for the given command name */
        opcode = get_opcode(command_name);
        if(opcode == -1){
            fprintf(stderr, "Error in line %d: Invalid command name\n", line_number);
            error_found++;
        }
        else{
             /* Parse instruction arguments based on its type (R, I, or J) */
             if(opcode == OPCODE_R_MATH || opcode == OPCODE_R_COPY){
                 funct = get_funct(command_name);
                 if(!check_and_enter_R_function_parameter(parameters, opcode, funct, line_number))
                     error_found++;
             }
             else if(opcode >= MIN_I_OPCODE && opcode <= MAX_I_OPCODE){
                      if(!check_and_enter_I_function_parameter(parameters, opcode, line_number))
                          error_found++;
             }
             else if(check_end_enter_J_function_parameter(parameters, opcode, line_number) == 0)
                       error_found++;
          }
      }
  
  fclose(fp);
  
  /* Stop processing if errors were encountered during the first pass */
  if(error_found > 0){
      filename[strlen(filename) - 3] = '\0';
      fprintf(stderr, "%d errors found during first pass in file %s Skipping second pass\n", error_found, filename);
      return 0;
  } 
  
  /* Save final counter values and adjust data symbol addresses according to the final IC */
  *dcf = DC;
  *icf = IC;
  update_data_symbol_table(IC);
  
  return 1; 
}
