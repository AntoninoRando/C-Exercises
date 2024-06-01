#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "executer.h"
#include "utils.c"
#include "shell.h"

int read_line(char dest[INPUT_SIZE], FILE *__restrict__ __stream)
{
    int allocated = 0, quotes = 0, endWithN = 1, end = EOI;

    do
    {
        int size = INPUT_SIZE - allocated - 1;
        char append[size];

        if (fgets(append, size, __stream) == NULL)
        {
            end = EOF;
            break;
        }

        memcpy(&dest[allocated], append, size);
        allocated = strlen(dest);

        // Overflow means not \n at the end and not EOF as next char
        // (i.e. there are other characters to read in the same line
        // that the fgets didn't get).
        if (dest[allocated - 1] != '\n')
        {
            if (fgetc(__stream) != EOF)
            {
                return OVERFLOW;
            }

            /* Buffer doesn't end with \n but all buffer size is taken, thus even if
            the string is full stored (the following char is EOF) there is no space
            for the string-terminator. This means overflow.*/
            if (allocated >= INPUT_SIZE)
            {
                return OVERFLOW;
            }

            endWithN = 0;
            break;
        }

        quotes += count_quotes(append);
    } while (quotes % 2 != 0);

    /* If ends with \n, that char can be replaced with \0. Otherwise the \0 must be
    added after (there is space, we already checked for overflow!)*/
    dest[allocated - endWithN] = '\0';

    return end;
}

void execute_input(char *input, int *quit)
{
    int inputLen = strlen(input);
    char **args = malloc(2 * sizeof(char *)); // 2: 1 per il comando e 1 per il NULL terminator
    int argc = 0, argi = 0;
    int writeAll = 0;
    args[argc] = malloc(INPUT_SIZE); // Bisogna allocare piu' memoria per poter scrivere tutti i caratteri

    /* When looping we must include the null-terminato because if the command ends with a quote,
    that quote will not indicate that the command is ended but will indicate that the quotes are
    ended instead.*/
    for (int i = 0; i <= inputLen; i++)
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
        if (i == inputLen) // This is the string terminator \0: don't write it, we write it manually
        {
            argEnded = 1;
            cmdEnded = 1;
            write = 0;
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

    // Free memory
    for (int f = 0; f <= argc; f++)
    {
        free(args[f]);
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
            printf("%sInvalid command %s\n", PROMPT, args[0]);
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}