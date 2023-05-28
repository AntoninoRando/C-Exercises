#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include "sorter.h"

typedef struct file_node // NON SO PERCHE' MA SE NON SCRIVO QUI FILE_NODE MI DA ERRORE
{
    char *name;
    char *dir_full_path;
    struct stat stat;
    struct file_node *next;
    struct file_node *prev;
} file_node;

static char *_get_full_path(const char *path, const char *f_name)
{
    char *full_path;
    full_path = (char *)malloc(strlen(path) + strlen(f_name) + 2); // strlen of full_path + strlen of f_name + '/' + '\0'
    strcpy(full_path, path);
    strcat(full_path, "/");
    strcat(full_path, f_name);
    return full_path;
}

int fill_list(struct file_node **head, const char *path, unsigned short flags)
{
    DIR *dir;

    if ((dir = opendir(path)) == NULL)
    {
        printf("%s  [error opening dir]\n", path);
        closedir(dir);
        return 1;
    }

    struct file_node *tail = NULL;
    struct dirent *ent;

    while ((ent = readdir(dir)) != NULL)
    {
        if (strcmp(".", ent->d_name) == 0 || strcmp("..", ent->d_name) == 0) // Skip this and previous dir.
        {
            continue;
        }

        if (ent->d_name[0] == '.' && !(flags >> 3 & 1)) // Skip private files if -a is not set.
        {
            continue;
        }

        char *full_path = _get_full_path(path, ent->d_name);
        struct stat f_stat;

        if (stat(full_path, &f_stat) != 0)
        {
            printf("%s  [error opening dir]\n", ent->d_name);
            closedir(dir);
            return 1;
        }

        if (!S_ISDIR(f_stat.st_mode) && flags >> 4 & 1) // Skip if not a dir and -d is set.
        {
            continue;
        }

        char *name_copy = malloc(strlen(full_path) + 1);
        strcpy(name_copy, flags >> 5 & 1 ? full_path : ent->d_name); // If -f is set, the name is the full_path.
        struct file_node *this_node = malloc(sizeof(file_node));
        this_node->name = name_copy;
        this_node->stat = f_stat;

        if (S_ISDIR(f_stat.st_mode))
        {
            this_node->dir_full_path = full_path;
        }
        else
        {
            this_node->dir_full_path = NULL;
            free(full_path);
        }

        _insert(head, &tail, this_node, flags);
    }

    closedir(dir);
    return 0;
}

static void _insert(struct file_node **head, struct file_node **tail, struct file_node *node, unsigned short flags)
{
    // Replace the head if there is no head.
    if (*head == NULL)
    {
        node->next = NULL;
        node->prev = NULL;
        *head = node;
        *tail = node;
        return;
    }

    // First, insert at the bottom.
    (*tail)->next = node;
    node->next = NULL;
    node->prev = *tail;

    int tail_remains = 0; // Does the tail remains the same or does it changes?

    if (_check_position(node, flags) == 0)
    {
        _move_up(node);
        tail_remains = 1; // As soon as the last node move up, the old tail come back.
    }
    while (_check_position(node, flags) == 0)
    {
        _move_up(node);
    }

    // Change the head and the tail if needed.
    if (node->prev == NULL)
    {
        *head = node;
    }

    if (tail_remains == 0)
    {
        *tail = node; // Change the tail.
    }
}

static void _move_up(struct file_node *node)
{
    if (node == NULL || node->prev == NULL)
    {
        return;
    }

    struct file_node *prev_prev = node->prev->prev;
    struct file_node *prev = node->prev;
    struct file_node *next = node->next;

    node->next = prev;
    node->prev = prev_prev;

    prev->next = next;
    prev->prev = node;

    if (prev_prev != NULL)
    {
        prev_prev->next = node;
    }

    if (next != NULL)
    {
        next->prev = prev;
    }
}

static int _check_position(struct file_node *node, unsigned short flags)
{
    // First node: ok
    if (node->prev == NULL)
    {
        return 1;
    }

    if (flags >> 2 & 1) // --dirsfirst
    {
        // node is dir BUT node->prev isn't: not ok
        if (node->dir_full_path != NULL && node->prev->dir_full_path == NULL)
        {
            return 0;
        }
        // node is not dir BUT node->prev is: ok BUT it cannot move, thus the sorting end.
        if (node->dir_full_path == NULL && node->prev->dir_full_path != NULL)
        {
            return 1;
        }
        // Remaining cases: 1) both dir 2) both non-dir. PARITY, the sorting continues.
    }

    if (flags >> 12) // -t
    {
        // node->prev is younger: not ok
        if (node->stat.st_mtime > node->prev->stat.st_mtime)
        {
            return 0;
        }
        // node->prev is older: ok
        if (node->stat.st_mtime < node->prev->stat.st_mtime)
        {
            return 1;
        }
        // Remaining cases: same time. PARITY, the sorting continues.
    }

    // -r
    int alphabetical = strcasecmp(node->prev->name, node->name);
    return flags >> 11 & 1 ? alphabetical > 0 : alphabetical < 0;
}