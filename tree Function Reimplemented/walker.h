static int _treeR(int, const char *, int *, int *, unsigned short, unsigned int, int); // passo char come puntatore perche' e' un array di carratteri, cioe' una stringa

int _pars_argv(int, char **, unsigned short *, char *, int *);

static char *_get_full_path(const char *, const char *);

int tree(int, char **);