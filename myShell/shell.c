#include <stdlib.h>
#include "executer.c"
#include "shell.h"

int shell_loop()
{
    int quit = 0;

    while (quit != 1)
    {
        char line[INPUT_SIZE];

        // PROMPT
        printf("%s ", PROMPT);

        int lineCheck = read_line(line);

        if (lineCheck != OK) // Blank or Overflow
        {
            if (lineCheck == OVERFLOW)
            {
                printf("shell: too many characters in the input: only %d are allowed\n", INPUT_SIZE);
            }
            continue;
        }

        execute_input(line, &quit);
        while (wait(NULL) > 0); // Waits for all children process to end.
    }

    exit(EXIT_SUCCESS);
}

// TODO: dovrei fare exit o return va bene?
int execute_bash(char *path)
{

    FILE *file = fopen(path, "r");

    if (file == NULL)
    {
        perror("fopen failed");
        fclose(file);
        return 1;
    }

    int j = strlen(path) - 1;
    // TODO: checks extension in a better way
    if (j <= 2 || (path[j - 2] != '.' && path[j - 1] != 's' && path[j] != 'h'))
    {
        printf("%s is not a bash file\n", path);
        fclose(file);
        return 1;
    }

    char *line = NULL;
    ssize_t len = 0;
    while ((getline(&line, &len, file)) != -1)
    {
        int quit = 0;
        
        if (len > INPUT_SIZE)
        {
            printf("shell: too many characters in the input: only %d are allowed\n", INPUT_SIZE);
            continue;
        }

        // If line does not end with new line, add a new line.
        printf("%s %s%s", PROMPT, line, line[strlen(line)-1] == '\n' ? "" : "\n");

        execute_input(line, &quit);

        while (wait(NULL) > 0); // Waits for all children process to end.

        if (quit == 1)
        {
            break;
        }
    }

    fclose(file);

    if (line)
    {
        free(line); 
    }
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
        errors += execute_bash(argv[i]);
    }

    return errors;
}