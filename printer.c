#include <sys/stat.h>
#include "printer.h"

void _print_file_mode(struct stat fileStat)
{
    printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
    printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
    printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
    printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
    printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
    printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
    printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
    printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");
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

    // Non uso gli unicodes per rendere piu' comprensibile il codice
    // visto che con gli operatori ternari potrebbe risultare confuso.

    for (int i = 0; i < level; i++)
        printf("%s    ", level_mask >> i & 1 ? " " : "│");

    int brakets = (arg_mask >> 1 & 1) + (arg_mask >> 6 & 1) + (arg_mask >> 7 & 1) + (arg_mask >> 10 & 1);
    int brakets_unmodified = brakets;

    printf("%s── %s", level_mask >> level & 1 ? "└" : "├", brakets > 0 ? "[" : "");

    if (arg_mask >> 1 & 1)
    {
        printf(" %d%s", f_stat.st_ino, brakets > 1 ? " " : "");
        brakets--;
    }
    if (arg_mask >> 6 & 1)
    {
        _print_file_mode(f_stat);
        printf("%s", brakets > 1 ? " " : "");
        brakets--;
    }
    if (arg_mask >> 7 & 1)
    {
        char full_size[12] = "           ";
        char size[12];
        sprintf(size, "%lld", f_stat.st_size);
        for (int i = 0; i < strlen(size); i++)
        {
            full_size[10 - i] = size[i]; // La fine e' \0, quindi non va toccata
        }
        printf("%s%s", full_size, brakets > 1 ? " " : "");
        brakets--;
    }
    if (arg_mask >> 10 & 1)
    {
        char date[20];
        strftime(date, sizeof(date), "%d-%m-%y", localtime(&(f_stat.st_mtime)));
        printf("%s%s", date, brakets > 1 ? " " : "");
        brakets--;
    }

    printf("%s%s\n", brakets_unmodified > 0 ? "] " : "", name);

    // Usa i colori di LS_COLORS se e' settata la variabile
    // char *ls_colors = getenv("LS_COLORS");
    // if (ls_colors != NULL)
    //     printf("\u251C\u2500\u2500\u2500 \033[%sm%s\033[0m\n", ls_colors, name);
    // else

    // Se non scrivo sul terminale di vs code "chcp 65001" non visualizza questi chars
    // printf("\u251C\u2500\u2500\u2500");  Stampa "├──"
    // printf("\u2502");                    Stampa "│"
    // printf("\u2514\u2500\u2500\u2500");  Stampa "└──"
}