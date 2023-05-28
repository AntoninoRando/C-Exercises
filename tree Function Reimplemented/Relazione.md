I parametri sono convertiti in una bit-mask secondo la seguente logica:
- ad ogni parametro ho associato un numero (e.g. ad "--help" corrisponde 0, ad "-a" corrisponde 1);
- se l'utente ha passato il parametro associato al numero i, allora l'i-esimo bit della mask è 1, altrimenti è 0.

A proposito di paramtri, una questione importante è: come si comporta la funzione se una directory ha lo stesso nome di uno o più parametri?
Esaminando il comportamento della funzione tree ufficiale, ho concluso che in questo caso ambiguo si prioritizza...


La bit-mask viene usata anche per capire se si è giunti alla fine di una directory; in tal caso, il carattere del "link" dell'albero cambia (i.e. non prosegue più all'elemento dopo). Se la directory che stiamo scorrendo è giunta alla fine, e tale directoy si trova all'i-esimo livello, allora l'i-esimo bit della mask è settato.

La _treeR compie le seguenti azioni in ordine.
1. Citando la documentazione "In no event does tree print the file system constructs '.' (current directory) and '..' (previous directory)". Quindi tali file vengono subito evitati.
2. Se il file è nascosto e l'"-a" bit (3) non è settato, evita questo file.
...
x. Se il file è una directory, stampa il nome e chiama ricorsivamente la funzione.
x. Se invece era un file normale E non è settato il "-d" bit (4):
    - se è settato il "--dirsfirst" (2), chiama prima ricorsivamente la funzione (per vedere se c'è altro dopo) 


Per fare le cartele prima:
- quando incontra una dir la stampa;
- quando incontra un file, richiama la funzione in modo che parta dal file dopo. Al ritorno, stamperà il file.

... DA FINIRE

# The Printing-Process
The program does not print one string all in once, it instead print several pieces one by one, in the following order.

1. The first thing to be printed are the white-spaces:

    - the '│' character and 3 whitespaces for every level above this file;
    - _or_, if this is the last file of the super-level, the '│' character is replaced with another whitespace.

    To determine which super-levels are came to their last file, a `level_mask` is used. It is a binary number: if the i-level is terminated, the i-bit is set to 1.

2. If at least one argument that requires brakets to be printed has been give, the opening square braket '[' is printed.

## LS_COLORS
I read in the documentation that `tree` colors files using the LS_COLORS environment variable. I tried to understand how it worked and I understood the following: _you can write that varible in the format `file_type=terminal_color_format:...`_. So i parsed that variable with this convinction. I have to implement a dictionary, but I had no time so I execute the parsing every time.

# The Sorting-Process
I've notice that `tree` functions sorts alphabetically by default and it is case _in_-sensitive, thus I used the `strcasecmp` function instead of `strcmp`.