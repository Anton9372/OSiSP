#include "child_lib.h"

int main(int argc, char* argv[], char* envp[]) {
    if(argc < 3) {
        fprintf(stderr, "Failed to find option");
        exit(EXIT_FAILURE);
    }
    printf("NAME: %s\n", argv[0]);
    printf("PID: %d\n", getpid());
    printf("PPID: %d\n", getppid());
    switch(argv[2][0]) {
        case '+':
            optionPlus(argv[1]);
            break;
        case '*':
            optionStar(argv[1], envp);
            break;
        case '&':
            optionAmpersand(argv[1]);
            break;
    }
    return 0;
}





































// #include <stdio.h>
// #include <unistd.h>
// #include "lib.h"

// int main(int argc, char* argv[], char* envp[]) {
//   printf("APPLICTION NAME: %s\n", argv[0]);
//   printf("PID: %d\n", getpid());
//   printf("PPID: %d\n", getppid());
//   if(argc == 2) {
//     char** name = NULL;
//     ssize_t numberOfNumbers = 0;
//     getEnvironmentVariableNames(argv[1], &name, &numberOfNumbers);
//     printEnvironmentVariableValues(name, numberOfNumbers);
//     freeNames(&name, &numberOfNumbers);
//   }
//   else {
//     fprintf(stderr, "Failed to open file");
//     exit(EXIT_FAILURE);
//   }
//   return 0;
// }