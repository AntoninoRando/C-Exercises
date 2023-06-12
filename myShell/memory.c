// #include "shell.c"

// #define MEMORY_FILE "./memory.txt"

// char memory[INPUT_SIZE][500]; // TODO: SONO IN ORDINE GIUSTO I DUE NUMERI?
// int memoryCounter = -1;

// static void load_shell_chronology()
// {
//     FILE *filePointer = fopen(MEMORY_FILE, "r+"); // r+ for both reading and writing

//     if (filePointer == NULL)
//     {
//         return; // TODO: HANDLE ERROR
//     }

//     int i = 0;
//     while(fgets(memory[i], INPUT_SIZE, filePointer))
//     {
//         i++;
//         memoryCounter++;
//     }

//     fclose(filePointer);
// }

// static void print_up()
// {
//     if (memoryCounter < 0)
//     {
//         return;
//     }

//     printf("%s", memory[memoryCounter]);
//     memoryCounter--;    
// }
