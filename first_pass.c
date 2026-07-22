#include "global.h"

#define MAX_LABEL_LEN 31

char line[1024];
char word[1024];
bool Label = false;
IC = 100;
DC = 0;


int main(){
int bytes_read = 0;
char *next_part;
FOREVER{
    /* Read a full line from standard input safely */
    if (fgets(line, sizeof(line), stdin) != NULL) {
        if(is_empty_or_comment(line) == 0){
            if (sscanf(line, "%s%n", word, &bytes_read ) == 1) /* Extract the first word from the line */{
                if(is_label(word) == 0){
                    Label = true;
                next_part = line + bytes_read;
                if(sscanf(next_part, "%s", next_word) == 1){
                    if(is_data_directive(next_part)){
                    /*line 7 */
                    }
                    else /*line 9*/
                } /*end - if(sscanf(next_part, "%s", next_word) == 1)*/
                    }/* end - if(isLabel(current) == 0)*/
                  }/* end -  if (sscanf(line, "%s", current) == 1) 
        } /*end - if(line != "/0" || line != "/")*/
        }/* end - if(is_empty_or_comment(line) == 0)*/ 
    else
        break;
}
int is_label(char *word){
    int i = 0;
    int length = strlen(word);
    if (length < 2 || (length - 1) > MAX_LABEL_LEN)
        return 0;
    if(!((word[0] >= 'a' && word[0] <= 'z') || (word[0] >= 'A' && word[0] <= 'Z')))
      return 0;
    if (word[length - 1] != ':')
        return 0;
    while(i < length - 1){
        if((word[i] >= 'a' && word[i] <= 'z') || (word[i] >= 'A' && word[i] <= 'Z') || (word[i] >= '0' && word[i] <= '9'))
        i++;
        else
             return 0;
    } /*end while*/
    word[length - 1] = '\0';
    if(get_opcode(word) != -1)
        return 0;
    return 1;
}
int is_data_directive(char *word){
  if(word[0] != '.')
    return 0;
  if((strcmp(word, ".dh") == 0) || (strcmp(word, ".dw") == 0) || (strcmp(word, ".db") == 0) || (strcmp(word, ".asciz") == 0))
      return 1;
  return 0;
}


        
        
