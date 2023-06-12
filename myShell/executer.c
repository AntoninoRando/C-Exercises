#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "executer.h"

static int execute_commands(struct Commands *commands)
{
    for (; commands != NULL; commands = commands->next)
    {
        execute_cmd(commands->args);
        // free(head);
    }
}

static int execute_cmd(char **args)
{
    pid_t pid;

    pid = fork();

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
        int error = execvp(args[0], args); // TODO: va controllato l'errore?
        if (error == -1)
        {
            perror("exec failed");
            exit(EXIT_FAILURE);
        }
        // TODO: come si uccide il figlio se l'exec ha fallito?

        // TODO: capire quale exec usare siccome questo qua sotto non funziona ma lo preferisco.
        //execl("bin", "sh", "-c", "pwd", NULL);
        // TODO: va termiato necessariamente con NULL? https://stackoverflow.com/questions/2407605/c-warning-missing-sentinel-in-function-call
    }

    return 0;
}