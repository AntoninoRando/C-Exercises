#define PROMPT "\033[1;31mshell $\033[0m"

int shell_loop();

// Execute the shell file (.sh) specified by path. Returns 1 on error and 0 on success.
int execute_bash(char *path);