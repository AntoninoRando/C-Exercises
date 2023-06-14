# Shell Lifecycle
1. Initialize.
2. Interpret.
    1. Read.
    2. Parse.
    3. Execute.
3. Terminate.

# Executing Commands
1. `fork` syscall: creates a copy of this process;
2. `exec` syscall: change the copy image to be another process.
3. *After* forking *all* commands, the shell waits for all of them to finish.

# Wrong Input
There are 2 types of wrong input:
- fixable: the shell ignores the bad input and compiles it;
- fatals: the shell doesn't execute the input and notify the user about the error.
For example:
- `shell command1;; command2` is a wrong typed input because of the two semicolons, but this error can easely be fixed removing one of them;
- `shell command1command2` can't be fixed as this is considere one single and non-exiting command.

# Miscellaneous
- If multiple arguments are given, they are treaten like multiple .sh files.
- New line character in .sh file are treaten like instruction concatenation (;), unless they are inside quotes ("").