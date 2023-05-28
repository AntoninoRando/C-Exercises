/**
 * @brief The tree function that was mean to be re-implemented.
 * 
 * @param argc The numbers of args passed.
 * @param argv The actual arg vector.
 * @return 1 if the args were passed wrong, 0 otherwise.
 */
int tree(int argc, char **argv);

/**
 * @brief The actual tree recursive implementation.
 * 
 * @param level The level to print.
 * @param path The path to the folder to recursively print.
 * @param dir_count The total dirs encountered.
 * @param file_count The total files encountered.
 * @param arg_mask  Which arguments were passed.
 * @param level_mask Which levels has ended.
 * @param max_level The max level imposed by -L (0 if unbounded).
 * @return 1 if the directory could not be visited, 0 otherwise.
 */
static int _treeR(int level, const char *path, int *dir_count, int *file_count, unsigned short flags, unsigned int arg_mask, int max_level);

/**
 * @brief Converts the arg vector into a bit-mask.
 * 
 * @return 1 if the args were passed wrong, 0 otherwise. 
 */

/**
 * @brief Converts the arg vector into a bit-mask.
 * 
 * @param argc The numbers of args passed.
 * @param argv The actual arg vector.
 * @param arg_mask The arg mask to fill.
 * @param paths The paths on which the tree function will be called.
 * @param max_level The max level of depth established by -L parameter.
 * @param paths_num Number of paths encountered.
 * @return 1 if the args were passed wrong, 0 otherwise.  
 */
static int _pars_argv(int argc, char **argv, unsigned short *arg_mask, char ***paths, int *max_level, int *paths_num);