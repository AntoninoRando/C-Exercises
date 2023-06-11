#include <stdlib.h>
#include "inputReader.c"

#define COMMAND_LIMIT 512

int shell()
{
    int quit = 0;
    char command[COMMAND_LIMIT];

    int commandCheck = read_line(command, COMMAND_LIMIT);

    if (commandCheck == OVERFLOW)
    {
        printf("shell: too many characters in the input: only %d are allowed\n", COMMAND_LIMIT-2);
        return 1;
    }

    system(command);
    return 0;
}

int check_if_bash(int argc, char **argv) {

}

int main(int argc, char **argv)
{
    return shell();
}