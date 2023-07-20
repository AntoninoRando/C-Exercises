#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "walker.h"
#include "printer.c"
#include "sorter.c"

int tree(int argc, char **argv)
{
    // The max file length in linux is 255 (256 = 255 + 1 for the string terminator).
    char **paths;
    int dir_count = 0, file_count = 0, max_level = 0, paths_num = 0;
    unsigned short arg_mask = 0b0;

    if (_pars_argv(argc, argv, &arg_mask, &paths, &max_level, &paths_num) == 1)
    {
        print_help(); // Print help if args were wrong.
        return 1;
    }

    if (arg_mask >> 0 & 1) // --help
    {
        print_help();
        return 0;
    }

    // TODO: implement default path "." when no paths are given
    for (int i = 0; i < paths_num; i++)
    {
        struct stat f_stat;
        if (stat(paths[i], &f_stat) != 0)
        {
            printf("%s  [error opening dir]\n", paths[i]);
            return 1;
        }

        // Print the starting directory.
        print_args(arg_mask, f_stat);
        print_colorized(paths[i], f_stat);

        _treeR(0, paths[i], &dir_count, &file_count, arg_mask, 0b0, max_level);
        free(paths[i]);
    }
    free(paths);

    // Print the toal number of directories and files.
    if (dir_count > 0)
    {
        printf("\n%d directories%s", dir_count, file_count > 0 ? ", " : "\n");
    }
    if (file_count > 0)
    {
        printf("%d files\n", file_count);
    }

    return 0;
}

static int _treeR(int level, const char *path, int *dir_count, int *file_count, unsigned short flags, unsigned int arg_mask, int max_level)
{
    struct file_node *head = NULL;

    // Error while filling the list.
    if (fill_list(&head, path, flags) == 1)
    {
        return 1;
    }

    while (head != NULL)
    {
        // If this levels has come to an end.
        if (head->next == NULL)
        {
            arg_mask |= (1 << level);
        }

        print_name(level, head->name, arg_mask, flags, head->stat);

        // If the file is a dir.
        if (head->dir_full_path != NULL)
        {
            (*dir_count)++;
            if (max_level == 0 || level + 1 < max_level) // -L
            {
                _treeR(level + 1, head->dir_full_path, dir_count, file_count, flags, arg_mask, max_level);
            }
        }
        else
        {
            (*file_count)++;
        }

        struct file_node *old_head = head;
        head = head->next;
        free(old_head);
    }

    free(head);

    return 0;
}

static int _pars_argv(int argc, char **argv, unsigned short *arg_mask, char ***paths, int *max_level, int *paths_num)
{
    int last_path = 0;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--help") == 0)
        {
            *(arg_mask) |= 1 << 0;
            return 0; // If --help, other args are useless.
        }
        else if (strcmp(argv[i], "--inodes") == 0)
        {
            *(arg_mask) |= 1 << 1;
            continue;
        }
        else if (strcmp(argv[i], "--dirsfirst") == 0)
        {
            *(arg_mask) |= 1 << 2;
            continue;
        }
        else if (strcmp(argv[i], "--") == 0) // The following argument has to be a path...
        {
            // No other args, that is no path specified.
            if (i + 1 == argc)
            {
                continue;
            }

            // Get the following path.
            i++;

            // If the file length exceed the limit, ignore it.
            if (strlen(argv[i]) > 255)
            {
                return 0;
            }
            argv[last_path] = argv[i];
            last_path++;
            continue;
        }

        int lines = 0;
        while (argv[i][lines] == '-') // se e' finita la stringa incontra '\0' che e' diverso da '-' e termina il cilo.
        {
            lines++;
        }

        // No line = path
        if (lines == 0)
        {
            argv[last_path] = argv[i];
            last_path++;
        }
        else if (lines == 1) // Single arg.
        {
            int c, j = i;
            for (int c = 1; c < strlen(argv[j]); c++)
            {
                if (argv[j][c] == 'L')
                {
                    if (i + 1 == argc)
                    {
                        printf("tree: Missing argument to -L option.\n");
                        return 1;
                    }
                    i++;
                    int level = atoi(argv[i]); // Se non si puo' convertire ritorna 0, che comunque non va bene.
                    if (level < 1)
                    {
                        printf("tree: Invalid level, must be greater than 0.\n");
                        return 1;
                    }
                    *max_level = level;
                    continue;
                }

                int c2;
                for (c2 = 0; c2 < 11; c2++)
                {
                    if (argv[j][c] != "adfpsugDrt"[c2])
                        continue;
                    *(arg_mask) |= 1 << (c2 + 3); // i primi 3 sono help, inodes e dirsfirst
                    break;
                }
                if (c2 == 11) // Se avesse trovato un argomento valido il ciclo sarebbe finito con un break (prima di c2 = 11)
                {
                    printf("here i = %d, n = %d\ntree: Invalid argument -\'%c\'.\n", i, argc, argv[i][c]);
                    return 1;
                }
            }
        }
        else // Since we've alreay check arguments with 2 lines, this must be a wrong argument.
        {
            printf("tree: Invalid argument \'%s\'.\n", argv[i]);
            return 1;
        }
    }
    
    *paths_num = last_path > 0 ? last_path : 1;

    // User gave no path, so use '.' by default
    (*paths) = malloc(*paths_num * sizeof(char *));

    if (last_path == 0)
    {
        (*paths)[0] = malloc(2);
        strcpy((*paths)[0], ".");
        return 0;
    }

    for (int i = 0; i < last_path; i++)
    {
        (*paths)[i] = malloc(strlen(argv[i]) + 1);
        strcpy((*paths)[i], argv[i]);
    }
    return 0;
}
