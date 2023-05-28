#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "linkedListUtils.h"

typedef struct file_node // NON SO PERCHE' MA SE NON SCRIVO QUI FILE_NODE MI DA ERRORE
{
    char *name;
    char *dir_full_path;
    time_t date;
    struct stat stat;
    struct file_node *next;
    struct file_node *prev;
} file_node;

static char *_get_full_path(const char *path, const char *f_name)
{
    char *full_path;
    full_path = (char *)malloc(strlen(path) + strlen(f_name) + 2);
    // quell'2 e' per la stringa '/' ed il null terminator '/0
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
        perror(path);
        closedir(dir);
        return 1; // ATTENZIONE, VA ritornato PURE SULLA RICORSIONE
    }

    struct file_node *tail = NULL;
    struct dirent *ent;

    while ((ent = readdir(dir)) != NULL)
    {
        if (strcmp(".", ent->d_name) == 0 || strcmp("..", ent->d_name) == 0)
            continue;

        if (ent->d_name[0] == '.' && !(flags >> 3 & 1)) // Privata e non -a.
            continue;

        char *full_path = _get_full_path(path, ent->d_name);

        struct stat f_stat;

        if (stat(full_path, &f_stat) != 0)
        {
            perror("An error has occurred while trying to read file's informations");
            closedir(dir);
            return 1;
        }

        if (!S_ISDIR(f_stat.st_mode) && flags >> 4 & 1) // Non dir e -d.
        {
            continue;
        }

        char *name_copy = malloc(strlen(full_path) + 1);
        strcpy(name_copy, flags >> 5 & 1 ? full_path : ent->d_name); // Se -f il nome sara' il full path.

        struct file_node *this_node = malloc(sizeof(file_node));
        this_node->name = name_copy;
        this_node->stat = f_stat;
        this_node->date = f_stat.st_mtime;
        this_node->dir_full_path = S_ISDIR(f_stat.st_mode) ? full_path : NULL; // DUBBIO: free(full_path) se non dir?

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

    int tail_remains = 0;

    if (_is_position_ok(node, flags) == 0)
    {
        _move_up(node);
        tail_remains = 1;
    }
    while (_is_position_ok(node, flags) == 0)
    {
        _move_up(node);
    }
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

    // free VA FATTO?
}

// Controlla se node (non NULL) e' in una posizione VALIDA per --dirsfirst
static int _respect_dirsfirst(struct file_node *node)
{
    // Primo nodo: va bene
    if (node->prev == NULL)
    {
        return 1;
    }

    // E' una dir ma quello prima no: non va bene, deve salire.
    if (node->dir_full_path != NULL && node->prev->dir_full_path == NULL)
    {
        return 0;
    }

    // Non dir ma quello prima si': va bene ma non puo' salire
    if (node->dir_full_path == NULL && node->prev->dir_full_path != NULL)
    {
        return -1;
    }

    // Casi rimanenti: 1) entrambi dir 2) entrambi non dir.
    return 1;
}

static int _respect_r(struct file_node *node)
{
    // Primo nodo: va bene
    if (node->prev == NULL)
    {
        return 1;
    }

    return strcmp(node->name, node->prev->name) < 0 ? 1 : 0;
}

static int _respect_t(struct file_node *node)
{
    // Primo nodo: va bene
    if (node->prev == NULL)
    {
        return 1;
    }

    return node->date < node->prev->date;
}

static int _is_position_ok(struct file_node *node, unsigned short flags)
{
    if (flags >> 2 & 1)
    {
        int dirsfirst = _respect_dirsfirst(node);
        if (dirsfirst == 0)
        {
            return 0;
        }
        if (dirsfirst == -1)
        {
            return 1;
        }
    }

    if (flags >> 11 & 1)
    {
        return _respect_r(node);
    }

    if (flags >> 12)
    {
        return _respect_t(node);
    }

    return 1;
}