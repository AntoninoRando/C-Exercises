
/**
 * @brief A node of a double-linked-list that represents a file. It contains its name and stat strcut.
 * If the file is a dir, it also contains its full-path, NULL otherwise. 
 */
typedef struct file_node file_node;

/**
 * @brief Concatenates a path with a file name and returns a pointer to the resulting string.
 * 
 * @param path The path prefix.
 * @param f_name The path resolving-file.
 * @return A pointer to the resulting string.
 */
static char *_get_full_path(const char *path, const char *f_name);

/**
 * @brief Makes a sorted double-linked-list of files and put its head inside `head` variable.
 *
 * @param head
 * @param path
 * @param arg_mask
 * @return Returns 1 if an error occurred, 0 otherwise.
 */
int fill_list(struct file_node **head, const char *path, unsigned short arg_mask);

/**
 * @brief Insert a node at the bottom of the list, then move it up until it reaches the right position.
 * List's head ortail may change during the insertion.
 *
 * @param head The list's head, that is the node with the greatest sorting priority
 * (or, if you prefer, the first to be displayed).
 * @param tail The list's tail, that is the node with the lowest sorting priority
 * (or, if you prefer, the last to be displayed).
 * @param node The node that has to be inserted in the right position.
 * @param flags Which arguments where given.
 */
static void _insert(struct file_node **head, struct file_node **tail, struct file_node *node, unsigned short flags);

/**
 * @brief Checks if a node is in the right position accordignly to the sorting arguments (or default sorting).
 * Let's suppose that all arguments were given, thys: --dirsfirst comparison is performed first, then -t in
 * case of parity, them -r in parity again (like the tree function).
 * 
 * @param node The node that has to be checked.
 * @param flags Which parameters where given.
 * @return 0 if the node is NOT in the right position, 1 otherwise.
 */
static int _check_position(struct file_node *node, unsigned short flags);

/**
 * @brief Moves a node one position up in the linked list (that is, toward the head).
 *
 * @param node The link that has to be moved.
 */
static void _move_up(struct file_node *node);