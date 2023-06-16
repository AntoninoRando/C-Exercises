#define OVERFLOW 1
#define EOI 0 // End Of Input
#define INPUT_SIZE 512

#define EMPTY 2
#define QUIT 3

#define DIVIDERS ";\n"


// TODO: ci dovrebbe essere la funzione di libreria "readline"
// (https://man7.org/linux/man-pages/man3/readline.3.html)
// ma non me la fa usare perche mi trovo su docker. Per usarla dovre 
// farte "unminimize" del container.

/**
 * @brief Read the input typed by the user in the terminal and put it inside dest.
 * 
 * @param dest The string that will contain the user input.
 * @return int A value describing the input.
 */
int read_line(char dest[INPUT_SIZE]); // TODO: ricontrollare se va bene l'array size qui

void execute_input(char *input, int *quit);

int special_check(char* cmd);

static int execute_cmd(char **args);

char** parse_command(char* cmd);