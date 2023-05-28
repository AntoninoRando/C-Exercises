#include <sys/stat.h>

/***********
 * Prototipo delle Funzioni
 ***********/
// NON HO CAPITO A COSA SERVE DICHIARARE IL PROTOTIPO... uso i nomi anche qui per la documentation.

/**
 * @brief Prints the file name, in the right level, along with its info.
 *
 * @param level The level of depth in which the file dwells.
 * @param name File name.
 * @param level_mask Which levels reached the end in the format: i-bit is 1 is level i reached the end, 0 otherwise.
 * @param arg_mask  Which parameters were passed to the main.
 * @param f_stat File info.
 *
 * @return 0 if no errors occurred, 1 otherwise.
 */
static void _print_name(int level, char *name, unsigned int level_mask, unsigned short arg_mask, struct stat f_stat);

void _print_file_mode(struct stat f_stat);

void print_args(unsigned short arg_mask, struct stat f_stat);

void print_colorized(const char *name, struct stat f_stat);

void print_help();