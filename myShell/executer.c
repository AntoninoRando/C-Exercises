#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "executer.h"

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

void *parse_line(char *input, int *quit)
{
    for (char* cmd = strtok(input, ";"); cmd != NULL; cmd = strtok(NULL, ";"))
    {
        if (strcmp(cmd, "quit") == 0)
        {
            *quit = 1;
            continue;
        }
        char **args = parse_cmd(cmd);
        execute_cmd(args);
    }
}

char **parse_cmd(char *cmd)
{
    char **args = malloc(64 * sizeof(char *)); // TODO: cambiare quel 64 in qualcosa di variabile
    int i = 0;
    for (char *tok = strtok(cmd, " \t\r\n\a"); tok != NULL; tok = strtok(NULL, " \t\r\n\a"))
    {
        args[i++] = tok;
    }

    return args;
}

static int execute_cmd(char **args)
{
    pid_t pid = fork();

    // error
    if (pid == -1)
    {
        perror("fork failed"); // TODO: non so se vada bene come messaggio.
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
        // Cambio l'immagine al figlio cosi' da eseguire il comando
        int error = execvp(args[0], args); 
        if (error == -1)
        {
            perror("exec failed");
            exit(EXIT_FAILURE); // TODO: come si uccide il figlio se l'exec ha fallito? Cosi' dovrebbe bastare...
        }

        // TODO: capire quale exec usare siccome questo qua sotto non funziona ma lo preferisco.
        // execl("bin", "sh", "-c", "pwd", NULL);
        // TODO: va termiato necessariamente con NULL? https://stackoverflow.com/questions/2407605/c-warning-missing-sentinel-in-function-call
    }

    return 0;
}