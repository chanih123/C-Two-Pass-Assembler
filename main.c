#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "preprocessor.h"
#include "global.h"
#include "first_pass.h"

#define IC_INIT_VALUE 100

int main(int argc, char *argv[]) 
{
    int i, j;
    FILE *input_file;
    FILE *file_.am;
    FILE *file_.ob;
    FILE *file_.ext;
    FILE *file_.ent;
    char *input_filename;
    char *output_filename;
    Macro *macro_head;
    int process_result;
    int len;
    int ICF, int DCF;

    /* בדיקה שהועברו ארגומנטים משורת הפקודה (שמות הקבצים ללא סיומת) */
    if(argc < 2) 
    {
        fprintf("Error: File names were not transferred.\n");
        return 0;
    }

    /* לולאה על כל הקבצים שהועברו בטרמינל */
    for(i = 1; i < argc; i++) 
    {
        macro_head = NULL; /* איפוס רשימת המאקרואים עבור כל קובץ חדש */
        code_image = (BYTE *) malloc(INITIAL_CAPACITY * sizeof(BYTE));
        data_image = (BYTE *) malloc(INITIAL_CAPACITY * sizeof(BYTE));
        symbol_table = init_symbol_table(INITIAL_CAPACITY);

        if(code_image == NULL || data_image == NULL || symbol_table == NULL){
            fprintf(stderr, "Fatal Error: Memory allocation failed for assembler structures.\n");
            free_symbol_table();
            return 0;
        }
        input_filename = argv[i];
        len = strlen(input_filename);
        if(len >= 3 && strcmp(input_filename + len - 3, ".as") != 0){
            printf("Error: The name '%s' is invalid\n", input_filename);
            continue;
        }
        /* פתיחת קובץ המקו (=.as) לקריאה */
        input_file = fopen(input_filename, "r");
        if(input_file == NULL) 
        {
            printf("Error: Cannot open file '%s'\n", input_filename);
            free(input_filename);
            free(output_filename);
            continue; /* עוברים לקובץ הבא */
        }

        /* פתיחת קובץ היעד (.am) לכתיבה */
        file_.am = fopen(file_.am, "w");
        if(file_.am == NULL) 
        {
            printf("Error: Cannot create file '%s'\n", file_.am);
            fclose(input_file);
            free(input_filename);
            free(file_.am);
            continue;
        }

        /* הפעלת שלב קדם-האסמבלר - פרישת המאקרואים */
        if(process_macros(input_file, file_.am, &macro_head) == 0){
           printf("Macro deployment failed for %s, moving to next file.\n", file_.am);
           fclose(file_.am);
           continue;
        }
        /* סגירת הקבצים בסיום העיבוד */
        fclose(input_file);
        fclose(file_.am);
        free_macros(macro_head);

            /* הרצת המעבר הראשון על הקובץ שנוצר */
        if(run_first_pass(file_.am, &symbol_table, &ICF, &DCF) == 0){
            printf("\n>>> First Pass Failed due to errors in file '%s'.\n", file_.am);
            continue;
        }
        if(run_second_pass(file_.am, file_.ob, file_.ext, file_.ent) == 0){
            printf("\n>>> Second Pass Failed due to errors in file '%s'.\n", file_.am);
            continue;
        }
        printf("\n");
        free(input_filename);
        free(file_.am);
        free(file_.ob);
        free(file_.ext);
        free(file_.ent);
    }

    return 1;
}
