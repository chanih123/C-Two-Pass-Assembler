#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "preprocessor.h"
#include "global.h"
#include "first_pass.h"

int main(int argc, char *argv[]) 
{
    int i, j;
    FILE *input_file;
    FILE *output_file;
    char *input_filename;
    char *output_filename;
    Macro *macro_head;
    int process_result;

    /* בדיקה שהועברו ארגומנטים משורת הפקודה (שמות הקבצים ללא סיומת) */
    if (argc < 2) 
    {
        fprintf(stderr, "Usage: %s <file1> <file2> ...\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* לולאה על כל הקבצים שהועברו בטרמינל */
    for (i = 1; i < argc; i++) 
    {
        macro_head = NULL; /* איפוס רשימת המאקרואים עבור כל קובץ חדש */

        /* הקצאת זיכרון לשמות הקבצים + 4 תווים עבור ".as" / ".am" והתו '\0' */
        input_filename = malloc(strlen(argv[i]) + 4);
        output_filename = malloc(strlen(argv[i]) + 4);

        if (input_filename == NULL || output_filename == NULL) 
        {
            printf("Error: Memory allocation failed for filenames.\n");
            continue; /* עוברים לקובץ הבא אם יש כזה */
        }

        /* יצירת שמות הקבצים עם הסיומות המתאימות */
        strcpy(input_filename, argv[i]);
        strcat(input_filename, ".as");

        strcpy(output_filename, argv[i]);
        strcat(output_filename, ".am");

        /* פתיחת קובץ המקור (.as) לקריאה */
        input_file = fopen(input_filename, "r");
        if (input_file == NULL) 
        {
            printf("Error: Cannot open file '%s'\n", input_filename);
            free(input_filename);
            free(output_filename);
            continue; /* עוברים לקובץ הבא */
        }

        /* פתיחת קובץ היעד (.am) לכתיבה */
        output_file = fopen(output_filename, "w");
        if (output_file == NULL) 
        {
            printf("Error: Cannot create file '%s'\n", output_filename);
            fclose(input_file);
            free(input_filename);
            free(output_filename);
            continue;
        }

        printf("=== Processing file: %s ===\n", input_filename);

        /* הפעלת שלב קדם-האסמבלר - פרישת המאקרואים */
        process_result = process_macros(input_file, output_file, &macro_head);

        /* סגירת הקבצים בסיום העיבוד */
        fclose(input_file);
        fclose(output_file);

        /* ניקוי הזיכרון של רשימת המאקרואים */
        free_macros(macro_head);

        /* ניהול התוצאה: מעבר לשלב הבא או עצירה ומחיקה */
        if (process_result == SUCCESS) 
        {
            printf("Macro unrolling completed successfully for '%s'.\n", argv[i]);
            printf("=== Starting First Pass for file: %s ===\n\n", output_filename);
            
            /* הרצת המעבר הראשון על הקובץ שנוצר */
            if (run_first_pass(output_filename)) 
            {
                printf("\n>>> First Pass Completed Successfully for '%s'! <<<\n", argv[i]);
                printf("Final IC (Instruction Counter) = %d\n", IC);
                printf("Final DC (Data Counter)        = %d\n\n", DC);

                /* 1. הדפסת תמונת הקוד (code_image) */
                printf("=========================================\n");
                printf("             CODE IMAGE (%d bytes)       \n", IC - 100);
                printf("=========================================\n");
                for (j = 100; j < IC; j += 4) {
                    printf("Address %04d:  %02X %02X %02X %02X\n", 
                           j, 
                           code_image[j], 
                           code_image[j + 1], 
                           code_image[j + 2], 
                           code_image[j + 3]);
                }
                printf("\n");

                /* 2. הדפסת תמונת הנתונים (data_image) */
                printf("=========================================\n");
                printf("             DATA IMAGE (%d bytes)       \n", DC);
                printf("=========================================\n");
                for (j = 0; j < DC; j++) {
                    printf("DC[%04d] (Absolute %04d):  0x%02X (%d)\n", 
                           j, 
                           IC + j, 
                           data_image[j], 
                           (signed char)data_image[j]);
                }
                printf("=========================================\n\n");
            } 
            else 
            {
                printf("\n>>> First Pass Failed due to errors in file '%s'. <<<\n\n", output_filename);
            }
        } 
        else 
        {
            printf("Errors found in '%s'. Stopping process for this file.\n\n", input_filename);
            /* לפי ההנחיות: אם יש שגיאה במאקרו, עוצרים ולא ממשיכים לשלב הבא. */
            remove(output_filename);
        }

        /* שחרור זיכרון שמות הקבצים בסוף הטיפול בקובץ הנוכחי */
        free(input_filename);
        free(output_filename);
    }

    return EXIT_SUCCESS;
}
