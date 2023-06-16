#define OVERFLOW 1
/**
 * @brief End Of Input: the input ended with a new line character.
 */
#define EOI 0
#define INPUT_SIZE 512

#define CMD_DIVS ";\n"
#define ARG_DIVS " \t"

/**
 * @brief Read a line from the input strean and put it inside dest. dest will include multiple lines if there were opened quotes.

 */
int read_line(char dest[INPUT_SIZE], FILE *__restrict__ __stream);

/**
 * @brief Execute all command contained inside the input. If one of those command is quit, set quit to 1.
 */
void execute_input(char *input, int *quit);

/**
 * @brief Execute a single commands.
 */
static int execute_cmd(char **args);
