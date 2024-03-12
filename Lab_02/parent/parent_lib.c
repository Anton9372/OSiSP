#include "parent_lib.h"

int strCompare(const void* a, const void* b) {
    return strcoll(*(const char**)a, *(const char**)b);
}

ssize_t getNumberOfEnvironmentVariables(char* envp[]) {
    ssize_t counter = 0;
    while(*envp != NULL) {
        counter++;
        envp++;
    }
    return counter;
}

void printEnvironmentVariables(char* envp[]) {
    while(*envp != NULL) {
        printf("%s\n", *envp);
        envp++;
    }
}

const char* findEnvpVariableValue(char* envp[], const char* nameOfVariable) {
    for(size_t i = 0; envp[i] != NULL; ++i) {
        if(strncmp(envp[i], nameOfVariable, strlen(nameOfVariable)) == 0) {
            return envp[i] + strlen(nameOfVariable) + 1;
        }
    }
    return NULL;
}

const char* findEnvironVariableValue(const char* nameOfVariable) {
    extern char** environ;
    for(ssize_t i = 0; i < getNumberOfEnvironmentVariables(environ); ++i) {
        if(strncmp(environ[i], nameOfVariable, strlen(nameOfVariable)) == 0) {
            return environ[i] + strlen(nameOfVariable) + 1;
        }
    }
    return NULL;
}

void createChildProcess(char* operation, char* fileName, char* envp[], short* counter) {
    int status;
    char childName[9];
    sprintf(childName, "%s%d", "child_", (*counter)++);
    pid_t pid = fork();
    if(pid == 0) {
        char* childArgv[] = {childName, fileName, operation , NULL};
        switch(operation[0]) {
            case '+' :
                status = execve(getenv("CHILD_PATH"), childArgv, envp);
                break;
            case '*' :
                status = execve(findEnvpVariableValue(envp, "CHILD_PATH"), childArgv, envp);
                break;
            case '&' :
                status = execve(findEnvironVariableValue("CHILD_PATH"), childArgv, envp);
                break;
        }
        if(status == -1) {
            fprintf(stderr, "Failed to launch the program instance");
            exit(EXIT_FAILURE);
        } 
    }
    else if (pid == -1) {
        fprintf(stderr, "Failed to create child process");
        exit(EXIT_FAILURE);
    }
    else {
        if(waitpid(pid, &status, 0) == -1) {
            fprintf(stderr, "Failed to block parent process");
            exit(EXIT_FAILURE);
        }   
    }
}