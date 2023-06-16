#define OVERFLOW 1
#define EOI 0 // End Of Input
#define INPUT_SIZE 512

#define CMD_DIVS ";\n"
#define ARG_DIVS " \t"


/**
 * @brief Read the input typed by the user in the terminal and put it inside dest.
 *
 * @param dest The string that will contain the user input.
 * @return int A value describing the input.
 */
int read_line(char dest[INPUT_SIZE], FILE *__restrict__ __stream); // TODO: ricontrollare se va bene l'array size qui

void execute_input(char *input, int *quit);

static int execute_cmd(char **args);
