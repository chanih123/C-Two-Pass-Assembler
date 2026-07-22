#define MAX_LABEL_LENGTH 31  /*The maximum possible label length */
#define MAX_INSTRUCTION_NAME_LENGTH 4  /*The maximum possible instruction name length */
#define CAPACITY_SYMBOL_TABLE 10  /*Default symbol table size */

#define FOREVER while(1)
int IC = 0;
int DC = 0;
/*קודים שהצאט כתב צריך לעבור עליהם!!!!!!*/

Symbol *symbol_table = NULL;
int symbol_count = 0;       /* כמה סמלים קיימים בפועל */
int symbol_capacity = 0;    /* לכמה סמלים יש כרגע מקום מוקצה בזיכרון */

/*פונקציית אתחול למערך הדינמי (תקראי לה בתחילת הטיפול בכל קובץ) */
void init_symbol_table() {
    symbol_capacity = CAPACITY_SYMBOL_TABLE;
    symbol_count = 0;
    symbol_table = (Symbol*) malloc(symbol_capacity * sizeof(Symbol));
    
    if (symbol_table == NULL) {
        printf("Memory allocation for the symbol table failed.\n");
        exit(1);
    }
}

/* 3. פונקציה להוספת סמל חדש שמגדילה את המערך דינמית כשצריך (realloc) */
void add_symbol(const char *name, int value, SymbolType type, int is_entry) {
    /* אם הגענו לקצה הקיבולת הנוכחית, נגדיל את המערך פי 2 */
    if (symbol_count >= symbol_capacity) {
        symbol_capacity *= 2;
        Symbol *temp = (Symbol *) realloc(symbol_table, symbol_capacity * sizeof(Symbol));
        
        if (temp == NULL) {
            printf("Expanding the symbol table memory failed.\n");
            /* כאן נצטרך בעתיד לשחרר זיכרון ולסגור קבצים בצורה מסודרת */
            exit(1);
        }
        symbol_table = temp;
    }
    /* הוספת האיבר החדש למערך */
    strcpy(symbol_table[symbol_count].name, name);
    symbol_table[symbol_count].value = value;
    symbol_table[symbol_count].type = type;
    symbol_table[symbol_count].is_entry = is_entry;
    
    symbol_count++; /* עדכון מספר הסמלים */
}

/* 4. חובה! פונקציה לשחרור הזיכרון בסיום התוכנית (למניעת Memory Leaks) */
void free_symbol_table() {
    if (symbol_table != NULL) {
        free(symbol_table);
        symbol_table = NULL;
        symbol_count = 0;
        symbol_capacity = 0;
    }
}

/* הגדרת טיפוס של בית בודד (8 סיביות ללא סימן) */
typedef unsigned char BYTE;

/* משתנים עבור תמונת הקוד (מנוהלת על ידי IC) */
BYTE *code_image = NULL;
int IC = 0;                 /* מונה ההוראות */
int code_capacity = 0;

/* משתנים עבור תמונת הנתונים (מנוהלת על ידי DC) */
BYTE *data_image = NULL;
int DC = 0;                 /* מונה הנתונים */
int data_capacity = 0;


typedef enum {SYMBOL_EXTERNAL, SYMBOL_CODE, SYMBOL_DATA} SymbolType;
typedef struct{
  char name[MAX_LABEL_LENGTH +1];
  int value;
  SymbolType type;
  int is_entry;
} Symbol;

typedef enum {R,I,J} type;
typedef struct{
  char name[MAX_INSTRUCTION_NAME_LENGTH +1];
  type op_type;
  int funct;
  int opcode;
} instruction;
  static instruction instructions [] ={
  {"add",R, 1,0},
  {"sub",R, 2, 0},
  {"and",R, 3, 0},
  {"or",R, 4, 0},
  {"nor",R, 5, 0},
  {"move",R, 1, 1},
  {"mvhi",R, 2, 1},
  {"mvlo",R, 3, 1},
  {"addi",I, 0, 10},
  {"subi",I, 0, 11},
  {"andi",I, 0, 12},
  {"ori",I, 0, 13},
  {"nori",I, 0, 14},
  {"bne",I, 0, 15},
  {"beq",I, 0, 16},
  {"blt",I, 0, 17},
  {"bgt",I, 0, 18},
  {"lb",I, 0, 19},
  {"sb",I, 0, 20},
  {"lw",I, 0, 21},
  {"sw",I, 0, 22},
  {"lh",I, 0, 23},
  {"sh",I, 0, 24},
  {"jmp",J, 0, 30},
  {"la",J, 0, 31},
  {"call",J, 0, 32},
  {"hlt",J, 0, 63},
  };
  
  /* להעביר ליוטילס נקודה סי ולהצהיר ביוטלס נקודה איץ*/
int is_empty_or_comment(char *line){
  int i = 0;
  while(line[i] == '\t' || line[i] != ' '){
      i++;
  }
  if(line[i] == '\0' || line[i] == '\n' || line[i] == '\r' || line[i] == ';'){
    return 1;
  }
  return 0;
}
int get_opcode(char * name)
{
  size_t i = 0;
  while(i < sizeof(instructions)/sizeof(instruction))
  {
    if(strcmp(name,instructions[i].name)==0)
    {
      return instructions[i].opcode;
    }
    i++;
  }
  return -1;
}
