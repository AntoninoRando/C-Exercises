#define PROMPT "\033[1;31mshell $\033[0m "

/**
 * @brief Execute a shell reading the lines from the standard input.
 */
int shell_loop();

/**
 * @brief Execute the bash file (.sh) specified by path. Returns 1 on error and 0 on success.
 */
int bash_loop(char *path);