#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
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

void *execute_line(char *input, int *quit)
{
    for (char *cmd = strtok(input, ";"); cmd != NULL; cmd = strtok(NULL, ";"))
    {

        int check = special_check(cmd);
        if (check != 0) // EMPTY or QUIT
        {
            if (check == QUIT)
            {
                *quit = 1;
            }
            continue;
        }

        char **args = manual_parse(cmd);
        execute_cmd(args);
    }
}

int special_check(char *cmd)
{
    int end = strlen(cmd);

    // Trim leading spaces
    // TODO: questa operazione modifica la stringa, ma non dovrebbe creare
    // problemi per altri comandi visto che leva solo gli spazi iniziali.
    while (isspace((unsigned char)*cmd))
    {
        cmd++;
        end--;
    }

    // All spaces
    if (end == 0)
    {
        return EMPTY;
    }

    char quit[] = "quit";

    int i = 0;
    while (end != 0)
    {
        end--;

        if (cmd[i] != quit[i])
        {
            return 0;
        }

        continue; // TODO: tornare qualcosa che sicuramente non e' ne QUIT ne EMPTY
    }

    return end == 0 ? QUIT : 0;
}

char **manual_parse(char *cmd)
{
    // Trim leading spaces
    while (isspace((unsigned char)*cmd))
    {
        cmd++;
    }

    // TODO: RICORDA DI FARE FREE OVUNQUE HO SCRITTO MALLOC
    char **args = malloc(1 * sizeof(char *));
    char divider = ' '; // TODO: aggiungere altri white-space characters
    int j = 0, sub_start = 0, sub_len = 0;

    for (int i = 0; i <= strlen(cmd); i++)
    {
        char c = cmd[i];
        sub_len++;

        if (c == divider || i == strlen(cmd)) // Arg ended
        {
            // Reset divider if it changed
            if (divider == '"')
            {
                divider = ' ';
                sub_len--;
            }
            
            args[j] = malloc(sub_len);
            // TODO: il -1 toglie lo spazio finale
            memcpy(args[j], &cmd[sub_start], sub_len-1); // ALTERNATIVA: strncpy(arg, cmd+sub_start, sub_len-1);
            // TODO: il null terminator non dovrebbe servire... (perche' non viene aggiunto)
            sub_len = 0;
            sub_start = i + 1;
            j++;
            args = realloc(args, (j + 1) * sizeof(char *)); // Make space for new char.
            continue;
        }

        if (c == '"') // Encountered an arg beginning with "
        {
            divider = '"';
            sub_start++;
            continue;
        }
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
        // int error = execl("/bin/sh", "/bin/sh", "-c", cmd, NULL); // TODO: will this always work?
        // TODO: sostituire con la versione vecchia execvp(args[0], args) cosi' che implemento
        // pure il parsing della stringa.
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