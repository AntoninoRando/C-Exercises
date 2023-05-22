#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/***********
 * Prototipo delle Funzioni
 ***********/
// NON HO CAPITO A COSA SERVE DICHIARARE IL PROTOTIPO
static void _print_name(int, char *, unsigned int, unsigned short, struct stat);
static int _treeR(int, const char *, int *, int *, unsigned short, unsigned int); // passo char come puntatore perche' e' un array di carratteri, cioe' una stringa
static void _pars_argv(int, char **, unsigned short *, char *);
int tree(int, char **);

static const int HELP_OP = 0;
static const int A_OP = 1;
static const int BIGD_OP = 2;
static const int F_OP = 3;
static const int P_OP = 4;
static const int S_OP = 5;
static const int U_OP = 6;
static const int D_OP = 7;
static const int INODES_OP = 8;
static const int R_OP = 9;
static const int T_OP = 10;
static const int DIRSFIRST_OP = 11;
static const int BIGL_OP = 12;

/***********
 * Funzioni di Utility
 ***********/
// LE FUNZIONI STATIC NON DOVREBBERO ESSERE VISIBILI ANCHE ALL'ESTERNO.
// Uso const davanti al parametro perche' e' un riferimento ma la funzione non deve modificare il valore.
static int _treeR(int level, const char *path, int *dir_count, int *file_count, unsigned short flags, unsigned int level_mask)
{

    DIR *dir;

    dir = opendir(path); // Open the current directory
    if (dir == NULL)
    {
        char message[] = "An error has occurred while trying to open \"";
        strcat(message, path);
        strcat(message, "\"");
        perror(message);
        return 1;
    }

    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL)
    {
        // Citando la documentazione: "In no event does tree print the file system
        // constructs '.' (current directory) and '..' (previous directory)".
        if (strcmp(".", ent->d_name) == 0 || strcmp("..", ent->d_name) == 0)
            continue;

        // Skippa se il file e' nascosto e l' "-a" bit non e' settato.
        if (ent->d_name[0] == '.' && !(flags >> A_OP & 1))
            continue;

        // Concatena il name al path precedente
        char *full_path;
        full_path = (char *)malloc(strlen(path) + strlen(ent->d_name) + 2);
        // quell'2 e' per la stringa '/' ed il null terminator '/0
        strcpy(full_path, path);
        strcat(full_path, "/");
        strcat(full_path, ent->d_name);

        // Vedi se c'è un'altra dir dopo questa
        long int current_index = telldir(dir); // Salva l'index del current file
        // vai avanti nella dir e vedi se c'e' altro
        if (readdir(dir) == NULL)
            level_mask |= (1 << level);
        seekdir(dir, current_index); // Torna indietro

        // Il file stat contenente informazioni essenziali.
        struct stat f_stat;
        if (stat(full_path, &f_stat) != 0) {
            perror("An error has occurred while trying to read file's informations.");
            return 1;
        }

        // Se il file e' una DIRECTORY
        if (S_ISDIR(f_stat.st_mode) == 1)
        {
            (*dir_count)++;
            // Stampa il full path se "-f"
            _print_name(level, flags >> F_OP & 1 ? full_path : ent->d_name, level_mask, flags, f_stat);
            _treeR(level + 1, full_path, dir_count, file_count, flags, level_mask);
        }
        // Se non e' dir, stampa solo se non e' settato il bit di stampare solo dir.
        else if (!(flags >> 2 & 1))
        {
            (*file_count)++;
            _print_name(level, flags >> F_OP & 1 ? full_path : ent->d_name, level_mask, flags, f_stat);
            // !!!!NON SO SE -ad stampa solo directory, incluse quelle nascoste! IO HO FATTO COSI!!!!!!
        }

        free(full_path);
    }

    closedir(dir);

    return 0;
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

    // Stampa gli spazi prima del nome.
    for (int i = 0; i < level; i++)
        printf("%s    ", level_mask >> i & 1 ? " " : "│");

    // brakets = e' stato passato almeno un argomento che richiede le brackets?
    int brakets = (arg_mask >> 4 & 1 );

    char date[20];
    strftime(date, sizeof(date), "%d-%m-%y", localtime(&(f_stat.st_mtime)));
    
    printf("%s── %s%s%s%s\n", 
        level_mask >> level & 1 ? "└" : "├",
        brakets == 1 ? "[" : "",
        arg_mask >> 4 & 1 ? date : "",
        brakets == 1 ? "] " : "",
        name);

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

static void _pars_argv(int argc, char **argv, unsigned short *flags, char *path)
{
    char map[5][7] = {"--help", "-a", "-d", "-f", "-D"};
    // Parsing arguments
    for (int i = 1; i < argc; i++)
    {
        int j;
        for (j = 0; j < 5; j++)
        {
            if (strcmp(argv[i], map[j]) != 0)
                continue;
            *(flags) |= 1 << j;
            break;
        }
        /* Se il ciclo finisce senza un break, l'argv[i] non era un "singolo" aromgneto,
        quindi:
            - o e' un path (se non era nemmeno un path dara' errore all'apertura, quindi
            non ci interessa controllarlo);
            - o e' una stringa contenente piu' argomenti (e.g. -adf). */
        if (j == 5)
            strcpy(path, argv[i]);
    }
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
    unsigned short flags = 0b0; // 16 bit: adfpsugD
    _pars_argv(argc, argv, &flags, path);

    // Se il comando e' help.
    if ((flags >> HELP_OP) & 1)
        printf("Documentazione");
    else
    {
        printf("%s\n", path); // printa la directory iniziale su cui si scorre
        _treeR(0, path, &dir_count, &file_count, flags, 0b0);
        printf("\n%d directories, %d files", dir_count, file_count);
    }
}