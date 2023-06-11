#include <stdio.h>
#include <string.h>

#define BLANK 0
#define OVERFLOW -1   
#define OK 1
#define PROMPT "myShell > "

static int read_line(char* dest, size_t limit)
{
    printf("%s", PROMPT);
    
    if (fgets(dest, limit, stdin) == NULL)
    {
        return BLANK;
    }

    // Limit exided means no 'new line' char at the end.
    if (dest[strlen(dest) - 1] != '\n')
    {
        // NON SO SE VADA FLUSSHATO L'EXTRA
        return OVERFLOW;
    }

    // Replace newline character with string terminator.
    dest[strlen(dest) - 1] = '\0';
    return OK;
}

static int get_commands(char* input)
{
    // char* token = strtok(input, ';');
    // while (token != NULL)
    // {
        
    // }
}