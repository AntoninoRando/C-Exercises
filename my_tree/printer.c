#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include "printer.h"

void print_name(int level, char *name, unsigned int level_mask, unsigned short arg_mask, struct stat f_stat)
{
    for (int i = 0; i < level; i++)
    {
        printf("%s   ", level_mask >> i & 1 ? " " : "│");
    }

    printf("%s── ", level_mask >> level & 1 ? "└" : "├");
    print_args(arg_mask, f_stat);
    print_colorized(name, f_stat);

    /* Use"chcp 65001" on VS Code terminal to visualize these chars.
    printf("\u251C\u2500\u2500\u2500");  Stampa "├──"
    printf("\u2502");                    Stampa "│"
    printf("\u2514\u2500\u2500\u2500");  Stampa "└──" */
}

void print_args(unsigned short arg_mask, struct stat f_stat)
{
    int brakets = (arg_mask >> 1 & 1) + (arg_mask >> 6 & 1) + (arg_mask >> 7 & 1) + (arg_mask >> 8 & 1) + (arg_mask >> 9 & 1) + (arg_mask >> 10 & 1);

    if (brakets > 0)
    {
        printf("[");
    }

    if (arg_mask >> 1 & 1) // --inodes
    {
        printf("%lu%s", f_stat.st_ino, brakets > 1 ? " " : "] ");
        brakets--;
    }
    if (arg_mask >> 6 & 1) // -p
    {
        _print_file_mode(f_stat);
        printf("%s", brakets > 1 ? " " : "] ");
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
        printf("%s%s", full_size, brakets > 1 ? " " : "] "); // If there are other args to print, leave a space.
        brakets--;
    }
    if (arg_mask >> 8 & 1) // -u
    {
        struct passwd *pw = getpwuid(f_stat.st_uid);
        if (pw == NULL) // No username available.
        {
            printf("UID %i", f_stat.st_uid);
        }
        else
        {
            printf("%s", pw->pw_name);
        }
        printf("%s", brakets > 1 ? " " : "] ");
        brakets--;
    }
    if (arg_mask >> 9 & 1) // -g
    {
        struct group *gr = getgrgid(f_stat.st_gid);
        if (gr == NULL)
        {
            printf("GID %i", f_stat.st_gid);
        }
        else
        {
            printf("%s", gr->gr_name);
        }
        printf("%s", brakets > 1 ? " " : "] ");
        brakets--;
    }
    if (arg_mask >> 10 & 1) // -D
    {
        char date[20];
        strftime(date, sizeof(date), "%B %d %H:%M", localtime(&(f_stat.st_mtime)));
        printf("%s%s", date, brakets > 1 ? " " : "] ");
        brakets--;
    }
}

void print_colorized(const char *name, struct stat f_stat)
{
    char *ls_colors = getenv("LS_COLORS");

    // If LS_COLORS is not set, print in the standard format.
    if (ls_colors == NULL)
    {
        printf("%s\n", name);
        return;
    }

    // Convert from file-type to LS_COLOR label.
    char file_type[3];
    unsigned int mode = f_stat.st_mode;
    if (S_ISREG(mode))
        strcpy(file_type, "fi"); // Regular File
    else if (S_ISDIR(mode))
        strcpy(file_type, "di"); // Directory
    else if (S_ISLNK(mode))
        strcpy(file_type, "ln"); // Symbolic Link
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
        char *key = strtok(token, "=");  // File_type
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

static void _print_file_mode(struct stat f_stat)
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

void print_help()
{
    printf("usage: tree [-adfgprstuD] [-L level [-R]]");
    printf(" [--inodes]\n[--dirsfirst] [--help]\n[--] [directory ...]\n");
    printf("  ------- Listing options -------\n  -a            All files ");
    printf("are listed.\n  -d            List directories only.\n  -f    ");
    printf("        Print the full path prefix for each file.\n  -L level");
    printf("      Descend only level directories deep.\n  ------- File options -------\n");
    printf("  -p            Print the protections for each file.\n  -u");
    printf("            Displays file owner or UID number.\n  -g");
    printf("            Displays file group owner or GID number.\n  -s");
    printf("            Print the size in bytes of each file.\n  -D");
    printf("            Print the date of last modification or status change");
    printf(".\n  --inodes      Print inode number of each file.\n  ------- Sorting options -------\n");
    printf("  -t            Sort files by last modification time.\n  -r");
    printf("            Reverse the order of the sort.\n  --dirsfirst");
    printf("   List directories before files.\n  ------- Miscellaneous options -------\n  --help");
    printf("        Print usage and this help message and exit.\n  --");
    printf("            Options processing terminator.\n");
}