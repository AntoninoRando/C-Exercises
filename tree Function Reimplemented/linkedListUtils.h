
typedef struct file_node file_node;

int fill_list(struct file_node **head, const char *path, unsigned short flags);

static void _insert_by_dir(struct file_node **head, struct file_node *dir_node);

static void _insert(struct file_node **head, struct file_node **last_node, struct file_node *node);