#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <stdio.h>
#include "global.h"

/* הגדרת הקבועים לפרויקט */
#define SUCCESS 1
#define FAILURE 0

/* 
 * הגדרת מבנה הנתונים של מאקרו.
 * חובה להגדיר זאת כאן כדי שכל קובץ שעושה #include "preprocessor.h" יכיר את הטיפוס Macro.
 */
typedef struct Macro {
    char *name;
    char *content;
    int line_number;
    struct Macro *next;
} Macro;

/* חתימות הפונקציות של הפרה-אסמבלר */
Macro *create_macro(const char *name, int line_number);
int add_macro(Macro **head, Macro *new_macro);
Macro *find_macro(Macro *head, const char *name);
void free_macros(Macro *head);
int append_macro_line(Macro *macro, const char *line);
int process_macros(FILE *input_file, FILE *output_file, Macro **macro_head);
int is_valid_macro_name(const char *name);

#endif /* PREPROCESSOR_H */

