# My Shell

## Shell Lifecycle

1. Initialize.
2. Interpret.
    1. Read.
    2. Parse.
    3. Execute.
3. Terminate.

## Executing Commands

1. `fork` syscall: creates a copy of this process;
2. `exec` syscall: change the copy image to be another process.
3. *After* forking *all* commands, the shell waits for all of them to finish.

## Parsing Input

If I used the `execl("/bin/sh", "/bin/sh", "-c", command, NULL)` I could avoid to manually parse the input line. I did not know if that function was allowed, so I used the `execvp` instead.

## Wrong Input

There are 2 types of wrong input:

- fixable: the shell ignores the bad input and compiles it;
- fatals: the shell doesn't execute the input and notify the user about the error.
For example:
- `shell command1;; command2` is a wrong typed input because of the two semicolons, but this error can easely be fixed removing one of them;
- `shell command1command2` can't be fixed as this is considere one single and non-exiting command.

In the second case:

- if the commands does not exist, this shell prints an error message;
- if the commands' arguments do not exist, the exec will print the error message.

## Miscellaneous

New line character in .sh file are treaten like instruction concatenation (;), unless they are inside quotes ("").

The shell has been design to cover border cases like:

- semicolon inside quotes (e.g. `echo "Hello; World"`);
- newline inside quotes (e.g. `echo "Hello World"`);

```Shell
echo "Hello
World"
```

- multiple semicolon;
- empty commands;
- quotes inside quotes (e.g. `echo "Hello \"World\""`);
- user opened quotes and then types enter.

To simplify, \ is always considered an escape character (to type \ simply type \\). Thus, it is not supported the \ ad the end of the input (which normally let the user type in a new line but do not insert the new-line character).

## Bash Mode

The bash mode differs from the interactive mode in two ways:

- in bash mode, the stream from which the lines are read is the bash file;
- the prompt is printend on the stout differently.

Multiple bash files can be passsed as arguments, each of them executes individually (in order).

## Shell Custom Commands

Shell custom commands like `cd` are not implemented, except for `quit`. Those commands could also make problem with the concurrency feature of this shell.
