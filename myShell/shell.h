int shell_loop();

// Execute the shell file (.sh) specified by path. Returns 1 on error and 0 on success.
int execute_bash(char *path);