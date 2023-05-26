
typedef struct file_node file_node;

int fill_list(struct file_node **head, const char *path, unsigned short flags);

/**
 * @brief Sposta un nodo, che sta gia' in coda, nella posizione che tiene conto dei parametri
 * -r, -t e --dirsfirst. Se non ci sono parametri, la funzione non fa nulla.
 *
 * @param head
 * @param node
 * @param flags
 * @return Il puntatore alla coda della linked list.
 */
static void _insert(struct file_node **head, struct file_node **tail, struct file_node *node, unsigned short flags);

static int _respect_dirsfirst(struct file_node *node);

static int _respect_r(struct file_node *node);

static int _respect_t(struct file_node *node);

static int _is_position_ok(struct file_node *node, unsigned short flags);

static void _move_up(struct file_node *node);