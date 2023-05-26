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

    struct file_node *last_node = NULL;
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
        this_node->next = NULL;

        if (S_ISDIR(f_stat.st_mode))
        {
            this_node->dir_full_path = full_path;
            if (flags >> 2 & 1)
            {
                _insert_by_dir(head, this_node);
                continue;
            }
        }
        else
        {
            this_node->dir_full_path = NULL;
            free(full_path);
        }
        _insert(head, &last_node, this_node);
    }

    closedir(dir);
    return 0;
}

static void _insert_by_dir(struct file_node **head, struct file_node *dir_node)
{
    if (*head == NULL)
    {
        *head = dir_node;
        return;
    }

    // Alla prima dir incontrata la head cambia, quindi non c'e' bisogno di farla scorrere.
    if ((*head)->dir_full_path == NULL) {
        dir_node->next = *head;
        *head = dir_node;
        return;
    }

    struct file_node *iterator = *head;
    while (iterator->next != NULL && iterator->next->dir_full_path != NULL)
    {
        iterator = iterator->next;
    }

    dir_node->next = iterator->next;
    iterator->next = dir_node;
}

static void _insert(struct file_node **head, struct file_node **last_node, struct file_node *node)
{
    if (*head == NULL)
    {
        *head = node;
    }

    if (*last_node != NULL)
    {
        (*last_node)->next = node;
    }

    *last_node = node;
}