#include "parent_lib.h"

int main(int argc, char* argv[], char* envp[]) {

    if(argc < 2) {
        fprintf(stderr, "Invalid cmd args");
        exit(EXIT_FAILURE);
    }
    qsort(envp, getNumberOfEnvironmentVariables(envp), sizeof(char*), strCompare);
    printEnvironmentVariables(envp);

    char operation;
    short counter = 0;
    while(scanf(" %c", &operation) && operation != 'q' && counter < 100) {
        switch (operation)
        {
            case '+':
                createChildProcess("+", argv[1], envp, &counter);
                break;
            case '*':
                createChildProcess("+", argv[1], envp, &counter);
                break;
            case '&':
                createChildProcess("+", argv[1], envp, &counter);
                break;
            default:
                fprintf(stderr, "Invalid operation");
                exit(EXIT_FAILURE);
        }
    }
    return 0;
}






















// #include <stdio.h>
// #include <stdlib.h>
// #include "lib.h"

// int main(int argc, char* argv[], char* envp[]) {
//   char option;
//   ssize_t limit = 0;
//   qsort(envp, numberOfEnvironmentVariables(envp), sizeof(char*), compare);
//   printEnvironmentVariables(envp);
  
//   while(scanf(" %c", &option) && option != 'q') {
//     switch (option)
//     {
//       case '+':
        
//         break;
//       case '*':

//         break;
//       case '&':

//         break;
//       default:
//         fprintf(stderr, "Unknown option");
//         exit(EXIT_FAILURE);
//     }
//   }
//   return 0;
// }