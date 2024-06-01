#include <sys/stat.h>

/**
 * @brief Prints the file name, in the right level, along with its info.
 *
 * @param level The level of depth in which the file dwells.
 * @param name The file name that has to be printed.
 * @param level_mask Which levels reached the end in the format: i-bit is 1 is level i reached the end, 0 otherwise.
 * @param arg_mask  Which parameters were passed to the main.
 * @param f_stat File info.
 */
void print_name(int level, char *name, unsigned int level_mask, unsigned short arg_mask, struct stat f_stat);

/**
 * @brief Prints the file mode in the tree-function format.
 *
 * @param f_stat The file info.
 */
static void _print_file_mode(struct stat f_stat);

/**
 * @brief If optional arguments were given, print the square brackets and the additional file-info the user requested.
 *
 * @param arg_mask Which arguments were given.
 * @param f_stat The file info.
 */
void print_args(unsigned short arg_mask, struct stat f_stat);

/**
 * @brief Prints the file name. If the LS_COLORS is set, the file is also colored.
 * 
 * @param name 
 * @param f_stat 
 */
void print_colorized(const char *name, struct stat f_stat);

/**
 * @brief Prints the documentation requested with --help.
 * 
 */
void print_help();