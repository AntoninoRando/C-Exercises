#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "executer.h"
#include "utils.c"

int read_line(char dest[INPUT_SIZE], FILE *__restrict__ __stream)
{
    int allocated = 0, quotes = 0, end = EOI;

    do
    {
        int size = INPUT_SIZE - allocated;
        char append[size];

        if (fgets(append, size, __stream) == NULL)
        {
            end = EOF;
            break;
        }

        memcpy(&dest[allocated], append, size);
        allocated = strlen(dest);

        if (dest[allocated - 1] != '\n') // Limit exided means no new-line at the end.
        {
            return OVERFLOW;
        }

        quotes += count_quotes(append);
    } while (quotes % 2 != 0);

    return end;
}

void execute_input(char *input, int *quit)
{
    int inputLen = strlen(input);

    if (inputLen == 0)
    {
        return;
    }

    char **args = malloc(2 * sizeof(char *)); // 2: 1 per il comando e 1 per il NULL terminator
    int argc = 0, argi = 0;
    int writeAll = 0;
    args[argc] = malloc(INPUT_SIZE); // Bisogna allocare piu' memoria per poter scrivere tutti i caratteri

    for (int i = 0; i < inputLen; i++)
    {
        // Escape character
        if (input[i] == '\\')
        {
            i++;
            if (i == inputLen)
            {
                break;
            }
            args[argc][argi] = input[i];
            argi++;
            continue;
        }

        // Check if quotes are closing or opening
        if (input[i] == '"')
        {
            writeAll = !writeAll;
            continue;
        }

        // Write everything inside quotes
        if (writeAll)
        {
            args[argc][argi] = input[i];
            argi++;
            continue;
        }

        // If not inside quotes, check for dividers
        int argEnded = 0;
        int cmdEnded = 0;
        int write = 1;

        if (strchr(ARG_DIVS, input[i]) != NULL)
        {
            argEnded = 1;
            write = 0;
        }
        if (i == inputLen - 1)
        {
            argEnded = 1;
            cmdEnded = 1;
        }
        if (strchr(CMD_DIVS, input[i]) != NULL)
        {
            argEnded = 1;
            cmdEnded = 1;
            write = 0;
        }

        // Write arguments and execute command
        if (write)
        {
            args[argc][argi] = input[i];
            argi++;
        }

        // Se questo argomento non e' ancora finito, continua a scriverlo (nella prossima iterazione)
        if (!argEnded || argi <= 0)
        {
            continue;
        }

        args[argc][argi] = '\0';

        if (cmdEnded)
        {
            // Execute command
            args[argc + 1] = NULL;
            if (execute_cmd(args) == -1)
            {
                *quit = 1;
            }

            // Free memory
            for (int f = 0; f <= argc; f++)
            {
                free(args[f]);
            }
            free(args);

            // Reset for next command
            args = malloc(2 * sizeof(char *));
            args[0] = malloc(INPUT_SIZE);
            argc = 0;
            argi = 0;
            writeAll = 0;
        }
        else
        {
            argc++;
            argi = 0;
            args = realloc(args, (argc + 2) * sizeof(char *)); // 1 per il NULL alla fine
            args[argc] = malloc(INPUT_SIZE);
        }
    }

    free(args);
}

static int execute_cmd(char **args)
{
    if (!args)
    {
        return 0;
    }

    if (strcmp(args[0], "quit") == 0)
    {
        return -1;
    }

    pid_t pid = fork();

    // error
    if (pid == -1)
    {
        perror("Fatal error");
        return 1;
    }

    // child
    if (pid == 0)
    {
        if (execvp(args[0], args) == -1) // Cambio l'immagine al figlio e controllo possibili errori
        {
            printf("Invalid command %s\n", args[0]);
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}