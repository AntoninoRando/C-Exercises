#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
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

// TODO: occhio a quando ci sono soli ; perche' mi da errore. Occhio anche al quit
// TODO: occhio a quando si termina con ;
// TODO: occhio a quando si mettono degli spazi dopo ;
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

        if (input[i] == ' ')
        {
            argEnded = 1;
            write = 0;
        }
        if (i == inputLen - 1)
        {
            argEnded = 1;
            cmdEnded = 1;
        }
        if (strchr(DIVIDERS, input[i]) != NULL)
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

        if (!argEnded || argi <= 0)
        {
            continue;
        }
        
        args[argc][argi] = '\0';

        if (!cmdEnded)
        {
            argc++;
            argi = 0;
            args = realloc(args, (argc + 1) * sizeof(char *)); // 1 per il NULL alla fine
            args[argc] = malloc(INPUT_SIZE);
            continue;
        }
        
        args[argc+1] = NULL;

        if (execute_cmd(args) == -1)
        {
            *quit = 1;
        }

        for (int f = 0; f <= argc; f++)
        {
            free(args[f]);
        }
        free(args);

        args = malloc(2 * sizeof(char *));
        argc = 0;
        argi = 0;
        writeAll = 0;
        args[argc] = malloc(INPUT_SIZE);
    }

    if (args)
    {
        free(args);
    }
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
    /* TODO: non ho capito in che senso fork() torna due volte; da quando eseguo
    fork si crea un nuovo processo che continua ad eseguire esattamente
    dalla linea in cui ho chiamato fork()? In tal caso 'if (pid == 0)' sara'
    vero solo nel codice eseguito dal figlio mentre l'else in quello eseguito
    dal padre. Mi pare sensata come cosa siccome anche il PC dovrebbe essere copiato. */
    if (pid == 0)
    {
        if (execvp(args[0], args) == -1) // Cambio l'immagine al figlio e controllo possibili errori
        {
            perror(args[0]);
            // printf("Args Passed:\n");
            // for (; *args != NULL; args++)
            // {
            //     printf("|%s|\n", *args);
            // }
            exit(EXIT_FAILURE); // TODO: come si uccide il figlio se l'exec ha fallito? Cosi' dovrebbe bastare...
        }
    }

    return 0;
}