#include <stdio.h>
#include <ctype.h>
#include <string.h>

/* הצהרת הפונקציה */
int check_directive_parameter(char *parameters, int line_number);

int main(void)
{
    char input[256];
    int line_number = 1;

    while (1)
    {

        if (check_directive_parameter("6, -9", line_number))
            printf("Valid parameters!\n");

        line_number++;
        if (check_directive_parameter("10203", line_number))
            printf("Valid parameters!\n");
        printf("\n");
    }

    return 0;
}
int check_directive_parameter(char *parameters, int line_number)
{
    char *p = parameters;
    int expect_number = 1;

    /* דילוג על רווחים בתחילת השורה */
    while (*p == ' ' || *p == '\t')
        p++;

    /* אין פרמטרים */
    if (*p == '\0') {
        fprintf(stderr, "Error in line %d: Missing parameter\n", line_number);
        return 0;
    }

    while (1) {

        if (expect_number) {

            /* פסיק לפני המספר הראשון או אחרי פסיק */
            if (*p == ',') {
                fprintf(stderr,
                        "Error in line %d: Comma before the first number\n",
                        line_number);
                return 0;
            }

            /* סימן אופציונלי */
            if (*p == '+' || *p == '-')
                p++;

            /* חייבת להיות לפחות ספרה אחת */
            if (!isdigit(*p)) {
                fprintf(stderr,
                        "Error in line %d: Illegal number\n",
                        line_number);
                return 0;
            }

            /* קריאת כל הספרות */
            while (isdigit(*p))
                p++;

            expect_number = 0;
        }

        /* רווחים אחרי מספר */
        while (*p == ' ' || *p == '\t')
            p++;

        /* סוף המחרוזת */
        if (*p == '\0')
            return 1;

        /* חייב להיות פסיק */
        if (*p != ',') {
            fprintf(stderr,
                    "Error in line %d: Missing comma between numbers\n",
                    line_number);
            return 0;
        }

        /* נמצא פסיק */
        p++;

        /* רווחים אחרי פסיק */
        while (*p == ' ' || *p == '\t')
            p++;

        /* פסיק אחרי הפסיק הראשון */
        if (*p == ',') {
            fprintf(stderr,
                    "Error in line %d: Multiple consecutive commas\n",
                    line_number);
            return 0;
        }

        /* פסיק בסוף */
        if (*p == '\0') {
            fprintf(stderr,
                    "Error in line %d: Comma after the last number\n",
                    line_number);
            return 0;
        }

        expect_number = 1;
    }
}

int check_number(char *num, int line_number){
char *end;
long temp;
if(num[0] == '+' || num[0] == '-'){
    temp = strtol(reg + 1, &end, 10);
    if(*end != '\0' || end == reg + 1){
      fprintf(stderr, "Error in line %d: Invalid number\n", line_number);
      return 0;
      }
return 1; 
}
int check_directive_parameter(char *parameters, int line_number){
char *reg;
char copy[100];
int reg_num = 0;
int count = 0;

strcpy(copy, parameters);
num = strtok(copy, ", \t\n");

while(reg != NULL){
    reg_num = check_register(reg, line_number) ;
    if(reg_num == -1)
      return 0;
    }    
    count++;
    reg = strtok(NULL, ", \t\n");
}
if((opcode == 0 && count != 3) || (opcode == 1 && count != 2)){
  fprintf(stderr, "Error in line %d: Invalid number of registers\n", line_number);
  return 0;
}
return 1;
}
void blanks_remove(char str[]){
  int i = 0, j = 0;
  while(str[i] != '\0' && str[j] != '\0'){
    if(!isspace(str[i])){
      i++;
      }
    else{
      j = i +1;
      while(str[j] == ' ' || str[j] == '\t' || str[j] == '\n'){
          j++;
          }
      str[i] = str[j];
      str[j] = ' ';
    }
    }
    str[i +1] = '\0';
}
