#include "child_lib.h"
#define SIZE 255

void optionPlus(const char* fileName) {
    FILE* file = fopen(fileName, "r");
    if(file == NULL) {
        fprintf(stderr, "Failed to open file");
        exit(EXIT_FAILURE);
    }
    char* buffer = (char*)calloc(SIZE, sizeof(char));
    if(buffer == NULL) {
        fprintf(stderr, "Failed to allocate memory");
        exit(EXIT_FAILURE);
    }
    while(fgets(buffer, SIZE, file) != NULL) {
        char *newline = strchr(buffer, '\n');
        if (newline != NULL) {
            *newline = '\0';
        }
        printf("%s=%s\n", buffer, getenv(buffer));
    }
    fclose(file);
}

void optionStar(const char* fileName, char* envp[]) {
    FILE* file = fopen(fileName, "r");
    if(file == NULL) {
        fprintf(stderr, "Failed to open file");
        exit(EXIT_FAILURE);
    }
    char* buffer = (char*)calloc(SIZE, sizeof(char));
    if(buffer == NULL) {
        fprintf(stderr, "Failed to allocate memory");
        exit(EXIT_FAILURE);
    }
    while(fgets(buffer, SIZE, file) != NULL) {
        char *newline = strchr(buffer, '\n');
        if (newline != NULL) {
            *newline = '\0';
        }   
        char** i = envp;
        while(*i != NULL) {
            if(strncmp(*i, buffer, strlen(buffer)) == 0) {
                printf("%s\n", *i);
            }
            i++;
        }
    } 
    fclose(file);
}

void optionAmpersand(const char* fileName) {
    extern char** environ;
    FILE* file = fopen(fileName, "r");
    if(file == NULL) {
        fprintf(stderr, "Failed to open file");
        exit(EXIT_FAILURE);
    }
    char* buffer = (char*)calloc(SIZE, sizeof(char));
    if(buffer == NULL) {
        fprintf(stderr, "Failed to allocate memory");
        exit(EXIT_FAILURE);
    }
    while(fgets(buffer, SIZE, file) != NULL) {
        char *newline = strchr(buffer, '\n');
        if (newline != NULL) {
            *newline = '\0';
        }
        char** i = environ;
        while(*i != NULL) {
            if(strncmp(*i, buffer, strlen(buffer)) == 0) {
                printf("%s\n", *i);
            }
            i++;
        }   
    }
    fclose(file);
}