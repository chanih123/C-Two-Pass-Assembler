
/**
 * @file preprocessor.c
 * @brief Implementation of the pre-assembler (macro expansion) phase.
 *
 * This file contains the logic for reading the initial assembly source code,
 * identifying macro definitions ("mcro" and "mcroend"), storing their content
 * in a linked list, and replacing macro calls with their expanded content in 
 * the output (.am) file.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "preprocessor.h"

/**
 * @brief Allocates and initializes a new Macro structure.
 *
 * @param name The name of the macro.
 * @param line_number The line number where the macro was defined (for error tracking).
 * @return A pointer to the newly created Macro, or NULL if allocation fails or name is invalid.
 */
Macro* create_macro(const char* name, int line_number) 
{
    Macro *new_macro;
    if(name == NULL || strlen(name) == 0)
    {
        return NULL;
    }

    new_macro = malloc(sizeof(Macro));
    if(new_macro==NULL){
        return NULL;
    }
    
    new_macro->name = malloc(strlen(name) + 1);
    if(new_macro->name==NULL) 
    {
        free(new_macro);
        return NULL;
    }
    
    strcpy(new_macro->name, name);
    new_macro->content=NULL;
    new_macro->line_number=line_number;
    new_macro->next = NULL;

    return new_macro;
}

/**
 * @brief Appends a new macro to the end of the macro linked list.
 *
 * @param head A pointer to the head of the macro list.
 * @param new_macro The new macro node to append.
 * @return SUCCESS (1) if appended successfully, FAILURE (0) otherwise.
 */
int add_macro(Macro **head, Macro *new_macro) {

    Macro *current;
   
    if(new_macro==NULL||head==NULL){
        return FAILURE;
    }

    if(*head==NULL){
        *head = new_macro;
        return SUCCESS;
    }

    current = *head;
    while(current->next!=NULL) {
        current=current->next;
    }

    new_macro->next=NULL;
    current->next=new_macro;

    return SUCCESS;
}

/**
 * @brief Searches for a macro by its name in the linked list.
 *
 * @param head The head of the macro linked list.
 * @param name The name of the macro to search for.
 * @return A pointer to the found Macro, or NULL if not found.
 */
Macro *find_macro(Macro *head, const char *name)
{
    Macro *current=head;

    while(current!=NULL)
    {
        if(strcmp(current->name, name)== 0)
        {
            return current;
        }

        current = current->next;
    }

    return NULL;
}

/**
 * @brief Checks if a given line is a macro definition declaration.
 *
 * @param line The line of text to check.
 * @return 1 if it starts with "mcro", 0 otherwise.
 */
int is_macro_definition(const char *line)
{
    if(line==NULL)
    {
        return 0;
    }

    if(strncmp(line, "mcro", 4) == 0 &&
        (line[4] == ' '||line[4] == '\t'))
    {
        return 1;
    }

    return 0;
}

/**
 * @brief Frees all memory allocated for the macro linked list.
 *
 * @param head The head of the macro linked list to free.
 */
void free_macros(Macro *head)
{
    Macro *current = head;
    Macro *next_macro;

    while(current!=NULL)
    {
        next_macro= current->next;

        free(current->name);
        free(current->content);
        free(current);

        current=next_macro;
    }
}

/**
 * @brief Checks if a given line marks the end of a macro definition.
 *
 * @param line The line of text to check.
 * @return 1 if it is exactly "mcroend", 0 otherwise.
 */
int is_mcroend(const char *line)
{
    if(line == NULL)
    {
        return 0;
    }

    if(strncmp(line, "mcroend", 7) == 0 &&(line[7] == '\0' ||line[7] == '\n' || line[7] == ' ' ||  line[7] == '\t'))
    {
        return 1;
    }

    return 0;
}

/**
 * @brief Validates a macro name against reserved keywords.
 *
 * @param name The name to validate.
 * @return 1 if the name is valid, 0 if it matches a reserved word or is NULL.
 */
int is_valid_macro_name(const char *name) 
{
    int i;
    int num_reserved;
    
    /* List of reserved words - instruction names, directives, and registers */
    const char *reserved_words[] = {
        "mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec",
        "jmp", "bne", "red", "prn", "jsr", "rts", "hlt",
        "mcro", "mcroend",
        "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
        ".data", ".string", ".entry", ".extern"
    };
    
    if(name == NULL) 
    {
        return 0;
    }

    num_reserved = sizeof(reserved_words)/sizeof(reserved_words[0]);

    for(i = 0; i < num_reserved; i++) 
    {
        if(strcmp(name, reserved_words[i]) == 0) 
        {
            return 0; /* Invalid name - it is a reserved word */
        }
    }

    return 1; /* Valid name */
}

