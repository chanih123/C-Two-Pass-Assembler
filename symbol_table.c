#include "symbol_table.h"
/*קודים שהצאט כתב צריך לעבור עליהם!!!!!!*/


/*פונקציית אתחול למערך הדינמי (תקראי לה בתחילת הטיפול בכל קובץ) */
Symbol* init_symbol_table(int initial_capacity) {
    Symbol *table = (Symbol *) malloc(initial_capacity * sizeof(Symbol));
    if (table == NULL) {
        fprintf(stderr, "Fatal Error: Memory allocation failed for symbol table\n");
        exit(1);
    }
    return table;
}
/* 3. פונקציה להוספת סמל חדש שמגדילה את המערך דינמית כשצריך (realloc) */
int add_symbol(Symbol **symbol_table, int *symbol_count, int *symbol_capacity, char *name, int value, SymbolType type, int is_entry, int line_number) {
      if(find_symbol_index(*symbol_table, *symbol_count, name) != -1){
      fprintf(stderr, "Error in line %d: Symbol '%s' is already defined\n", line_number, name);
      return 0;
      }
    /* אם הגענו לקצה הקיבולת הנוכחית, נגדיל את המערך פי 2 */
    if (*symbol_count >= *symbol_capacity) {
        *symbol_capacity *= 2;
        *symbol_table = (Symbol *) realloc(*symbol_table, *symbol_capacity * sizeof(Symbol));
    
        
        if (*symbol_table == NULL) {
            printf("Expanding the symbol table memory failed.\n");
            /* כאן נצטרך בעתיד לשחרר זיכרון ולסגור קבצים בצורה מסודרת */
            exit(1);
        }
    }
    /* הוספת האיבר החדש למערך */
    strcpy((*symbol_table)[*symbol_count].name, name);
    (*symbol_table)[*symbol_count].value = value;
    (*symbol_table)[*symbol_count].type = type;
    (*symbol_table)[*symbol_count].is_entry = is_entry;
    
    (*symbol_count) ++; /* עדכון מספר הסמלים */
    return 1;
}

/* 4. חובה! פונקציה לשחרור הזיכרון בסיום התוכנית (למניעת Memory Leaks) */
void free_symbol_table(Symbol **symbol_table, int *symbol_count, int *symbol_capacity) {
    if (symbol_table != NULL) {
        free(symbol_table);
        symbol_table = NULL;
        symbol_count = 0;
        symbol_capacity = 0;
    }
}
int find_symbol_index(Symbol *symbol_table, int symbol_count, char *name){
    int i;
    for (i = 0; i < symbol_count; i++) {
       if (strcmp(symbol_table[i].name, name) == 0) {
            return i; /* מחזיר את האינדקס במערך שבו נמצא הסמל */
        }
    }
    return -1; /* הסמל לא נמצא */
}
void update_data_symbol_table(Symbol **symbol_table, int *symbol_count, int *symbol_capacity, int ICF){
    int i;
    for (i = 0; i < *symbol_count; i++) {
        if (symbol_table[i]->type ==  data){
            symbol_table[i]->value += ICF;
        }
    }
}

