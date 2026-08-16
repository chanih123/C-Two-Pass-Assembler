#include <stdio.h>
#include <string.h>
#include "global.h"
#include "utils.h"
#include "symbol_table.h"

/*
 * הפונקציה המבצעת את המעבר השני באסמבלר.
 * מבוססת באופן מלא על אלגוריתם המעבר השני.
 */
int run_second_pass(char *filename) {
    int opcode = 0;
    int line_number = 0;
    int bytes_read = 0;
    int error_found = 0;
    
    char line[MAX_LINE_LENGTH];
    char word[MAX_LINE_LENGTH];
    char command_name[MAX_LINE_LENGTH];
    char entry_label[MAX_LABEL_LENGTH];
    char *parameters;
    char *next_part;
    
    FILE *fp;

    /* אתחול מונה ההוראות מחדש ל-100 עבור המעבר השני */
    IC = 100;

    fp = fopen(filename, "r");
    if(fp == NULL) {
        fprintf(stderr, "Fatal Error: The file does not exist.\n");
        return 0;
    }

    /* שלב 1: קרא את השורה הבאה בקוד המקור. אם נגמר קוד המקור, עבור ל-9 */
    while(fgets(line, sizeof(line), fp) != NULL) {
        line_number++;
        word[0] = '\0';
        command_name[0] = '\0';
        bytes_read = 0;

        /* שלב 2: אם זוהי שורת הערה או שורה ריקה, חזור ל-1 */
        if(is_empty_or_comment(line) == 1) {
            continue;
        }

        if(sscanf(line, "%s %n", word, &bytes_read) == 1) {
            
            /* שלב 3: אם השדה הראשון בשורה הוא סמל (תווית), דלג עליו */
            if (word[strlen(word) - 1] == ':') {
                next_part = line + bytes_read;
                if(sscanf(next_part, "%s %n", command_name, &bytes_read) != 1) {
                    continue; /* שגיאה שכבר במעבר ראשון */
                }
                parameters = next_part + bytes_read;
            } else {
                strcpy(command_name, word);
                parameters = line + bytes_read;
            }

            /* שלב 4: האם זוהי שורת הנחיה שאינה הנחיית .entry? אם כן, חזור ל-1 */
            if(is_data_directive(command_name) || strcmp(command_name, ".extern") == 0) {
                continue; 
            }

            /* שלב 5: האם זוהי הנחיית .entry? אם לא, עבור ל-7 */
            if(strcmp(command_name, ".entry") == 0) {
                if(sscanf(parameters, "%s", entry_label) == 1) {
                    /* שלב 6: הוסף בטבלת הסמלים את המאפיין entry. אם הסמל לא נמצא, יש להודיע על שגיאה */
                    if (update_symbol_as_entry(symbol_table, entry_label) == 0) {
                        fprintf(stderr, "Error in line %d: Symbol '%s' for .entry not found in symbol table.\n", line_number, entry_label);
                        error_found++;
                    }
                }
                continue; /* חזור ל-1 */
            }

            /* 
             * שלב 7: זוהי שורת הוראה. השלם בתמונת הזיכרון את הקידוד הבינארי החסר.
             * בהוראה מסוג J (מלבד hlt) יש לקודד את כתובת התווית.
             * בהוראת הסתעפות מותנית יש לחשב ולקודד את המרחק.
             */
            opcode = get_opcode(command_name);
            if(opcode != -1) {
                /* הוראות הסתעפות - I type (opcodes 15-18: bne, beq, blt, bgt) */
                if(opcode >= 15 && opcode <= 18) {
                    /* במקרה של הסתעפות מותנית אם נדרש סמל המאופיין כ-external, יש לדווח על שגיאה */
                    if(complete_I_branch_instruction(parameters, IC, line_number) == 0) {
                        error_found++;
                    }
                }
                /* הוראות קפיצה - J type (opcodes 30-32: jmp, la, call). הוראת hlt (63) לא דורשת השלמה */
                else if(opcode >= 30 && opcode <= 32) {
                    if(complete_J_instruction(parameters, IC, line_number) == 0) {
                        error_found++;
                    }
                    /* שלב 8: אם נעשה שימוש בסמל שמאופיין כ-external בהוראה מסוג J, 
                     * הפונקציה תצטרך להוסיף את הכתובת לרשימת קבצי ה-ext (מתבצע בתוך complete_J_instruction) */
                }

                /* קידום ה-IC ב-4 עבור כל הוראה שעברנו עליה (כדי לשמור על סינכרון הכתובות) */
                IC += 4;
            }
        }
    }

    fclose(fp);

    /* שלב 9: קוד המקור נסרק בשלמותו. אם נמצאו שגיאות במעבר השני, עצור כאן (לא ייבנו קבצי פלט) */
    if(error_found > 0) {
        fprintf(stderr, "%d errors found during second pass in file %s. Output files will not be generated.\n", error_found, filename);
        return 0;
    }

    /* שלב 10: בנה את קבצי הפלט. מוחזר 1 (הצלחה) כדי שה-main יוכל לקרוא לפונקציית בניית הקבצים */
    return 1;
}
