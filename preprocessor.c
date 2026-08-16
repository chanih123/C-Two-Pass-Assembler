#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "preprocessor.h"


Macro* create_macro(const char* name, int line_number) 
{
Macro *new_macro;
if (name == NULL || strlen(name) == 0)
{
    return NULL;
}

  new_macro = malloc(sizeof(Macro));
   if (new_macro == NULL) {
        return NULL;
    }
new_macro->name = malloc(strlen(name) + 1);
    if (new_macro->name == NULL) 
{
        free(new_macro);
        return NULL;
    }
    strcpy(new_macro->name, name);
    new_macro->content = NULL;
    new_macro->line_number = line_number;
    new_macro->next = NULL;

    return new_macro;
}

int add_macro(Macro **head, Macro *new_macro) {

Macro *current;
   
    if (new_macro == NULL || head == NULL) {
        return FAILURE;
    }

  
    if (*head == NULL) {
        *head = new_macro;
        return SUCCESS;
    }

    current = *head;
    while (current->next != NULL) {
        current = current->next;
    }

new_macro->next = NULL;
current->next = new_macro;


    return SUCCESS;
}
Macro *find_macro(Macro *head, const char *name)
{
    Macro *current = head;

    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
        {
            return current;
        }

        current = current->next;
    }

    return NULL;
}
int is_macro_definition(const char *line)
{
    if (line == NULL)
    {
        return 0;
    }

    if (strncmp(line, "mcro", 4) == 0 &&
        (line[4] == ' ' || line[4] == '\t'))
    {
        return 1;
    }

    return 0;
}
void free_macros(Macro *head)
{
    Macro *current = head;
    Macro *next_macro;

    while (current != NULL)
    {
        next_macro = current->next;

        free(current->name);
        free(current->content);
        free(current);

        current = next_macro;
    }
}
int is_mcroend(const char *line)
{
    if (line == NULL)
    {
        return 0;
    }

    if (strncmp(line, "mcroend", 7) == 0 &&
        (line[7] == '\0' ||
         line[7] == '\n' ||
         line[7] == ' ' ||
         line[7] == '\t'))
    {
        return 1;
    }

    return 0;
}

/* הפונקציה מחזירה 1 אם השם חוקי למאקרו, ו-0 אם זהה למילה שמורה */
int is_valid_macro_name(const char *name) 
{
    int i;
    int num_reserved;
    
    /* רשימת המילים השמורות - יש לעדכן רשימה זו בהתאם להוראות האסמבלי הספציפיות בפרויקט שלך */
    const char *reserved_words[] = {
        "mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec",
        "jmp", "bne", "red", "prn", "jsr", "rts", "hlt",
        "mcro", "mcroend",
        "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
        ".data", ".string", ".entry", ".extern"
    };
    
    if (name == NULL) 
    {
        return 0;
    }

    num_reserved = sizeof(reserved_words) / sizeof(reserved_words[0]);

    for (i = 0; i < num_reserved; i++) 
    {
        if (strcmp(name, reserved_words[i]) == 0) 
        {
            return 0; /* השם אינו חוקי - זוהי מילה שמורה */
        }
    }

    return 1; /* השם חוקי */
}
int append_macro_line(Macro *macro, const char *line)
{
    char *new_content;
    size_t old_length;
    size_t line_length;

    if (macro == NULL || line == NULL)
    {
        return 0;
    }

    old_length = (macro->content == NULL) ?
                 0 : strlen(macro->content);

    line_length = strlen(line);

    new_content = realloc(macro->content,
                          old_length + line_length + 1);

    if (new_content == NULL)
    {
        return 0;
    }

    memcpy(new_content + old_length, line, line_length + 1);

    macro->content = new_content;

    return 1;
}

int process_macros(FILE *input_file, FILE *output_file, Macro **macro_head) 
{
    char line[MAX_LINE_LENGTH];
    char first_word[MAX_LINE_LENGTH];
    char second_word[MAX_LINE_LENGTH];
    char third_word[MAX_LINE_LENGTH]; /* משתנה עזר לבדיקת תווים מיותרים */
    int in_macro = 0;
    int words_read;
    Macro *current_macro = NULL;
    Macro *found_macro = NULL;
    int line_counter = 0;
    int error_flag = 0; /* דגל שגיאה גלובלי עבור הקובץ הנוכחי */

    while (fgets(line, MAX_LINE_LENGTH, input_file) != NULL) 
    {
        line_counter++;
        
        /* איפוס המחרוזות לפני כל קריאה */
        first_word[0] = '\0';
        second_word[0] = '\0';
        third_word[0] = '\0';

        /* חילוץ של עד 3 מילים מהשורה תוך התעלמות מרווחים וטאבים */
        words_read = sscanf(line, "%s %s %s", first_word, second_word, third_word);

        /* שורה ריקה או רק רווחים */
        if (words_read <= 0) 
        {
            if (in_macro) 
            {
                append_macro_line(current_macro, line);
            } 
            else 
            {
                fputs(line, output_file);
            }
            continue;
        }

        if (in_macro) 
        {
            /* אנחנו בתוך הגדרת מאקרו */
            if (strcmp(first_word, "mcroend") == 0) 
            {
                if (words_read > 1) 
                {
                    printf("Error: Extraneous text after 'mcroend' at line %d\n", line_counter);
                    error_flag = 1;
                }
                in_macro = 0;
                current_macro = NULL;
            } 
            else 
            {
                /* שורת תוכן של המאקרו */
                if (!append_macro_line(current_macro, line)) 
                {
                    printf("Error: Memory allocation failed at line %d\n", line_counter);
                    return FAILURE;
                }
            }
        } 
        else 
        {
            /* אנחנו במצב קריאה רגיל */
            if (strcmp(first_word, "mcro") == 0) 
            {
                if (words_read < 2) 
                {
                    printf("Error: Missing macro name at line %d\n", line_counter);
                    error_flag = 1;
                }
                else if (words_read > 2) 
                {
                    printf("Error: Extraneous text after macro name at line %d\n", line_counter);
                    error_flag = 1;
                }
                else if (!is_valid_macro_name(second_word)) 
                {
                    printf("Error: Invalid macro name '%s' at line %d\n", second_word, line_counter);
                    error_flag = 1;
                } 
                else 
                {
                    current_macro = create_macro(second_word, line_counter);
                    add_macro(macro_head, current_macro);
                    in_macro = 1;
                }
            } 
            else 
            {
                /* בדיקה האם המילה הראשונה היא שם של מאקרו מוכר */
                found_macro = find_macro(*macro_head, first_word);
                if (found_macro != NULL) 
                {
                    fputs(found_macro->content, output_file);
                } 
                else 
                {
                    /* שורת קוד רגילה */
                    fputs(line, output_file);
                }
            }
        }
    }

    /* מחזיר כשל אם נמצאה שגיאה כלשהי במהלך עיבוד המאקרואים, אחרת הצלחה */ 
    if(error_flag)
    return FAILURE;
    else
   return SUCCESS; 
}

