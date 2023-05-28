#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include "printer.h"

void _print_file_mode(struct stat f_stat)
{
    unsigned int mode = f_stat.st_mode;
    printf(S_ISDIR(mode) ? "d" : "-");
    printf(mode & S_IRUSR ? "r" : "-");
    printf(mode & S_IWUSR ? "w" : "-");
    printf(mode & S_IXUSR ? "x" : "-");
    printf(mode & S_IRGRP ? "r" : "-");
    printf(mode & S_IWGRP ? "w" : "-");
    printf(mode & S_IXGRP ? "x" : "-");
    printf(mode & S_IROTH ? "r" : "-");
    printf(mode & S_IWOTH ? "w" : "-");
    printf(mode & S_IXOTH ? "x" : "-");
}

static void _print_name(int level, char *name, unsigned int level_mask, unsigned short arg_mask, struct stat f_stat)
{
    /* La level_mask indica quali livelli hanno raggiunto la fine della dir.
    La level_mask non e' globale, ma e' associata ad un singolo file; cosi'
    che quando se la passa nelle cartelle ricorsive, si tiene il valore di prima, ma quando
    ritorna dalla ricorsione, resetta i livelli. Se lo passassi ogni volta per riferimento,
    quando un livello raggiunge la fine sopra, poi dovrei reimpostarlo per un livello ricorsivo
    che riraggiunge la fine sotto.
    Per farla breve: manitene le modifiche all'andata della ricorsione, non le mantiene al ritorno.
    */

    for (int i = 0; i < level; i++)
    {
        printf("%s   ", level_mask >> i & 1 ? " " : "│");
    }

    printf("%s── ", level_mask >> level & 1 ? "└" : "├");
    print_args(arg_mask, f_stat);
    print_colorized(name, f_stat);
    //printf("%s\n", name);

    /* Se non scrivo sul terminale di vs code "chcp 65001" non visualizza questi chars
    printf("\u251C\u2500\u2500\u2500");  Stampa "├──"
    printf("\u2502");                    Stampa "│"
    printf("\u2514\u2500\u2500\u2500");  Stampa "└──" */
}

void print_args(unsigned short arg_mask, struct stat f_stat)
{
    int brakets = (arg_mask >> 1 & 1) + (arg_mask >> 6 & 1) + (arg_mask >> 7 & 1) + (arg_mask >> 8 & 1) + (arg_mask >> 9 & 1) + (arg_mask >> 10 & 1);
    int brakets_unmodified = brakets;

    if (brakets > 0)
    {
        printf("[");
    }

    if (arg_mask >> 1 & 1) // --inodes
    {
        printf("%lu%s", f_stat.st_ino, brakets > 1 ? " " : "");
        brakets--;
    }
    if (arg_mask >> 6 & 1) // -p
    {
        _print_file_mode(f_stat);
        printf("%s", brakets > 1 ? " " : "");
        brakets--;
    }
    if (arg_mask >> 7 & 1) // -s
    {
        char full_size[12] = "           ";
        char size[12];
        sprintf(size, "%ld", f_stat.st_size);
        for (int i = 0; i < strlen(size); i++)
        {
            full_size[10 - i] = size[i]; // La fine e' \0, quindi non va toccata
        }
        printf("%s%s", full_size, brakets > 1 ? " " : "");
        brakets--;
    }
    if (arg_mask >> 8 & 1) // -u
    {
        struct passwd *pw = getpwuid(f_stat.st_uid);
        if (pw == 0)
        {
            printf("UID %i", f_stat.st_uid);
        }
        else
        {
            printf("%s", pw->pw_name);
        }
        printf("%s", brakets > 1 ? " " : "");
        brakets--;
    }
    if (arg_mask >> 9 & 1) // -g
    {
        struct group *gr = getgrgid(f_stat.st_gid);
        if (gr == 0)
        {
            printf("GID %i", f_stat.st_gid);
        }
        else
        {
            printf("%s", gr->gr_name);
        }
        printf("%s", brakets > 1 ? " " : "");
        brakets--;
    }
    if (arg_mask >> 10 & 1) // -D
    {
        char date[20];
        strftime(date, sizeof(date), "%d-%m-%y", localtime(&(f_stat.st_mtime)));
        printf("%s%s", date, brakets > 1 ? " " : "");
        brakets--;
    }

    if (brakets_unmodified > 0)
    {
        printf("] ");
    }
}

void print_colorized(const char *name, struct stat f_stat)
{
    char *ls_colors = getenv("LS_COLORS");

    if (ls_colors == NULL)
    {
        printf("%s\n", name);
        return;
    }

    // Determina il tipo di file
    char file_type[3];
    unsigned int mode = f_stat.st_mode;
    if (S_ISREG(mode))
        strcpy(file_type, "fi"); // File regolare
    else if (S_ISDIR(mode))
        strcpy(file_type, "di"); // Directory
    else if (S_ISLNK(mode))
        strcpy(file_type, "ln"); // Link simbolico
    else if (S_ISFIFO(mode))
        strcpy(file_type, "pi"); // FIFO
    // TODO AGGIUNGERE ALTRI TIPI


    // Parsing the LS_COLORS variable
    // char ls_copy[strlen(ls_colors) + 1];
    // strcpy(ls_copy, ls_colors);
    char *ls_copy = strdup(ls_colors);
    // strtok modifica la stringa. strtok_r puo tornare al contesto di prima, altrimenti lo strtok interno cambia il puntatore static all'ultima stringa passata.
    char *token = strtok_r(ls_copy, ":", &ls_copy); // Split each file_type=color1;color2;...:
    while (token != NULL)
    {
        char *key = strtok(token, "="); // File_type
        char *value = strtok(NULL, "="); // Colors
        // printf("\ntoken: %s;\tkey-value: %s-%s\n", token, key, value);
        if (strcmp(file_type, key) == 0)
        {
            // \033[ %s m %s \033[ 0 m \n  
            printf("\033[%sm%s\033[0m\n", value, name);
            return;
        }

        token = strtok_r(NULL, ":", &ls_copy); // Next color
    }

    printf("%s\n", name);
}