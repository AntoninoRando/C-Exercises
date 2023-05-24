#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "printer.c"
#include "walker.h"

typedef struct file_node // NON SO PERCHE' MA SE NON SCRIVO QUI FILE_NODE MI DA ERRORE
{
    char *name;
    struct stat stat;
    struct file_node *next;
} file_node;

/***********
 * Funzioni di Utility
 ***********/

// LE FUNZIONI STATIC NON DOVREBBERO ESSERE VISIBILI ANCHE ALL'ESTERNO.
// Uso const davanti al parametro perche' e' un riferimento ma la funzione non deve modificare il valore.
static int _treeR(int level, const char *path, int *dir_count, int *file_count, unsigned short flags, unsigned int level_mask, int max_level)
{
    DIR *dir;

    if ((dir = opendir(path)) == NULL)
    {
        perror(path);
        return 1; // ATTENZIONE, VA ritornato PURE SULLA RICORSIONE
    }

    struct file_node *head = NULL, *last_node = NULL;
    struct dirent *ent;

    while ((ent = readdir(dir)) != NULL)
    {
        if (strcmp(".", ent->d_name) == 0 || strcmp("..", ent->d_name) == 0)
            continue;

        if (ent->d_name[0] == '.' && !(flags >> 3 & 1))
            continue;

        char *full_path = _get_full_path(path, ent->d_name);

        struct stat f_stat;

        if (stat(full_path, &f_stat) != 0)
        {
            perror("An error has occurred while trying to read file's informations");
            return 1;
        }

        long int current_index = telldir(dir);
        if (readdir(dir) == NULL && head == NULL)
        {
            // Se la head non e' null, sebbene questa dir sia l'ultima, non verra stampata per ultima
            // perche' bisogna ancora stampare i files.
            level_mask |= (1 << level);
        }
        seekdir(dir, current_index);

        // Se il file e' una DIRECTORY
        if (S_ISDIR(f_stat.st_mode) == 1)
        {
            (*dir_count)++;

            // Stampa il full path se "-f"
            _print_name(level, flags >> 5 & 1 ? full_path : ent->d_name, level_mask, flags, f_stat);
            if (max_level == 0 || level + 1 < max_level)
            {
                _treeR(level + 1, full_path, dir_count, file_count, flags, level_mask, max_level);
            }
        }
        // Se non e' dir, stampa solo se non e' -d
        else if (!(flags >> 4 & 1))
        {
            (*file_count)++;

            if (!(flags >> 2 & 1)) // NO --dirsfirsat
            {
                _print_name(level, flags >> 5 & 1 ? full_path : ent->d_name, level_mask, flags, f_stat);
            }
            else
            {
                struct file_node *this_node = malloc(sizeof(file_node));

                char *name_copy = malloc(strlen(full_path) + 1);
                strcpy(name_copy, flags >> 5 & 1 ? full_path : ent->d_name);
                this_node->name = name_copy;
                this_node->stat = f_stat;
                this_node->next = NULL;

                if (head == NULL)
                {
                    head = this_node;
                }

                if (last_node != NULL)
                    last_node->next = this_node;
                last_node = this_node;

                // NO free(full_path);, ci pensera' dopo
            }
        }
        free(full_path);
    }

    while (head != NULL && head->next != NULL)
    {
        _print_name(level, head->name, level_mask, flags, head->stat);

        struct file_node *old_head = head;
        head = head->next;
        free(old_head); // NON SO SE IN AUTOMATICO LIBERA TUTTI I SUOI CAMPI, o se devo fare free(old_head->name);
    }
    if (head != NULL)
    {
        level_mask |= (1 << level);
        _print_name(level, head->name, level_mask, flags, head->stat);
        free(head);
    }

    closedir(dir);
    return 0;
}
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

