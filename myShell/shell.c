#include <stdlib.h>
#include <sys/wait.h>
#include "executer.c"
#include "shell.h"

int shell_loop()
{
    int quit = 0;
    while (quit != 1)
    {
        char line[INPUT_SIZE];

        printf("%s", PROMPT);
        fflush(stdout); // Ensure to print the prompt before buffer is full or new-line is entered

        int lineCheck = read_line(line, stdin);

        if (lineCheck == OVERFLOW)
        {
            printf("%stoo many characters in the input: only %d are allowed\n", PROMPT, INPUT_SIZE);
            continue;
        }

        execute_input(line, &quit);

        while (wait(NULL) > 0)
            ; // Waits for all children process to end.

        if (lineCheck == EOF)
        {
            break;
        }
    }

    return 0;
}

// TODO: dovrei fare exit o return va bene?
int bash_loop(char *path)
{
    FILE *file = fopen(path, "r");

    if (file == NULL)
    {
        perror("fopen failed");
        fclose(file);
        return 1;
    }

    int j = strlen(path) - 1;

    if (j <= 2 || (path[j - 2] != '.' && path[j - 1] != 's' && path[j] != 'h'))
    {
        printf("%s is not a bash file\n", path);
        fclose(file);
        return 1;
    }

    int quit = 0;
    while (quit != 1)
    {
        char line[INPUT_SIZE];

        int lineCheck = read_line(line, file);

        if (lineCheck == OVERFLOW)
        {
            printf("%stoo many characters in the input: only %d are allowed\n", PROMPT, INPUT_SIZE);
            continue;
        }
        
        // TODO: convertire in un controllo per solo linee vuote
        if (strlen(line) < 0)
        {
            continue;
        }

        printf("%s%s", PROMPT, line);
        fflush(stdout); // Ensure to print the prompt before buffer is full or new-line is entered

        execute_input(line, &quit);

        while (wait(NULL) > 0)
            ; // Waits for all children process to end.

        if (lineCheck == EOF)
        {
            break;
        }
    }

    fclose(file);
    return 0;
}

int main(int argc, char **argv)
{
    // Interactive mode
    if (argc == 1)
    {
        return shell_loop();
    }

    // Bash mode
    int errors = 0;

    for (int i = 1; i < argc; i++)
    {
        errors += bash_loop(argv[i]);
    }

    return errors;
}