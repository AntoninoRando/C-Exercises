#include <stdlib.h>
#include "executer.c"
#include "shell.h"
// #include "memory.c"

int shell_loop()
{
    int quit = 0;

    while (quit != 1)
    {
        char line[INPUT_SIZE];

        int lineCheck = read_line(line, INPUT_SIZE);

        if (lineCheck != OK) // Blank or Overflow
        {
            if (lineCheck == OVERFLOW)
            {
                printf("shell: too many characters in the input: only %d are allowed\n", INPUT_SIZE - 2);
            }
            continue;
        }

        execute_line(line, &quit);
        while(wait(NULL) > 0); // Waits for all children process to end.
    }

    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
    // load_shell_chronology();
    return shell_loop();
}