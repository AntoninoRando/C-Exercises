#include <string.h>
#include <stdio.h>
#include "inputReader.h"

// #include "memory.c"

typedef struct Commands
{
    char **args;
    struct Commands *next;
} Commands;

int read_line(char dest[INPUT_SIZE], size_t limit)
{
    // PROMPT
    printf("(myShell) $ ");

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

struct Commands *parse_line(char *input, int *quit)
{
    // TODO: how to properly allocate memory for a struct?
    struct Commands *commands = NULL, *head = NULL;

    char *cmd;
    for (cmd = strtok(input, ";"); cmd != NULL; cmd = strtok(NULL, ";"))
    {
        if (strcmp(cmd, "quit") == 0)
        {
            *quit = 1;
            continue;
        }

        // if (strcmp(command, "^[[A") == 0) // L'altro e' ^[[B
        // {
        //     print_up();
        //     continue;
        // }

        if (commands == NULL)
        {
            commands = malloc(sizeof(Commands));
            commands->args = take_cmdargs(cmd);
            head = commands;
        }
        else
        {
            commands->next = malloc(sizeof(Commands));
            commands->args = take_cmdargs(cmd);
            commands = commands->next;
        }
    }

    return head;
}

char** take_cmdargs(char *cmd)
{
    char** args = malloc(64 * sizeof(char*)); // TODO: cambiare quel 64 in qualcosa di variabile
    int i = 0;
    for (char *tok = strtok(cmd, " \t\r\n\a"); tok != NULL; tok = strtok(NULL, " \t\r\n\a"))
    {
        args[i++] = tok;
    }
    return args;
}