int _pars_argv(int argc, char **argv, unsigned short *flags, char *path, int *max_level)
{
    // Parsing arguments
    int path_priority = 0;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--help") == 0)
        {
            *(flags) |= 1 << 0;
            return 0; // Se c'e' help gli altri argomenti sono inutili.
        }
        else if (strcmp(argv[i], "--inodes") == 0)
        {
            *(flags) |= 1 << 1;
            continue;
        }
        else if (strcmp(argv[i], "--dirsfirst") == 0)
        {
            *(flags) |= 1 << 2;
            continue;
        }
        else if (strcmp(argv[i], "--") == 0) // l'argomento era proprio --, quindi l'argomento dopo è un path
        {
            if (i + 1 == argc) // se e' stato passato "--" come ultimo argomento...
                return 0;      // ... ignora e finisci qui, visto che abbiamo finito gli argomenti...
            i++;
            strcpy(path, argv[i]); // ... altrimenti quello che viene dopo e' un path.
            continue;
        }
        else if (strcmp(argv[i], "-L") == 0)
        {
            if (i + 1 == argc)
            {
                printf("tree: Missing argument to -L option.");
                return 1;
            }
            i++;
            int level = atoi(argv[i]); // Se non si puo' convertire ritorna 0.
            if (level < 1)
            {
                printf("tree: Invalid level, must be greater than 0.");
                return 1;
            }
            *max_level = level;
            continue;
        }

        int lines = 0;                // numero di trattini iniziali
        while (argv[i][lines] == '-') // se e' finita la stringa incontra '\0' che e' diverso da '-' e termina il cilo.
            lines++;

        if (lines == 0) // e' un path quello che e' stato passato
            strcpy(path, argv[i]);
        else if (lines == 1) // e' un argomento, o singolo o su piu' righe
        {
            int c;
            for (int c = 1; c < strlen(argv[i]); c++)
            {
                int c2;
                for (c2 = 0; c2 < 11; c2++)
                {
                    if (argv[i][c] != "adfpsugDrt"[c2])
                        continue;
                    *(flags) |= 1 << (c2 + 3); // i primi 3 sono help, inodes e dirsfirst
                    break;
                }
                if (c2 == 11) // Se avesse trovato un argomento valido il ciclo sarebbe finito con un break (prima di c2 = 11)
                {
                    printf("here i = %d, n = %d\ntree: Invalid argument -\'%c\'.", i, argc, argv[i][c]);
                    return 1;
                }
            }
        }
        else // siccome gli argomenti con piu' lines li abbiamo gia' controllati, questo che ne ha almeno 2 deve essere invalido.
        {
            printf("tree: Invalid argument \'%s\'.", argv[i]);
            return 1;
        }
    }
    return 0;
}


/***********
 * Core Function
 ***********/
int tree(int argc, char **argv)
{
    // In linux la massima lunghezza di un file dovrebbe essere 255 byte.
    // l'array e' di 256 considerando il char finale (credo, ma in struct dirent il nome e'
    // un array di 256 quindi pure io ho fatto cosi').
    char path[256] = "."; // Di base e' questa directory, ma quando parsa gli argv potrebbe cambiare
    int dir_count = 0, file_count = 0;
    // NON SONO SICURO SE VADA USATO UN UNSIGNED SHORT
    unsigned short flags = 0b0; // 16 bit: --help --inodes --dirsfirst -adfpsugDrt. l'-L bit non ci interessa perche' quello se e' stato dato si riconosce dal valore di max_level.
    int max_level = 0;
    if (_pars_argv(argc, argv, &flags, path, &max_level) == 1) // Se gli argomenti sono stati passati male.
        return 1;

    // Se il comando e' help.
    if ((flags >> 0) & 1)
    {
        printf("Documentazione");
        return 0;
    }
    else
    {
        printf("%s\n", path); // printa la directory iniziale su cui si scorre
        int errOrNot = _treeR(0, path, &dir_count, &file_count, flags, 0b0, max_level);
        printf("\n%d directories, %d files", dir_count, file_count);
        return errOrNot;
    }
}