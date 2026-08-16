#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "preprocessor.h"

int main(int argc, char *argv[]) 
{
    int i;
    FILE *input_file;
    FILE *output_file;
    char *input_filename;
    char *output_filename;
    Macro *macro_head;
    int process_result;

    /* בדיקה שהועברו ארגומנטים משורת הפקודה (שמות הקבצים ללא סיומת) */
    if (argc < 2) 
    {
        printf("Usage: %s <file1> <file2> ...\n", argv[0]);
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

        printf("Processing file: %s...\n", input_filename);

        /* הפעלת שלב קדם-האסמבלר - פרישת המאקרואים */
        process_result = process_macros(input_file, output_file, &macro_head);

        /* סגירת הקבצים בסיום העיבוד */
        fclose(input_file);
        fclose(output_file);

        /* ניקוי הזיכרון של רשימת המאקרואים - חשוב להשתמש בפונקציה שכתבת קודם */
        free_macros(macro_head);

        /* ניהול התוצאה: מעבר לשלב הבא או עצירה ומחיקה */
        if (process_result == SUCCESS) 
        {
            printf("Macro unrolling completed successfully for '%s'.\n", argv[i]);
            
            /* TODO: כאן יתווספו הקריאות לשלבים הבאים של האסמבלר 
               first_pass(output_filename); 
               וכו' */
        } 
        else 
        {
            printf("Errors found in '%s'. Stopping process for this file.\n", argv[i]);
            /* לפי ההנחיות: אם יש שגיאה במאקרו, עוצרים ולא ממשיכים לשלב הבא.
               נהוג גם למחוק את קובץ ה-.am הפגום שנוצר */
            remove(output_filename);
        }

        /* שחרור זיכרון שמות הקבצים בסוף הטיפול בקובץ הנוכחי */
        free(input_filename);
        free(output_filename);
    }

    return EXIT_SUCCESS;
}
