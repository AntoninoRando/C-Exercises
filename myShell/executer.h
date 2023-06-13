#define BLANK 0
#define OVERFLOW -1
#define OK 1
#define INPUT_SIZE 512

#define EMPTY 2
#define QUIT 3


// TODO: ci dovrebbe essere la funzione di libreria "readline"
// (https://man7.org/linux/man-pages/man3/readline.3.html)
// ma non me la fa usare perche mi trovo su docker. Per usarla dovre 
// farte "unminimize" del container.
int read_line(char dest[INPUT_SIZE], size_t limit); // TODO: ricontrollare se va bene l'array size qui

void *execute_line(char *input, int *quit);

int special_check(char* cmd);

static int execute_cmd(char **args);

char** manual_parse(char* cmd);