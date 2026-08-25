/**
 * @file main.c
 * @brief The main entry point for the assembler program.
 *
 * This program translates assembly language source files (.as) into 
 * machine code. It processes each file through a pre-assembler (macro 
 * expansion), a first pass (symbol table generation and data image 
 * building), and a second pass (instruction encoding and output generation).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "preprocessor.h"
#include "global.h"
#include "first_pass.h"
#include "second_pass.h"

#define IC_INIT_VALUE 100

/**
 * @brief The main function of the assembler.
 * 
 * Iterates over all file names provided as command-line arguments, 
 * appending the necessary extensions, and passes them through the 
 * assembly pipeline.
 * 
 * @param argc The number of command-line arguments.
 * @param argv An array of command-line argument strings (input file names).
 * @return 1 on successful execution, 0 on fatal failure.
 */
int main(int argc, char *argv[]) 
{
    int i;
    FILE *input_file;
    FILE *file_am;
    char *input_filename;
    char base_name[MAX_LINE_LENGTH];
    char am_filename[MAX_LINE_LENGTH + 10];
    char ob_filename[MAX_LINE_LENGTH + 10];
    char ext_filename[MAX_LINE_LENGTH + 10];
    char ent_filename[MAX_LINE_LENGTH + 10];
    Macro *macro_head;
    int len_base, len_filename;   
    int ICF, DCF;

    /* Check that command line arguments were passed (file names without extensions) */
    if(argc < 2) 
    {
        printf("File names were not transferred.\n");
        return 0;
    }

    /* Loop through all the files passed via the terminal */
    for(i = 1; i < argc; i++) 
    {
        input_filename = argv[i];
        len_filename = strlen(input_filename);
        
        /* Validate that the file has a '.as' extension */
        if(len_filename >= 3 && strcmp(input_filename + len_filename - 3, ".as") != 0){
            printf("Error: The name '%s' is invalid\n", input_filename);
            continue;
        }
        
        len_base = strlen(argv[i]);
        strcpy(base_name, argv[i]);
        base_name[len_base - 3] = '\0'; /* Extract the base name by stripping the extension */
        
        macro_head = NULL; /* Reset the macro list for each new file */
        code_image = (BYTE *) malloc(INITIAL_CAPACITY * sizeof(BYTE));
        data_image = (BYTE *) malloc(INITIAL_CAPACITY * sizeof(BYTE));
        init_symbol_table();

        if(code_image == NULL || data_image == NULL){
            fprintf(stderr, "Fatal Error: Memory allocation failed for assembler structures.\n");
            free_symbol_table();
            return 0;
        }

        /* Open the source file (.as) for reading */
        input_file = fopen(input_filename, "r");
        if(input_file == NULL){
            printf("Error: Cannot open file '%s'\n", base_name);
            free(code_image);
            free(data_image);
            continue; /* Move to the next file */
        }

        /* Create and open the target file (.am) for writing the macro-expanded code */
        sprintf(am_filename, "%s.am", base_name);
        file_am = fopen(am_filename, "w");
        if(file_am == NULL) {
            printf("Error: Cannot create file '%s'\n", base_name);
            fclose(input_file);
            free(code_image);
            free(data_image);
            continue;
        }

        /* Execute the pre-assembler stage: expand macros */
        if(process_macros(input_file, file_am, &macro_head) == 0){
           printf("Macro deployment failed for %s, moving to next file.\n", base_name);
           fclose(input_file);
           fclose(file_am);
           free(code_image);
           free(data_image);
           continue;
        }
        
        /* Close the files after pre-assembler processing is complete */
        fclose(input_file);
        fclose(file_am);
        free_macros(macro_head);

        /* Prepare output filenames for the second pass */
        sprintf(ob_filename, "%s.ob", base_name);
        sprintf(ext_filename, "%s.ext", base_name);
        sprintf(ent_filename, "%s.ent", base_name);

        /* Execute the first pass on the generated .am file */
        if(run_first_pass(am_filename, NULL, &ICF, &DCF) == 0){
            free_symbol_table();
            free(code_image);
            free(data_image);
            continue;
        }
        
        /* Execute the second pass */
        if(run_second_pass(am_filename, ob_filename, ext_filename, ent_filename, &ICF, &DCF) == 0){
            free_symbol_table();
            free(code_image);
            free(data_image);
            continue;
        }
        
        printf("\n");
        
        /* Free allocated memory and clear resources for the current iteration */
        free_symbol_table();
        free(code_image);
        free(data_image);
    }
    return 1;
}
