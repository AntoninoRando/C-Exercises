#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include "executer.h"
#include "utils.c"

int read_line(char dest[INPUT_SIZE])
{
    // if (fgets(dest, INPUT_SIZE, stdin) == NULL)
    // {
    //     return EOF;
    // }

    int allocated = 0, eof = EOI;

    do
    {
        // TODO: RICONTROLLARE QUANTO FATTO QUI SOTTO CON L'APPEND DELLA STRINGA
        // ad esempio se scrivo "Ciao\nproviamo\n\" e speriamo bene MI DA ERRORE
        int size = INPUT_SIZE - allocated;
        char append[size];

        if (fgets(append, size, stdin) == NULL)
        {
            eof = EOF;
            break;
        }

        memcpy(&dest[allocated], append, size);
        allocated = strlen(dest);

        // Limit exided means no new-line at the end.
        if (dest[allocated - 1] != '\n')
        {
            return OVERFLOW; // TODO: NON SO SE VADA FLUSSHATO L'EXTRA
        }

    } while (check_quotes(dest, allocated) != 0 || eof == EOF || !feof(stdin));
    /* TODO: senza l'aggiunta del !feof(stdin) con il file "quotes.sh" che ha 
    le quotes aperte alla fine non temina mai di printare. Deduco sia dovuto
    al fatto che non sempre c'e' l'EOF, ma non ho capito bene come funziona 
    questa cosa. */

    // Trim trailing new-line characters
    while (allocated > 0 && dest[allocated - 1] == '\n')
    {
        dest[allocated - 1] = '\0';
        allocated--;
    }

    return eof;
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

    // TODO: RICORDA DI FARE FREE OVUNQUE HO SCRITTO MALLOC
    // Il primo carattere va fatto manualmente perche' nel ciclo si accede all'(i-1)-esimo elemento
    char **args = malloc(1 * sizeof(char *));

    int argc = 0;
    int arg_len = 1;                                    // Conta il primo carattere
    int arg_start = strchr(DIVIDERS, input[0]) != NULL; // Inizia a contare 1 oltre se il carattere 1 e' un separatore
    int quotes = (input[0] == '"');                     // Aggiungi una quote se e' al primo carattere
    int spaces = 0;

    for (int i = 1; i < inputLen; i++)
    {
        arg_len++;

        // Quote found (ignore \")
        if (input[i] == '"' && input[i - 1] != '\\')
        {
            quotes++;
        }

        if (input[i] == ' ')
        {
            // Ignore following spaces
            if (spaces)
            {
                continue;
            }
            spaces++;
        }
        else
        {
            spaces = 0;
        }

        int last = (i == inputLen - 1);                      // This char is the last?
        int closedQuotes = (quotes % 2) == 0;                // Quotes are closed?
        int charIsDiv = strchr(DIVIDERS, input[i]) != NULL;  // This char is a divider?
        int cmdEnd = last || (closedQuotes && charIsDiv);    // Commands finished?
        int argEnd = (closedQuotes && spaces > 0) || cmdEnd; // Arg finished?

        if (argEnd)
        {
            arg_len = arg_len - charIsDiv - spaces; // Remove from the arg spaces and/or dividers
            args[argc] = malloc(arg_len + 1);
            memcpy(args[argc], &input[arg_start], arg_len); // Lo spazio, in caso ci sia, si puo' anche non togliere
            args[argc][arg_len] = '\0';
            argc++;
            arg_start = i + 1;
            arg_len = 0;
            args = realloc(args, (argc + 1) * sizeof(char *)); // Make space for new char.
        }

        if (cmdEnd)
        {
            execute_cmd(args);
            free(args);
            args = malloc(1 * sizeof(char *));
            argc = 0;
        }

        // ********************
        // EXECUTE COMMAND
        // int check = special_check(cmd);
        // if (check != 0) // EMPTY or QUIT
        // {
        //     if (check == QUIT)
        //     {
        //         *quit = 1;
        //     }
        //     continue;
        // }
        // ********************
    }

    free(args);
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

char **parse_command(char *cmd)
{
    // Trim leading spaces
    while (isspace((unsigned char)*cmd))
    {
        cmd++;
    }

    char divider = ' '; // TODO: ci vanno gli altri white-space characters che fungono da dividers?
    int j = 0, sub_start = 0, sub_len = 0;

    // TODO: RICORDA DI FARE FREE OVUNQUE HO SCRITTO MALLOC
    char **args = malloc(1 * sizeof(char *));
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
            memcpy(args[j], &cmd[sub_start], sub_len - 1); // ALTERNATIVA: strncpy(arg, cmd+sub_start, sub_len-1);
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
        if (execvp(args[0], args) == -1) // Cambio l'immagine al figlio e controllo possibili errori
        {
            perror("exec failed");
            exit(EXIT_FAILURE); // TODO: come si uccide il figlio se l'exec ha fallito? Cosi' dovrebbe bastare...
        }
    }

    return 0;
}