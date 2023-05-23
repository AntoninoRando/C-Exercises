#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// TODO: AGGIUNGERE UNA STRUCT PER LA ARGS_MASK CHE BASTI FARE struct.a PER CAPIRE SE E' SETTATO, COSI' NON MI
// CONFONDO CON I NUMERI.
/***********
 * Prototipo delle Funzioni
 ***********/
// NON HO CAPITO A COSA SERVE DICHIARARE IL PROTOTIPO
static void _print_name(int, char *, unsigned int, unsigned short, struct stat);
static int _treeR(int, const char *, int *, int *, unsigned short, unsigned int); // passo char come puntatore perche' e' un array di carratteri, cioe' una stringa
int _pars_argv(int, char **, unsigned short *, char *);
int tree(int, char **);

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
        perror(path);
        return 1; // ATTENZIONE, VA CONTROLLATO PURE SULLA RICORSIONE
    }

    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL)
    {
        // Citando la documentazione: "In no event does tree print the file system
        // constructs '.' (current directory) and '..' (previous directory)".
        if (strcmp(".", ent->d_name) == 0 || strcmp("..", ent->d_name) == 0)
            continue;

        // Skippa se il file e' nascosto e l' "-a" bit non e' settato.
        if (ent->d_name[0] == '.' && !(flags >> 3 & 1))
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
        if (stat(full_path, &f_stat) != 0)
        {
            perror("An error has occurred while trying to read file's informations.");
            return 1;
        }

        // Se il file e' una DIRECTORY
        if (S_ISDIR(f_stat.st_mode) == 1)
        {
            (*dir_count)++;
            // Stampa il full path se "-f"
            _print_name(level, flags >> 5 & 1 ? full_path : ent->d_name, level_mask, flags, f_stat);
            _treeR(level + 1, full_path, dir_count, file_count, flags, level_mask);
        }
        // Se non e' dir, stampa solo se non e' settato il bit di stampare solo dir -d.
        else if (!(flags >> 4 & 1))
        {
            (*file_count)++;
            _print_name(level, flags >> 5 & 1 ? full_path : ent->d_name, level_mask, flags, f_stat);
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
    int brakets = (arg_mask >> 10 & 1); // Aggiungere anche altri

    char date[20];
    strftime(date, sizeof(date), "%d-%m-%y", localtime(&(f_stat.st_mtime)));

    printf("%s── %s%s%s%s\n",
           level_mask >> level & 1 ? "└" : "├",
           brakets == 1 ? "[" : "",
           arg_mask >> 10 & 1 ? date : "", // Stampa la data se e' stato settato il bit -D
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

int _pars_argv(int argc, char **argv, unsigned short *flags, char *path)
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
                for (c2 = 0; c2 < 12; c2++)
                {
                    if (argv[i][c] != "adfpsugDrtL"[c2])
                        continue;
                    *(flags) |= 1 << (c2 + 3); // i primi 3 sono help, inodes e dirsfirst
                    break;
                }
                if (c2 == 12) // Se avesse trovato un argomento valido il ciclo sarebbe finito con un break (prima di c2 = 11)
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
    unsigned short flags = 0b0;                    // 16 bit: --help --inodes --dirsfirst -adfpsugDrtL
    if (_pars_argv(argc, argv, &flags, path) == 1) // Se gli argomenti sono stati passati male.
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
        int errOrNot = _treeR(0, path, &dir_count, &file_count, flags, 0b0);
        printf("\n%d directories, %d files", dir_count, file_count);
        return errOrNot;
    }
}