/**
 * @brief Appends a line of text to the content of an existing macro.
 *
 * @param macro The macro to which the line will be appended.
 * @param line The line of text to append.
 * @return 1 on success, 0 on failure (e.g., memory allocation error).
 */
int append_macro_line(Macro *macro, const char *line)
{
    char *new_content;
    size_t old_length;
    size_t line_length;

    if(macro==NULL||line==NULL)
    {
        return 0;
    }

    old_length=(macro->content==NULL) ? 0 : strlen(macro->content);

    line_length = strlen(line);

    new_content=realloc(macro->content,
                          old_length + line_length + 1);

    if(new_content==NULL)
    {
        return 0;
    }

    memcpy(new_content + old_length, line, line_length + 1);

    macro->content = new_content;

    return 1;
}

/**
 * @brief Core function of the pre-assembler: processes macros in the input file.
 *
 * Reads the input file line by line. If a macro definition is found, it stores 
 * its content. If a macro call is found, it expands it. Regular lines are written 
 * directly to the output file.
 *
 * @param input_file Pointer to the open source assembly file (.as).
 * @param output_file Pointer to the open target expanded file (.am).
 * @param macro_head Pointer to the head of the macro linked list.
 * @return SUCCESS (1) if processed without errors, FAILURE (0) otherwise.
 */
int process_macros(FILE *input_file, FILE *output_file, Macro **macro_head) 
{
    char line[MAX_LINE_LENGTH];
    char first_word[MAX_LINE_LENGTH];
    char second_word[MAX_LINE_LENGTH];
    char third_word[MAX_LINE_LENGTH]; /* Helper variable to check for extraneous text */
    int in_macro=0;
    int words_read;
    Macro *current_macro=NULL;
    Macro *found_macro=NULL;
    int line_counter=0;
    int error_flag=0; /* Global error flag for the current file */

    while(fgets(line, MAX_LINE_LENGTH, input_file)!= NULL) 
    {
        line_counter++;
        
        /* Reset strings before each read */
        first_word[0]= '\0';
        second_word[0]= '\0';
        third_word[0]= '\0';

        /* Extract up to 3 words from the line, ignoring spaces and tabs */
        words_read = sscanf(line, "%s %s %s", first_word, second_word, third_word);

        /* Empty line or only spaces */
        if(words_read<= 0) 
        {
            if(in_macro) 
            {
                append_macro_line(current_macro, line);
            } 
            else 
            {
                fputs(line, output_file);
            }
            continue;
        }

        if(in_macro) 
        {
            /* Inside a macro definition block */
            if(strcmp(first_word, "mcroend")==0) 
            {
                if(words_read>1) 
                {
                    printf("Error: Extraneous text after 'mcroend' at line %d\n", line_counter);
                    error_flag = 1;
                }
                in_macro = 0;
                current_macro = NULL;
            } 
            else 
            {
                /* A content line of the macro */
                if(!append_macro_line(current_macro, line)) 
                {
                    printf("Error: Memory allocation failed at line %d\n", line_counter);
                    return FAILURE;
                }
            }
        } 
        else 
        {
            /* Normal reading mode (outside macro definitions) */
            if(strcmp(first_word, "mcro") == 0) 
            {
                if(words_read < 2) 
                {
                    printf("Error: Missing macro name at line %d\n", line_counter);
                    error_flag = 1;
                }
                else if(words_read > 2) 
                {
                    printf("Error: Extraneous text after macro name at line %d\n", line_counter);
                    error_flag = 1;
                }
                else if(!is_valid_macro_name(second_word)) 
                {
                    printf("Error: Invalid macro name '%s' at line %d\n", second_word, line_counter);
                    error_flag=1;
                } 
                else 
                {
                    current_macro = create_macro(second_word, line_counter);
                    add_macro(macro_head, current_macro);
                    in_macro=1;
                }
            } 
            else 
            {
                /* Check if the first word is a recognized macro name */
                found_macro = find_macro(*macro_head, first_word);
                if(found_macro!=NULL) 
                {
                    fputs(found_macro->content, output_file);
                } 
                else 
                {
                    /* Standard code line */
                    fputs(line, output_file);
                }
            }
        }
    }

    /* Return failure if any error was found during macro processing, otherwise success */ 
    if(error_flag)
        return FAILURE;
    else
        return SUCCESS; 
}
